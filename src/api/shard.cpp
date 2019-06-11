
/*
 * Nekocord: A lightning fast discord library for c++
 * Copyright (C) 2018 Rebekah Rowe
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <neko/platform.hpp>
#include <rapidjson/writer.h>

#include "client.hpp"

namespace neko::discord::api {
using namespace std::string_view_literals;
using namespace std::chrono_literals;

Shard::Shard(BaseClient* _client, int _id) : client(_client), id(_id){
    this->HardReset();
    this->socket.SetMessageCallback([this](std::string_view msg){
        this->RecieveMessage(msg);
    });
    this->socket.SetErrorCallback([this](){
        std::cerr << "Shard: Unknown websocket error, reconnecting!" << std::endl;
        this->Disconnect(true);
    });
    this->Connect();
}
Shard::~Shard(){
    this->Disconnect(false);
}
void Shard::Connect() {
    if(this->socket.IsConnected())
        throw std::logic_error("Shard: Existing connection detected.");
    this->connect_attempts++;
    this->state = State::kConnecting;
    this->InitWebsocket();
}
void Shard::Disconnect(bool reconnect){

    if(this->heartbeat_interval.IsRunning())
        this->heartbeat_interval.Clear();

    try {
        if(reconnect && !this->session_id.empty()) {
            this->socket.Terminate();
        } else {
            this->socket.Close(1000, "Disconnect() called");
        }
    } catch(std::exception e) {
        std::cerr << "Error disconneting: " << e.what() << std::endl;
    }

    this->client->onDisconnect();

    this->Reset();
    if(reconnect && !this->session_id.empty()) {
        std::thread wait([this](){
            std::this_thread::sleep_for(std::chrono::milliseconds(this->reconnect_interval));
            this->Connect();
        });
        wait.detach();
        this->reconnect_interval = std::min<int>(std::floor(
                                       this->reconnect_interval *
                                       (std::rand() * 2 + 1)),
                                       30000);
    } else
        this->HardReset();
}

void Shard::Reset(){
    this->state = State::kDisconnected;

    this->last_heartbeat_ack = true;
    this->last_heartbeat_received.Reset();
    this->last_heartbeat_sent.Reset();
}

void Shard::HardReset() {
    this->Reset();
    this->sequence = 0;
    this->session_id.clear();
    this->reconnect_interval = 1000;
    this->connect_attempts = 0;
    this->heartbeat_interval.Clear();
}

// Send our auth info to discord
void Shard::Identify() {
    std::cout << "Shard: Idendifying" << std::endl;
    // Gather
    auto token = json::StringRef(this->client->token.data(),
                                 this->client->token.size());

    std::string_view os = "Unknown"sv; // save us the strlens
    if constexpr (plat::kLinux)
        os = "Linux"sv;
    else if constexpr (plat::kWindows)
        os = "Windows"sv;
    else if constexpr (plat::kApple)
        os = "Mac"sv;
    auto os_ref = json::StringRef(os.data(), os.size());

    json::Document msg(json::kObjectType);
    auto& alloc = msg.GetAllocator();
    msg.AddMember("op", 2, alloc); // Identify
    json::Value d(json::kObjectType); // "Data"
    d.AddMember("token", token, alloc);
    // Connection properties
    json::Value prop(json::kObjectType);
    prop.AddMember("$os", os_ref, alloc);
    prop.AddMember("$browser", "Nekocord", alloc);
    prop.AddMember("$device", "Nekocord", alloc);
    // wrap up json
    d.AddMember("properties", prop, alloc);
    msg.AddMember("d", d, alloc);

    this->Send(msg);
};

void Shard::Resume(){
    std::cout << "Shard: Resuming" << std::endl;
    this->state = State::kReconnecting;
    // Gather data
    auto token = json::StringRef(this->client->token.data(),
                                 this->client->token.size());
    auto ses_id = json::StringRef(this->session_id.data(),
                                  this->session_id.size());
    // Write
    json::Document msg(json::kObjectType);
    auto& alloc = msg.GetAllocator();
    msg.AddMember("token", token, alloc);
    msg.AddMember("session_id", id, alloc);
    msg.AddMember("seq", this->sequence, alloc);
    this->Send(msg);
}

void Shard::Send(const json::Value& msg) {
    json::StringBuffer buf;
    json::Writer<json::StringBuffer> writer(buf);
    msg.Accept(writer);
    // Send away
    this->Send(std::string_view(buf.GetString(), buf.GetSize()));
}

void Shard::Heartbeat() {
    // Check for acknowledge
    if (!this->last_heartbeat_ack) {
        std::cerr << "Shard: Heartbeat not acknowledged, reconnecting!" << std::endl;
        this->Disconnect(true);
        return;
    }

    // Create our message
    json::Document msg(json::kObjectType);
    msg.AddMember("op", 1, msg.GetAllocator()); // Heartbeat
    msg.AddMember("d", this->sequence, msg.GetAllocator());
    // Expect a pong
    this->last_heartbeat_sent.Reset();
    this->last_heartbeat_ack = false;
    this->Send(msg);
}

void Shard::InitWebsocket() {
        if(this->client->token.empty()) {
            try {
                this->Disconnect(false);
            } catch(...){}
            throw std::logic_error("Shard: Token not specified");
        }

        this->state = State::kConnecting;

        this->socket.Start(RestAPI::GetGateway());
        std::thread wait([this](){
            std::this_thread::sleep_for(5s);
            if (this->state == State::kConnecting) {
                this->Disconnect(true);
                std::cerr << "Shard: Connection timeout" << std::endl;
            }
        });
        wait.detach();
}

void Shard::RecieveMessage(std::string_view raw) {
    json::Document msg;
    msg.Parse(raw.data(), raw.size());

    // Save our sequence
    auto find = msg.FindMember("s");
    if (find != msg.MemberEnd() && !find->value.IsNull()) {
        int s = find->value.GetInt();
        if(s > this->sequence + 1 && this->state != State::kReconnecting)
            std::cerr << "Shard: Non-consecutive sequence " << this->sequence << " -> " << s << std::endl;
        this->sequence = s;
    }
    // Do events
    const json::Value& op = msg["op"];
    switch(op.GetInt()) {
    case 0: { // Dispatch
        json::Value& t = msg["t"]; // Event
        json::Value& d = msg["d"]; // Event data
        std::string_view event(t.GetString(), t.GetStringLength());
        // Ready handler
        if (this->state != State::kReady &&
            (event == "READY"sv || event == "RESUMED"sv)) {

            this->connect_attempts = 0;
            this->reconnect_interval = 1000;
            this->state = State::kReady;
            json::Value& s = d["session_id"];
            this->session_id = std::string(s.GetString(), s.GetStringLength());
        }
        this->client->EmitEvent(event, d);
        break;
    }
    case 1: { // Heartbeat
        this->Heartbeat();
        break;
    }
    case 9: { // Invalid session
        this->session_id.clear();
        std::cerr << "Shard: Invalid session, reidentifying!" << std::endl;
        this->Identify();
        break;
    }
    case 7: { // Reconnect
        this->Disconnect(true);
        break;
    }
    case 10: { // Hello
        json::Value& d = msg["d"];
        int hb_int = d["heartbeat_interval"].GetInt();
        if (hb_int > 0) {
            if(!this->heartbeat_interval.IsRunning()) {
                this->heartbeat_interval.SetErr([this](std::exception err){
                    std::cerr << err.what() << std::endl;
                });
                this->heartbeat_interval.Set([this](){
                   this->Heartbeat();
               }, hb_int);
            }

            this->state = State::kNearly;

            if(!this->session_id.empty()) {
               this->Resume();
            } else {
               this->Identify();
            }
            this->Heartbeat();
            break;
        }
    }
    case 11: { // Heartbeat ack
        this->last_heartbeat_ack = true;
        this->last_heartbeat_received.Reset();
        break;
    }
    default: {
        std::cerr << "Shard: Recieved unknown event\n\t" << raw << std::endl;
    }}
}

}
