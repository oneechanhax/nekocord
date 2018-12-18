
#pragma once

#include <vector>

#include <rapidjson/document.h>

#include "../snowflake.hpp"

namespace neko::discord {

class PermissionOverwrite {
public:
    PermissionOverwrite(const rapidjson::Value&);
    Snowflake id;
    std::string type;
    int deny;
    int allow;
};

class Channel;
class Guild;
class GuildChannel {
protected:
    friend Channel;
    GuildChannel(Channel&, Guild&, const rapidjson::Value&);
    ~GuildChannel();
    void Update(const rapidjson::Value&);
public:
    Channel& channel;
    Guild& guild;

    std::string name;
    bool nsfw = false;
    std::vector<PermissionOverwrite> permission_overwrites;
    int position;
    Snowflake parent_id;
    //GuildChannel* parent;
};

}
