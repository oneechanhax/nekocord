
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

#include <neko/chrono.hpp>
#include <neko/thread.hpp>
#include <rapidjson/document.h>

#include "websocket.hpp"
#include "http.hpp"

namespace neko::discord {
class BaseClient;

namespace api {
namespace json = rapidjson;

class Shard {
public:
    Shard(BaseClient& client, int id);
    ~Shard();
    BaseClient& client;
    const int id;

    enum class State {
        kReady, // We recieved our READY event
        kNearly, // We sent our IDENTITY, waiting for ready
        kConnecting, // Trying to connect to the socket
        kReconnecting, // Got disconnected, trying again
        kDisconnected, // We are not connected at all
    };
    inline State GetState(){ return this->state; }

    // Usage functions
    void Connect();
    void Disconnect(bool reconnect);
    inline void Send(std::string_view msg) { this->socket.Send(msg); };
    void Send(const json::Value& msg);

private:

    // Connection
    Websocket socket;
    void InitWebsocket();
    State state;
    int connect_attempts;
    int reconnect_interval;
    void Recieve(std::string_view);

    // Resuming
    std::string session_id;
    int sequence; // Sequence number for resuming

    // Heartbeat
    void Heartbeat();
    thread::Loop heartbeat_interval;
    bool last_heartbeat_ack;
    Timer last_heartbeat_sent;
    Timer last_heartbeat_received;

    // Utils
    void Reset();
    void HardReset();
    void Identify();
    void Resume();
};

}}
