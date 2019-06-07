
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

#include "client.hpp"

namespace neko::discord {
using namespace rapidjson;

Game::Game(const Value& data) {
    this->id = data["id"].GetString();
    this->name = data["name"].GetString();
    auto find = data.FindMember("state");
    if (find != data.MemberEnd())
        this->state = find->value.GetString();
    const Value& timestamps = data["timestamps"];
    this->timestamp = timestamps["start"].GetInt();
    this->type = data["type"].GetInt();
}

Presence::Presence(BaseClient* client, const Value& data)
    : Presence(client->FetchUser(atol(data["user"]["id"].GetString())), data) {}

Presence::Presence(User* _user, const Value& data)
    : user(_user) {
    this->Update(data);
}

Presence* Presence::Update(const Value& data) {
    this->status = data["status"].GetString();

    this->game.reset();
    const Value& game = data["game"];
    if (!game.IsNull())
        this->game = Game(game);
    return this;
}

}
