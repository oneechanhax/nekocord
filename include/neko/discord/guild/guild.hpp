
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

#include <unordered_map>

#include <rapidjson/document.h>

#include "emoji.hpp"
#include "member.hpp"
#include "role.hpp"

namespace neko::discord {
namespace json = rapidjson;

// TODO, what is "features" and what do i do with it???
class BaseClient;
class GuildChannel;
class Guild {
    friend BaseClient;
    Guild(BaseClient* _client, const json::Value&);
    Guild(BaseClient* _client, Snowflake, const json::Value&);
    Guild* Create(const json::Value&); // GUILD_CREATE
    Guild* Update(const json::Value&); // GUILD_UPDATE
    // GUILD_DELETE
    ~Guild();
public:
    BaseClient* client;
    bool unavailable; // The guild could be unavailable
    Snowflake id;

    std::unordered_map<Snowflake, GuildChannel*> channels;
    std::unordered_map<Snowflake, Emoji*> emojis;
    std::unordered_map<Snowflake, Role> roles;
    std::unordered_map<Snowflake, GuildMember*> members;
    int explicit_content_filter;
    std::string icon;
    std::string joined_at; // TODO, parse this into some actual cpp format
    int member_count; // could this overflow???
    int mfa_level;
    std::string name;
    Snowflake owner_id;

    GuildChannel* FetchChannel(Snowflake id, bool cache = true);
    Role* FetchRole(Snowflake id);
    GuildMember* FetchMember(Snowflake id, bool cache = true);
};

}
