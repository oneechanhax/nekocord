
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

#include "user.hpp"

namespace neko::discord {

// READY GUILD_CREATE
User::User(BaseClient* _client, Snowflake id, const rapidjson::Value& data) : client(_client) {
    this->id = id;
    this->username = data["username"].GetString();
    this->discriminator = data["discriminator"].GetString();
    const rapidjson::Value& avatar = data["avatar"];
    if (!avatar.IsNull())
        this->avatar = avatar.GetString();
    auto find = data.FindMember("bot");
    if (find != data.MemberEnd())
        this->bot = find->value.GetBool();
}

}

// READY
/*{
    "username": "lolbadbot",
    "id": "435994515464126464",
    "discriminator": "7448",
    "avatar": "4d65e75e23ec6e1cb7b4041efb47abbc",
    "verified": true,
    "mfa_enabled": false,
    "email": null,
    "bot": true
}*/

// GUILD_CREATE
/*{
    "username": "Onee",
    "id": "119674592712196098",
    "discriminator": "0695",
    "avatar": "b428a73b19ad37a69387d96300e393cd"
},*/
