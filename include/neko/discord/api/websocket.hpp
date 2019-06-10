
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

#include <thread>
#include <string_view>

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

namespace neko::discord::api {
namespace ws = websocketpp;

class Websocket {
public:
    using IntSocket = ws::client<ws::config::asio_tls_client>; // Internal
    using MsgFunc = std::function<void(std::string_view)>;
    using ErrFunc = std::function<void()>;
    Websocket();
    ~Websocket();
    void Start(const std::string& uri);
    void Close(int code, const std::string& reason);
    void Terminate();
    void Send(std::string_view msg);
    bool IsConnected() { return !this->stopped; }
    void SetMessageCallback(MsgFunc f) { this->msg_callback = f; };
    void SetErrorCallback(ErrFunc f) { this->err_callback = f; };
private:
    IntSocket socket;
    ws::connection_hdl handle;
    bool stopped = true;
    bool expecting_stop = false;
    MsgFunc msg_callback = [](auto){};
    ErrFunc err_callback = [](){};
};

}
