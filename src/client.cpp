
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

#include <fstream>

#include <neko/filesystem.hpp>
#include <rapidjson/writer.h>

#include "api/shard.hpp"
#include "guild/guild.hpp"
#include "channel.hpp"
#include "message.hpp"
#include "user.hpp"

#include "client.hpp"

namespace neko::discord {
namespace json = rapidjson;

BaseClient::BaseClient()
    : http(this) {}

void BaseClient::Login(std::string_view _token, int num_shards) {
    if (!this->shards.empty())
        throw std::logic_error("BaseClient: Shards already exist, cant login until they shutdown");
    this->token = _token;
    for (int i = 0; i < num_shards; i++)
        this->shards.push_back(new api::Shard(this, i));
}

void BaseClient::Disconnect() {
    for (api::Shard* s : this->shards)
        delete s;
}


// Events
// Our main role with the events is to effitently keep our cache updated
// Events send alot of data so we can do this as much as we can

void BaseClient::EmitEvent(std::string_view event, const json::Value& data) {
    using namespace std::string_view_literals;

    // Comparing string_views is extremely fast uwu
    // not sure if it compares size first but it should be fast
    if (event == "READY"sv)
        this->onReady(data);
    else if (event == "GUILD_CREATE"sv)
        this->onGuildCreate(data);
    else if (event == "MESSAGE_CREATE"sv)
        this->onMessageCreate(data);
    else if (event == "CHANNEL_CREATE"sv)
        this->onChannelCreate(data);
    else if (event == "GUILD_UPDATE"sv)
        this->onGuildUpdate(data);
    // Ignore these
    else if (event == "MESSAGE_ACK"sv ||
             event == "GUILD_INTEGRATIONS_UPDATE"sv ||
             event == "USER_SETTINGS_UPDATE"sv ||
             event == "CHANNEL_PINS_ACK"sv){

    } else
        std::cerr << "DiscordClient recieved unknown event: " << event << std::endl;
}

// Cache parsers
void BaseClient::onReady(const json::Value& data) {

    // Our user info
    this->user = this->FetchUser(data["user"]);

    // We always recieve guild data
    for (const json::Value& v : data["guilds"].GetArray()) {
        Guild* new_guild = new Guild(this, v);
        this->guilds.insert({new_guild->id, new_guild});
    }


    this->onReady();
}
void BaseClient::onGuildCreate(const json::Value& data) {
    // Since user accounts get guilds on the start, we need to check if we added it already
    Snowflake id = atol(data["id"].GetString());
    auto find = this->guilds.find(id);
    if (find == this->guilds.end()) {
        Guild* new_guild = new Guild(this, id, data);
        this->guilds.insert({new_guild->id, new_guild});
        this->onGuildCreate(new_guild);
    } else
        this->onGuildCreate(find->second->Create(data));
}
void BaseClient::onMessageCreate(const json::Value& data) {
    Message msg(this, data);
    this->onMessageCreate(&msg);
}
void BaseClient::onChannelCreate(const json::Value& data) {
    Channel* channel = new Channel(this, data);
    this->channels.insert({channel->id, channel});
    this->onChannelCreate(channel);
}
void BaseClient::onGuildUpdate(const json::Value& data) {
    Snowflake id = atol(data["id"].GetString());
    auto find = this->guilds.find(id);
    assert(find != this->guilds.end());
    this->onGuildUpdate(find->second->Update(data));
}

// Info retrieval
User* BaseClient::FetchUser(Snowflake id, bool cache) {
    if (cache) {
        auto find = this->users.find(id);
        if (find != this->users.end())
            return find->second;
    }
    std::string r = this->http.Get("/users/" + std::to_string(id));
    json::Document data;
    data.Parse(r.data(), r.size());
    User* new_user = new User(this, id, data);
    this->users.insert({id, new_user});
    return new_user;
}
User* BaseClient::FetchUser(const json::Value& v) {
    // Send away
    return this->FetchUser(atol(v["id"].GetString()), v);
}
Guild* BaseClient::FetchGuild(Snowflake id, bool cache){
    if (cache) {
        auto find = this->guilds.find(id);
        if (find != this->guilds.end())
            return find->second;
    }
    std::string r = this->http.Get("/guilds/" + std::to_string(id));
    json::Document data;
    data.Parse(r.data(), r.size());
    Guild* new_guild = new Guild(this, id, data);
    this->guilds.insert({id, new_guild});
    return new_guild;
}
Guild* BaseClient::FetchGuild(const json::Value& v) {
    return this->FetchGuild(atol(v["id"].GetString()), v);
}
Emoji* BaseClient::FetchEmoji(Snowflake id) {
    auto find = this->emojis.find(id);
    if (find == this->emojis.end())
        throw std::logic_error("Unable to find emoji");
    return find->second;
}
Channel* BaseClient::FetchChannel(Snowflake id) {
    auto find = this->channels.find(id);
    if (find != this->channels.end())
        return find->second;
    std::string r = this->http.Get("/channels/" + std::to_string(id));
    json::Document data;
    data.Parse(r.data(), r.size());
    Channel* new_channel = new Channel(this, id, data);
    this->channels.insert({id, new_channel});
    return new_channel;
}

// Cache
// Internally caches data we already have
User* BaseClient::FetchUser(Snowflake id, const json::Value& data) {
    auto find = this->users.find(id);
    if (find != this->users.end())
        return find->second;
    else {
        User* new_user = new User(this, id, data);
        this->users.insert({id, new_user});
        return new_user;
    }
}
Guild* BaseClient::FetchGuild(Snowflake id, const json::Value& data) {
    auto find = this->guilds.find(id);
    if (find != this->guilds.end())
        return find->second;
    else {
        Guild* new_guild = new Guild(this, id, data);
        this->guilds.insert({id, new_guild});
        return new_guild;
    }
}

Snowflake ExtractId(const json::Value& data) {
    return atol(data["id"].GetString());
}

}
