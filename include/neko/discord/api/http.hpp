
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

#include <cpr/cpr.h>

#include "ratelimit.hpp"

namespace neko::discord {
class BaseClient;

namespace web {

// Handles http stuff uwu

class HttpMgr {
public:
    HttpMgr(BaseClient& _client);
    BaseClient& client;

    std::string Get(const std::string& url);
    // Returns status code
    std::int32_t Post(const std::string& url, const rapidjson::Value&);
    std::int32_t Post(const std::string& url, const std::string& msg);

    static std::string GetGateway(bool force = false); // for websocket
private:
    // Get the base header needed for auth
    // https://discordapp.com/developers/docs/reference#authentication
    // https://discordapp.com/developers/docs/reference#user-agent
    cpr::Header GetBaseHeader();
    // Ratelimiting is required
    RateLimiter rate_limiter;
};

}}
