
#pragma once

#include <rapidjson/document.h>

#include "snowflake.hpp"

namespace neko::discord {

// https://discordapp.com/developers/docs/resources/user
class BaseClient;
class User {
public:
    User(BaseClient& _client, Snowflake id, const rapidjson::Value&);
    BaseClient& client;

    Snowflake id;
    std::string username;
    std::string discriminator;
    std::string avatar;
    bool bot = false;
};

}
