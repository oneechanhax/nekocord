
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

Presence::Presence(BaseClient& client, const Value& data)
    : Presence(client.FetchUser(atol(data["user"]["id"].GetString())), data) {}

Presence::Presence(User& _user, const Value& data)
    : user(_user) {
    this->Update(data);
}

Presence& Presence::Update(const Value& data) {
    this->status = data["status"].GetString();

    this->game.reset();
    const Value& game = data["game"];
    if (!game.IsNull())
        this->game = Game(game);
    return *this;
}

}
