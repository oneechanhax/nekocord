
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

#include "web/http.hpp"
#include "web/websocket.hpp"
#include "presence.hpp"
#include "snowflake.hpp"

namespace neko::discord {

// Extracts "id" from json. Usefil for the other classes
Snowflake ExtractId(const rapidjson::Value&);

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
    void Login(std::string_view token);
// Events
protected:
    virtual void onReady(){}
    virtual void onGuildCreate(Guild&){}
    virtual void onMessageCreate(Message&){}
    virtual void onChannelCreate(Channel&){}
    virtual void onGuildUpdate(Guild&) {}
// Info retrieval
public:
    const User& FetchClientUser();
    //Invite FetchInvite(std::string_view);
    User& FetchUser(Snowflake id, bool cache = true); // Whether or not to use the cache
    Guild& FetchGuild(Snowflake id, bool cache = true);
    Emoji& FetchEmoji(Snowflake id);
    Channel& FetchChannel(Snowflake id);
    //std::vector<VoiceRegion> FetchVoiceRegions();
    //Webhook FetchWebhook(Snowflake id, std::string_view = std::string_view());
    //std::string GenerateInvite()
    //void SyncGuilds();

// Internal events
private:
    void onReady(const rapidjson::Value&);
    void onGuildCreate(const rapidjson::Value&);
    void onMessageCreate(const rapidjson::Value&);
    void onChannelCreate(const rapidjson::Value&);
    void onGuildUpdate(const rapidjson::Value&);

// Cache
    User* user;
    std::vector<Channel*> channels;
    std::vector<Emoji*> emojis;
    std::vector<Guild*> guilds;
    std::vector<Presence> presences;
    std::vector<User*> users;

    friend Guild;
    friend Channel;
    friend DMChannel;
    friend GroupDMChannel;
    friend Emoji;
    friend GuildMember;
    friend Message;
    // This fetch user takes in data and caches it
    User& FetchUser(Snowflake, const rapidjson::Value&);
    inline User& FetchUser(const rapidjson::Value& v) {
        return this->FetchUser(ExtractId(v), v);
    }
    Guild& FetchGuild(Snowflake, const rapidjson::Value&);
    inline Guild& FetchGuild(const rapidjson::Value& v) {
        return this->FetchGuild(ExtractId(v), v);
    }

// Connection info
    std::string token;

    friend web::HttpMgr;
    web::HttpMgr http;

    friend web::Websocket;
    web::Websocket websocket;
    std::string session_id;
    void EmitEvent(std::string_view event, const rapidjson::Value& msg);
};

}
