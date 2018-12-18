
#include <neko/platform.hpp>
#include <rapidjson/writer.h>

#include "client.hpp"

#include "web/websocket.hpp"

namespace neko::discord::web {
using namespace std::chrono_literals;
using namespace std::string_view_literals;

Websocket::Websocket(BaseClient& _client) : client(_client) {
    // Setup the socket
    this->socket.clear_access_channels(websocketpp::log::alevel::all);

    this->socket.set_tls_init_handler([this](websocketpp::connection_hdl) {
		return websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::tlsv1);
    });

    this->socket.init_asio();
    this->socket.set_message_handler(
        std::bind(&Websocket::Recieve, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
}

Websocket::~Websocket() {
    if (this->state != State::kDisconnected)
        this->Disconnect(0, "Client closing.");
}

void Websocket::Connect(const std::string& uri) {
    // this->Disconnect(); // commented cuz idk why
    // Ensure we are ready to connect
    if (!this->socket_mutex.try_lock())
        throw std::logic_error("Websocket thread in use.");

    this->expecting_close = false;
    // Setup our connection
    websocketpp::lib::error_code ec;
    Socket::connection_ptr con = this->socket.get_connection(uri, ec);
    if (ec) {
        if (this->state == State::kReconnecting)
            return this->Reconnect();
        else
            throw std::runtime_error(ec.message());
    }
    this->handle = con->get_handle();
    this->socket.connect(con);

    // Start the thread and give the socket guard to it
    this->state = State::kConnecting;
    std::thread thread([this](){
        std::lock_guard<std::mutex> guard(this->socket_mutex);
        this->socket.run();
        this->state = State::kDisconnected;
        // If we wernt expecting it, we have to deal with it
        if (!this->expecting_close)
            this->Reconnect();
    });
    thread.detach();
    this->socket_mutex.unlock();
}

void Websocket::Reconnect() {
    std::async([this](){ // Im sorry for any hung threadpools ;(
        // Make sure everything is reset
        this->Disconnect(0, "Reconnecting!");
        puts("Reconnect in 5500ms...");
        std::this_thread::sleep_for(5500ms);
        this->state = State::kReconnecting;
        this->Connect();
    });
}
// Stops any existing connection
void Websocket::Disconnect(int code, const std::string& reason) {
    this->expecting_close = true;

    // Disconnect the socket and ensure its stopped
    websocketpp::lib::error_code err;
    if (!this->handle.expired())
        this->socket.close(handle, code, reason, err);
    if (err)
        throw std::runtime_error("Failed to disconnect websocket.");
    std::lock_guard<std::mutex> guard(this->socket_mutex);

    // Wait until heartbeat thread saw our expecting_close
    std::lock_guard<std::mutex> guard2(this->heart_mutex);
}

void Websocket::Send(std::string_view msg) {
    websocketpp::lib::error_code ec;
    this->socket.send(this->handle, msg.data(), msg.size(),
        websocketpp::frame::opcode::text, ec);
    if (ec)
        throw std::runtime_error("Websocket: Unable to send message.");
}

void Websocket::Send(const rapidjson::Value& msg) {
    using namespace rapidjson;
    StringBuffer buf;
    Writer<StringBuffer> writer(buf);
    msg.Accept(writer);
    // Send away
    this->Send(std::string_view(buf.GetString(), buf.GetLength()));
}

void Websocket::Recieve(websocketpp::connection_hdl hdl, websocketpp::config::asio_client::message_type::ptr msg) {
    rapidjson::Document json;
    json.Parse(msg->get_payload().data(), msg->get_payload().size());
    const rapidjson::Value& op = json["op"];

    switch(op.GetInt()) {
    case 10: { // Hello
        // The first opcode we recieve on connection.
        // We are tasked with sending a heartbeat immediatly after recieving it.
        this->heart_interval =
            std::chrono::milliseconds(json["d"]["heartbeat_interval"].GetInt());
        this->SendHeartbeat();
        this->StartHeartbeat();
        // The server now wants auth data
        this->SendAuth();
        break;
    }
    case 11: // Heartbeat ACK(pong)
        this->heart_expecting_ack = false;
        break;
    case 0: { // Dispatch
              // this is when the server dispatches events to us
              // We'll just give these to the client to handle
        this->sequence = json["s"];
        rapidjson::Value& e = json["t"];
        std::string_view event(e.GetString(), e.GetStringLength());
        if (this->state != State::kReady && event == "READY"sv)
            this->state = State::kReady;
        this->client.EmitEvent(event, json["d"]);
        break;
    }
    default:
        printf("Connection: Unknown opcode %i\n", op.GetInt());
    }
}

// Heartbeat
void Websocket::SendHeartbeat() {
    using namespace rapidjson;
    // Create our message
    Document msg;
    msg.SetObject();
    msg.AddMember("op", 1, msg.GetAllocator()); // Heartbeat
    msg.AddMember("d", this->sequence, msg.GetAllocator());
    // Make it a string
    this->Send(msg);
    this->heart_timer.Reset();
    // Ensure we recieve an ACK(pong) back
    this->heart_timeout.Reset();
    this->heart_expecting_ack = true;
}
// Heartbeat thread stuff
void Websocket::StartHeartbeat(){
    // Ensure the heartbeat thread isnt running
    if (!this->heart_mutex.try_lock())
        throw std::logic_error("Heartbeat mutex already in use.");

    std::thread thread([this](){
        std::lock_guard<std::mutex>(this->heart_mutex);
        while (!this->expecting_close) {
            // Check if we should recieve a heartbeat
            if (this->heart_timer.CheckTime(this->heart_interval))
                this->SendHeartbeat();

            // Heartbeat ACK timeout
            if (this->heart_expecting_ack &&
                this->heart_timeout.CheckTime(5s)) { // is 500ms good enough?
                // Without a heartbeat, we need to close the connection and
                // request a new one.
                this->Reconnect();
                break;
            }
            std::this_thread::sleep_for(45ms); // so we dont kill the cpu ;-;
        }
    });
    thread.detach();
    this->heart_mutex.unlock();
}

// Send our auth info to discord
void Websocket::SendAuth() {
    using namespace rapidjson;
    Document msg(kObjectType);
    auto& alloc = msg.GetAllocator();

    auto token = StringRef(this->client.token.data(),
                           this->client.token.size());
    if (this->client.session_id.empty()) {
        // We dont have a session id, we need to craft an identity
        msg.AddMember("op", 2, alloc); // Identify

        Value d(kObjectType); // "Data"
        d.AddMember("token", token, alloc);

        // Connection properties
        Value prop(kObjectType);
        std::string_view os = "Unknown"sv; // save us the strlens
        if constexpr (plat::kLinux)
            os = "Linux"sv;
        else if constexpr (plat::kWindows)
            os = "Windows"sv;
        else if constexpr (plat::kApple)
            os = "Mac"sv;
        auto os_ref = StringRef(os.data(),
                                os.size());
        prop.AddMember("$os", os_ref, alloc);
        prop.AddMember("$browser", "Nekocord", alloc);
        prop.AddMember("$device", "Nekocord", alloc);

        d.AddMember("properties", prop, alloc);
        msg.AddMember("d", d, alloc);
    } else {
        // We have a id, we can use it to resume our session

        msg.AddMember("token", token, alloc);
        auto id = StringRef(this->client.session_id.data(),
                            this->client.session_id.size());
        msg.AddMember("session_id", id, alloc);
        msg.AddMember("seq", this->sequence, alloc);
    }
    this->Send(msg);
}

}
