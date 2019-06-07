
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

#include "api/websocket.hpp"

namespace neko::discord::api {
namespace ws = websocketpp;
using namespace std::chrono_literals;

Websocket::Websocket(){
    // Setup the socket
    this->socket.clear_access_channels(ws::log::alevel::all);
    this->socket.set_tls_init_handler([](auto) {
        return ws::lib::make_shared<asio::ssl::context>(asio::ssl::context::tlsv1);
    });
    this->socket.init_asio();
    this->socket.set_message_handler([this](ws::connection_hdl hdl, ws::config::asio_client::message_type::ptr msg){
        this->msg_callback(std::string_view(msg->get_payload().data(), msg->get_payload().size()));
    });
}

Websocket::~Websocket(){
    if (this->IsConnected())
        this->Close(1000, "Websocket: Destructor called");
}

void Websocket::Start(const std::string& uri){
    // Setup our connection
    ws::lib::error_code ec;
    IntSocket::connection_ptr con = this->socket.get_connection(uri, ec);
    if (ec)
        throw std::runtime_error("Websocket error connecting: " + ec.message());
    this->handle = con->get_handle();
    this->socket.connect(con);
    std::thread thread([this](){
        this->expecting_stop = false;
        this->stopped = false;
        this->socket.run();
        this->stopped = true;
        if (!this->expecting_stop)
            this->err_callback();
    });
    thread.detach();
}

void Websocket::Close(int code, const std::string& reason) {
    this->expecting_stop = true;
    ws::lib::error_code err;
    //if (!this->handle.expired())
        this->socket.close(handle, code, reason, err);
    while(!this->stopped)
        std::this_thread::sleep_for(25ms);
}

void Websocket::Terminate() {
    this->expecting_stop = true;
    std::cerr << "Websocket: Terminate not implimented." << std::endl;
}

void Websocket::Send(std::string_view msg) {
    websocketpp::lib::error_code ec;
    this->socket.send(this->handle, msg.data(), msg.size(),
        ws::frame::opcode::text, ec);
    if (ec)
        throw std::runtime_error("Websocket: Unable to send message.");
}

}
