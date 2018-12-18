
#pragma once

#include <rapidjson/document.h>

#include "../snowflake.hpp"

namespace neko::discord {

class BaseClient;
class Guild;
class GuildMember;
class Role {
public:
    Role(Guild&, const rapidjson::Value&);
    BaseClient& client;
    Guild& guild;

    uint16_t color;
    bool hoist;
    Snowflake id;
    bool managed;
    bool mentionable;
    std::string name;
    int permissions;
    int position;
};

}
