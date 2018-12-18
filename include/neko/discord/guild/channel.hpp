
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

#include <vector>

#include <rapidjson/document.h>

#include "../snowflake.hpp"

namespace neko::discord {

class PermissionOverwrite {
public:
    PermissionOverwrite(const rapidjson::Value&);
    Snowflake id;
    std::string type;
    int deny;
    int allow;
};

class Channel;
class Guild;
class GuildChannel {
protected:
    friend Channel;
    GuildChannel(Channel&, Guild&, const rapidjson::Value&);
    ~GuildChannel();
    void Update(const rapidjson::Value&);
public:
    Channel& channel;
    Guild& guild;

    std::string name;
    bool nsfw = false;
    std::vector<PermissionOverwrite> permission_overwrites;
    int position;
    Snowflake parent_id;
    //GuildChannel* parent;
};

}
