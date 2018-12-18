
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

#include "guild/guild.hpp"

namespace neko::discord {
using namespace rapidjson;

// GUILD_CREATE
Guild::Guild(BaseClient& _client, const Value& data)
    : Guild(_client, ExtractId(data), data) {}
Guild::Guild(BaseClient& _client, Snowflake id, const Value& data) : client(_client) {
    this->id = id;

    this->unavailable = data["unavailable"].GetBool();
    if (this->unavailable)
        return;

    this->Create(data);
}

// GUILD_CREATE, call this function if it already exists instead of creating a new one
Guild& Guild::Create(const Value& data) {
    this->joined_at = data["joined_at"].GetString();
    this->member_count = data["member_count"].GetInt();

    // these change on another event
    for (const Value& member : data["members"].GetArray())
        this->members.emplace_back(*this, member);

    for (const Value& channel : data["channels"].GetArray())
        this->client.channels.push_back(new Channel(*this, channel));

    return this->Update(data);
}

// GUILD_CREATE GUILD_UPDATE
Guild& Guild::Update(const Value& data) {

    this->explicit_content_filter = data["explicit_content_filter"].GetInt();
    this->icon = data["icon"].GetString();
    this->mfa_level = data["mfa_level"].GetInt();
    this->name = data["name"].GetString();
    this->owner_id = atol(data["owner_id"].GetString());

    // delete all emojis
    for (Emoji* i : this->emojis)
        delete i;
    this->emojis.clear();
    for (const Value& emoji : data["emojis"].GetArray())
        this->emojis.push_back(new Emoji(*this, emoji));

    this->roles.clear();
    for (const Value& role : data["roles"].GetArray())
        this->roles.push_back(Role(*this, role));

    return *this;
}

Guild::~Guild(){
    // Clean out the channels
    for (auto i = this->client.channels.begin(); i != this->client.channels.end(); i++) {
        if (((*i)->type == Channel::Type::kText ||
             (*i)->type == Channel::Type::kCategory ||
             (*i)->type == Channel::Type::kVoice) &&
             (*i)->GetGuildChannel().guild.id == this->id)
            this->client.channels.erase(i);
    }
    for (Emoji* i : this->emojis)
        delete i;
}

GuildChannel& Guild::FetchChannel(Snowflake id, bool cache) {
    if (cache) {
        auto find = std::find_if(this->channels.begin(), this->channels.end(), [&](auto& c){ return c->channel.id == id; });
        if (find != this->channels.end())
            return **find;
    }
    std::string r = this->client.http.Get("/channels/" + std::to_string(id));
    Document data;
    data.Parse(r.data(), r.size());

    Channel* new_channel = new Channel(this->client, data);
    this->client.channels.push_back(new_channel);
    if (new_channel->type != Channel::Type::kText ||
        new_channel->type != Channel::Type::kCategory ||
        new_channel->type != Channel::Type::kVoice)
        throw std::logic_error("Channel isnt a guild channel.");
    return new_channel->GetGuildChannel();
}
Role& Guild::FetchRole(Snowflake id){
    auto find = std::find_if(this->roles.begin(), this->roles.end(), [&](auto& r){ return r.id == id; });
    if (find == this->roles.end())
        throw std::runtime_error("Cant find role!");
    return *find;
}

}

// READY
/*{
    "unavailable": true,
    "id": "371194211631890454"
}*/

// check dev files for more
