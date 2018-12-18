
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

#include "guild/emoji.hpp"

namespace neko::discord {

Emoji::Emoji(Guild& _guild, const rapidjson::Value& data)
    : client(_guild.client), guild(_guild) {
    this->id = atol(data["id"].GetString());

    this->animated = data["animated"].GetBool();
    this->managed = data["managed"].GetBool();
    this->name = data["name"].GetString();
    this->require_colons = data["require_colons"].GetBool();

    for (const rapidjson::Value& role : data["roles"].GetArray())
        this->roles.push_back(atol(role.GetString()));

    this->client.emojis.push_back(this);
}

Emoji::~Emoji() {
    auto find = std::find(this->client.emojis.begin(),
                this->client.emojis.end(), this);
    this->client.emojis.erase(find);
}


}

// GUILD_CREATE GUILD_UPDATE
/*{
    "roles": [],
    "require_colons": true,
    "name": "6a1M03e", // lol its the actual string
    "managed": false,
    "id": "396000397627490315",
    "animated": false
},*/
