
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
#include "client.hpp"
#include "user.hpp"

#include "guild/member.hpp"

namespace neko::discord {

// GUILD_CREATE
GuildMember::GuildMember(Guild* _guild, const rapidjson::Value& data)
    : client(_guild->client), guild(_guild), user(_guild->client->FetchUser(data["user"])) {

    this->deaf = data["deaf"].GetBool();
    this->mute = data["mute"].GetBool();
}

// GUILD_MEMBER_UPDATE
void GuildMember::Update(const rapidjson::Value& data){

    this->nick.clear();
    const auto& nick = data.FindMember("nick");
    if (nick != data.MemberEnd() && !nick->value.IsNull())
        this->nick = nick->value.GetString();

    // Add our roles
    this->roles.clear();
    for (const rapidjson::Value& role : data["roles"].GetArray())
        this->roles.push_back(atol(role.GetString()));
}
void GuildMember::AddRole(Snowflake i) {
    this->client->http.Put("/guilds/" + std::to_string(this->guild->id) +
                          "/members/" + std::to_string(this->user->id) +
                          "/roles/" + std::to_string(i));
}
void GuildMember::AddRole(Role* r) {
    this->AddRole(r->id);
}

}

// From GUILD_CREATE
/*{
    "user": {
        "username": "Onee",
        "id": "119674592712196098",
        "discriminator": "0695",
        "avatar": "b428a73b19ad37a69387d96300e393cd"
    },
    "roles": [
        "371196035667591175",
        "371196416435027970",
        "371269070647197699"
    ],
    "nick": "butte",
    "mute": false,
    "joined_at": "2017-10-21T07:25:36.237000+00:00",
    "deaf": false
},*/
