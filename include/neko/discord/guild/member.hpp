
#pragma once

#include <rapidjson/document.h>

#include "../snowflake.hpp"

namespace neko::discord {

class BaseClient;
class User;
class Guild;
class Role;
class GuildMember {
    friend Guild;
    void Update(const rapidjson::Value&);
    // TODO, Update
public:
    GuildMember(Guild&, const rapidjson::Value&);
    BaseClient& client;
    Guild& guild;
    User& user;

    bool deaf;
    bool mute;
    std::string nick;
    std::vector<Snowflake> roles;
};

}
