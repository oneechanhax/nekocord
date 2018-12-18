
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

#include <rapidjson/document.h>

#include "snowflake.hpp"

namespace neko::discord {

// https://discordapp.com/developers/docs/resources/user
class BaseClient;
class User {
public:
    User(BaseClient& _client, Snowflake id, const rapidjson::Value&);
    BaseClient& client;

    Snowflake id;
    std::string username;
    std::string discriminator;
    std::string avatar;
    bool bot = false;
};

}
