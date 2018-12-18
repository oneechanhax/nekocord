
#include "user.hpp"

namespace neko::discord {

// READY GUILD_CREATE
User::User(BaseClient& _client, Snowflake id, const rapidjson::Value& data) : client(_client) {
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
