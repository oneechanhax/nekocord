
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

#include "guild/role.hpp"

namespace neko::discord {

// GUILD_CREATE GUILD_UPDATE
Role::Role(Guild& _guild, const rapidjson::Value& data) : guild(_guild), client(_guild.client) {
    this->id = atol(data["id"].GetString());

    this->color = data["color"].GetInt();
    this->hoist = data["hoist"].GetBool();
    this->managed = data["managed"].GetBool();
    this->mentionable = data["mentionable"].GetBool();
    const rapidjson::Value& v = data["name"];
    this->name = std::string(v.GetString(), v.GetStringLength());
    this->permissions = data["permissions"].GetInt();
    this->position = data["position"].GetInt();
}

}

// GUILD_CREATE
/*{
"position": 0,
"permissions": 104324161,
"name": "@everyone",
"mentionable": false,
"managed": false,
"id": "371194211631890454",
"hoist": false,
"color": 0
},*/
