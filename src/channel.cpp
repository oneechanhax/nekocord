
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

#include <rapidjson/writer.h>

#include "guild/channel.hpp"
#include "guild/guild.hpp"
#include "client.hpp"

#include "channel.hpp"

namespace neko::discord {
using namespace rapidjson;

// CHANNEL_CREATE
Channel::Channel(BaseClient& _client, const Value& data)
    : Channel(_client, atol(data["id"].GetString()), data){}
Channel::Channel(BaseClient& _client, Snowflake id, const Value& data)
    : client(_client) {
    this->id = id;

    this->type = static_cast<Type>(data["type"].GetInt());
    switch(this->type) {
    case Type::kDm:
        this->dm_channel = new DMChannel(*this, data);
        break;
    case Type::kGroupDm:
        this->group_dm_channel = new GroupDMChannel(*this, data);
        break;
    case Type::kText:
    case Type::kCategory:
    case Type::kVoice:
        this->guild_channel = new GuildChannel(*this, this->client.FetchGuild(data["guild_id"]), data);
    }
}

// GUILD_CREATE
Channel::Channel(Guild& guild, const Value& data) : client(guild.client) {
    this->id = atol(data["id"].GetString());

    // To ensure its guild channel
    this->type = static_cast<Type>(data["type"].GetInt());
    assert(this->type == Type::kText || this->type == Type::kCategory || this->type == Type::kVoice);
    this->guild_channel = new GuildChannel(*this, guild, data);
}

Channel::~Channel() {
    switch(this->type) {
    case Type::kDm:
        delete this->dm_channel;
        break;
    case Type::kGroupDm:
        delete this->group_dm_channel;
        break;
    case Type::kText:
    case Type::kCategory:
    case Type::kVoice:
        delete this->guild_channel;
        break;
    }
}

Channel& Channel::Update(const Value& data) {
    assert(this->id == atol(data["id"].GetString()));
    switch(this->type) {
    case Type::kDm:
        assert(false); // These dont update right ;-;
    case Type::kGroupDm:
        this->group_dm_channel->Update(data);
        break;
    case Type::kText:
    case Type::kCategory:
    case Type::kVoice:
        this->guild_channel->Update(data);
        break;
    }
    return *this;
}

DMChannel& Channel::GetDMChannel() {
    assert(this->type == Type::kDm);
    return *this->dm_channel;
}
GroupDMChannel& Channel::GetGroupDMChannel(){
    assert(this->type == Type::kGroupDm);
    return *this->group_dm_channel;
}
GuildChannel& Channel::GetGuildChannel() {
    assert(this->type == Type::kText || this->type == Type::kCategory || this->type == Type::kVoice);
    return *this->guild_channel;
}

void Channel::SendMessage(std::string_view data, bool tts) {
    assert(this->type == Type::kDm || this->type == Type::kGroupDm || this->type == Type::kText);
    Document msg(kObjectType);
    auto& alloc = msg.GetAllocator();

    msg.AddMember("content", StringRef(data.data(), data.size()), alloc);
    if (tts)
        msg.AddMember("tts", Value().SetBool(true), alloc);

    this->client.http.Post("/channels/" + std::to_string(this->id) + "/messages", msg);
}

DMChannel::DMChannel(Channel& _channel, const Value& data)
    : channel(_channel),
    recipient(_channel.client.FetchUser(data["recipients"][0])) {
    this->last_message_id = atol(data["last_message_id"].GetString());
}

GroupDMChannel::GroupDMChannel(Channel& _channel, const Value& data)
    : channel(_channel) {
    this->last_message_id = atol(data["last_message_id"].GetString());
    this->Update(data);
}

void GroupDMChannel::Update(const rapidjson::Value& data) {
    this->icon = data["icon"].GetString();
    this->owner_id = atol(data["owner_id"].GetString());

    this->recipients.clear();
    for (const Value& member : data["recipients"].GetArray())
        this->recipients.push_back(&this->channel.client.FetchUser(member));
}

}
