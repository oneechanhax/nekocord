
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

#pragma once

#include <mutex>
#include <string_view>
#include <thread>

#include <neko/chrono.hpp>
#include <rapidjson/document.h>
#define ASIO_STANDALONE
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#include "http.hpp"

namespace neko::discord {
class BaseClient;

namespace web {

// A websocket manager
// One problem i have is that there are only so many threads i can have

class Websocket {
public:
    Websocket(BaseClient& client);
    ~Websocket();
    BaseClient& client;

    enum class State {
        kDisconnected, // We are not connected at all
        kConnecting, // Trying to connect to the socket
        kNearly, // We sent our IDENTITY, waiting for ready
        kReady, // We recieved our READY event
        kReconnecting, // Got disconnected, trying again
        kIdle // Got disconnected, waiting to reconnect
    };

    // Usage functions
    void Connect(const std::string& uri = HttpMgr::GetGateway());
    void Disconnect(int code, const std::string& reason);
    void Send(std::string_view msg);
    void Send(const rapidjson::Value& msg);

    inline State GetState(){ return this->state; }
private:
    State state = State::kDisconnected; // Internal state

    // Websocket connection
    using Socket = websocketpp::client<websocketpp::config::asio_tls_client>;
    Socket socket;
    websocketpp::connection_hdl handle;
    // Websocket thread
    std::mutex socket_mutex;
    bool expecting_close = false;
    // Connection events
    void Recieve(websocketpp::connection_hdl hdl, websocketpp::config::asio_client::message_type::ptr msg);

    // Heartbeat
    Timer heart_timer; // uses interval to send beats
    Timer heart_timeout; // timeout of ACK
    bool heart_expecting_ack = false;
    std::chrono::steady_clock::duration heart_interval;
    rapidjson::Value sequence; // Sequence number for resuming
    void SendHeartbeat();
    // Heartbeat Thread
    std::mutex heart_mutex;
    bool heart_thread_stop;
    void StartHeartbeat();
    void StopHeartbeat();

    // Utils
    void Reconnect();
    void SendAuth();
};

}}
