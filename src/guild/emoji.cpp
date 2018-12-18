
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
