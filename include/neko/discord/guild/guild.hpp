
#pragma once

#include <rapidjson/document.h>

#include "emoji.hpp"
#include "member.hpp"
#include "role.hpp"

namespace neko::discord {

// TODO, what is "features" and what do i do with it???
class BaseClient;
class GuildChannel;
class Guild {
    friend BaseClient;
    Guild(BaseClient& _client, const rapidjson::Value&);
    Guild(BaseClient& _client, Snowflake, const rapidjson::Value&);
    Guild& Create(const rapidjson::Value&); // GUILD_CREATE
    Guild& Update(const rapidjson::Value&); // GUILD_UPDATE
    // GUILD_DELETE
    ~Guild();
public:
    BaseClient& client;
    bool unavailable; // The guild could be unavailable
    Snowflake id;

    std::vector<GuildChannel*> channels;
    std::vector<Emoji*> emojis;
    std::vector<Role> roles;
    std::vector<GuildMember> members;
    int explicit_content_filter;
    std::string icon;
    std::string joined_at; // TODO, parse this into some actual cpp format
    int member_count; // could this overflow???
    int mfa_level;
    std::string name;
    Snowflake owner_id;

    GuildChannel& FetchChannel(Snowflake id, bool cache = true);
    Role& FetchRole(Snowflake id);
};

}
