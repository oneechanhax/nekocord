
#pragma once

#include <rapidjson/document.h>

#include "../snowflake.hpp"

namespace neko::discord {

class BaseClient;
class Guild;
class Emoji {
    friend Guild;
public:
    Emoji(Guild&, const rapidjson::Value&);
    ~Emoji();
    BaseClient& client;
    Guild& guild;
    Snowflake id;

    bool animated;
    bool managed;
    std::string name;
    bool require_colons;
    std::vector<Snowflake> roles;
};

}
