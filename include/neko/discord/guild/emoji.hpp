
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

#include "../snowflake.hpp"

namespace neko::discord {

class BaseClient;
class Guild;
class Emoji {
    friend Guild;
public:
    Emoji(Guild&, const rapidjson::Value&);
    ~Emoji();
    BaseClient& client;
    Guild& guild;
    Snowflake id;

    bool animated;
    bool managed;
    std::string name;
    bool require_colons;
    std::vector<Snowflake> roles;
};

}
