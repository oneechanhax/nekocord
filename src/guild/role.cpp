
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
