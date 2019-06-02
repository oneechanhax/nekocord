
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

#include "guild/guild.hpp"
#include "channel.hpp"

#include "guild/channel.hpp"

namespace neko::discord {
namespace rj = rapidjson;

PermissionOverwrite::PermissionOverwrite(const rj::Value& data) {
    this->id = atol(data["id"].GetString());
    this->type = data["type"].GetString();
    this->allow = data["allow"].GetInt();
    this->deny = data["deny"].GetInt();
}

// GUILD_CREATE
GuildChannel::GuildChannel(Channel& _channel, Guild& _guild, const rj::Value& data)
    : channel(_channel), guild(_guild) {

    auto find = data.FindMember("nsfw");
    if (find != data.MemberEnd())
        this->nsfw = find->value.GetBool();
    this->name = data["name"].GetString();
    this->parent_id = 0;
    //this->parent = nullptr;
    find = data.FindMember("parent_id");
    if (find != data.MemberEnd() && !find->value.IsNull()) {
        this->parent_id = atol(find->value.GetString());
        //this->parent = &this->guild.FetchChannel(this->parent_id);
    }
    this->position = data["position"].GetInt();

    for (const rj::Value& overwrite : data["permission_overwrites"].GetArray())
        this->permission_overwrites.push_back(PermissionOverwrite(overwrite));

    this->guild.channels.insert({this->channel.id, this});
}

// CHANNEL_DELETE
GuildChannel::~GuildChannel(){
    this->guild.channels.erase(this->channel.id);
}

// CHANNEL_UPDATE
void GuildChannel::Update(const rj::Value& data) {
    // TODO
    return;
}

}

// GUILD_CREATE
/*{
    "user_limit": 0,
    "type": 4,
    "position": 0,
    "permission_overwrites": [
        {
            "type": "role",
            "id": "371194211631890454",
            "deny": 871824465,
            "allow": 65536
        }
    ],
    "name": "IMPORTANT",
    "id": "371194212198383616",
    "bitrate": 64000
},*/
