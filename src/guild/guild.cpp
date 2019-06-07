
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

#include "guild/channel.hpp"
#include "client.hpp"
#include "channel.hpp"
#include "user.hpp"

#include "guild/guild.hpp"

namespace neko::discord {
namespace json = rapidjson;

// GUILD_CREATE
Guild::Guild(BaseClient* _client, const json::Value& data)
    : Guild(_client, atol(data["id"].GetString()), data) {}
Guild::Guild(BaseClient* _client, Snowflake id, const json::Value& data) : client(_client) {
    this->id = id;

    this->unavailable = data["unavailable"].GetBool();
    if (this->unavailable)
        return;

    this->Create(data);
}

// GUILD_CREATE, call this function if it already exists instead of creating a new one
Guild* Guild::Create(const json::Value& data) {
    this->joined_at = data["joined_at"].GetString();
    this->member_count = data["member_count"].GetInt();

    // these change on another event
    for (const json::Value& member : data["members"].GetArray()) {
        GuildMember* new_mem = new GuildMember(this, member);
        this->members.insert({new_mem->user->id, new_mem});
    }

    for (const json::Value& channel : data["channels"].GetArray()) {
        Channel* new_channel = new Channel(this, channel);
        this->client->channels.insert({new_channel->id, new_channel});
    }

    return this->Update(data);
}

// GUILD_CREATE GUILD_UPDATE
Guild* Guild::Update(const json::Value& data) {

    this->explicit_content_filter = data["explicit_content_filter"].GetInt();
    this->icon = data["icon"].GetString();
    this->mfa_level = data["mfa_level"].GetInt();
    this->name = data["name"].GetString();
    this->owner_id = atol(data["owner_id"].GetString());

    // delete all emojis
    for (auto i : this->emojis)
        delete i.second;
    this->emojis.clear();
    for (const json::Value& emoji : data["emojis"].GetArray()) {
        Emoji* new_emoji = new Emoji(this, emoji);
        this->emojis.insert({new_emoji->id, new_emoji});
    }

    this->roles.clear();
    for (const json::Value& role : data["roles"].GetArray()) {
        Snowflake id = atol(role["id"].GetString());
        this->roles.insert({id, Role(this, id, role)});
    }

    return this;
}

Guild::~Guild(){
    // Clean out the channels
    for (auto i = this->client->channels.begin(); i != this->client->channels.end(); i++) {
        if ((i->second->type == Channel::Type::kText ||
             i->second->type == Channel::Type::kCategory ||
             i->second->type == Channel::Type::kVoice) &&
             i->second->GetGuildChannel()->guild->id == this->id)
            this->client->channels.erase(i);
    }
    for (auto i : this->emojis)
        delete i.second;
}

GuildChannel* Guild::FetchChannel(Snowflake id, bool cache) {
    if (cache) {
        auto find = this->channels.find(id);
        if (find != this->channels.end())
            return find->second;
    }
    std::string r = this->client->http.Get("/channels/" + std::to_string(id));
    json::Document data;
    data.Parse(r.data(), r.size());

    Channel* new_channel = new Channel(this->client, data);
    this->client->channels.insert({new_channel->id, new_channel});
    if (new_channel->type != Channel::Type::kText ||
        new_channel->type != Channel::Type::kCategory ||
        new_channel->type != Channel::Type::kVoice)
        throw std::logic_error("Channel isnt a guild channel.");
    return new_channel->GetGuildChannel();
}
Role* Guild::FetchRole(Snowflake id){
    auto find = this->roles.find(id);
    if (find == this->roles.end())
        throw std::runtime_error("Cant find role!");
    return &find->second;
}
GuildMember* Guild::FetchMember(Snowflake id, bool cache) {
    if (cache) {
        auto find = this->members.find(id);
        if (find != this->members.end())
            return find->second;
    }
    std::string r = this->client->http.Get("/guilds/" + std::to_string(this->id) +
                                           "/members/" + std::to_string(id));
    json::Document data;
    data.Parse(r.data(), r.size());
    if (data.IsNull())
        throw std::logic_error("Guild::FetchMember: Member doesnt exist");
        
    GuildMember* new_mem = new GuildMember(this, data);
    this->members.insert({new_mem->user->id, new_mem});
    return new_mem;
}

}

// READY
/*{
    "unavailable": true,
    "id": "371194211631890454"
}*/

// check dev files for more
