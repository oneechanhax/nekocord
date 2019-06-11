
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

#include "api/rest.hpp"
#include "presence.hpp"
#include "snowflake.hpp"

namespace neko::discord {
namespace json = rapidjson;

namespace api {
class Shard;
}
class Guild;
class GuildMember;
class Message;
class Channel;
class DMChannel;
class GroupDMChannel;
class Emoji;
class BaseClient {
public:
    BaseClient();
    void Login(std::string_view token, int num_shards = 1);
    void Disconnect();
    void GetToken();

// Events
protected:
    virtual void onReady(){}
    virtual void onGuildCreate(Guild*){}
    virtual void onMessageCreate(Message*){}
    virtual void onChannelCreate(Channel*){}
    virtual void onGuildUpdate(Guild*) {}
    virtual void onDisconnect() {}

// Info retrieval
public:
    const User* FetchClientUser();
    User* FetchUser(Snowflake id, bool cache = true); // Whether or not to use the cache
    Guild* FetchGuild(Snowflake id, bool cache = true);
    Emoji* FetchEmoji(Snowflake id);
    Channel* FetchChannel(Snowflake id);
    // This fetch user takes in data and caches it
    User* FetchUser(Snowflake, const json::Value&);
    User* FetchUser(const json::Value& v);
    Guild* FetchGuild(Snowflake, const json::Value&);
    Guild* FetchGuild(const json::Value& v);

// Internal events
private:
    // Cache
    void onReady(const json::Value&);
    void onGuildCreate(const json::Value&);
    void onMessageCreate(const json::Value&);
    void onChannelCreate(const json::Value&);
    void onGuildUpdate(const json::Value&);
    User* user;
    std::unordered_map<Snowflake, Channel*> channels;
    std::unordered_map<Snowflake, Emoji*> emojis;
    std::unordered_map<Snowflake, Guild*> guilds;
    std::unordered_map<Snowflake, Presence> presences;
    std::unordered_map<Snowflake, User*> users;

    // Connection info
    std::string token;
    friend api::RestAPI;
    api::RestAPI http;
    friend api::Shard;
    std::vector<api::Shard*> shards;
    void EmitEvent(std::string_view event, const json::Value& data);
    // easy access
    friend Channel;
    friend Guild;
    friend Emoji;
    friend GuildMember;
};

}
