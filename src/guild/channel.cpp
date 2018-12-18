
#include "guild/guild.hpp"
#include "channel.hpp"

#include "guild/channel.hpp"

namespace neko::discord {
using namespace rapidjson;

PermissionOverwrite::PermissionOverwrite(const Value& data) {
    this->id = atol(data["id"].GetString());
    this->type = data["type"].GetString();
    this->allow = data["allow"].GetInt();
    this->deny = data["deny"].GetInt();
}

// GUILD_CREATE
GuildChannel::GuildChannel(Channel& _channel, Guild& _guild, const Value& data)
    : channel(_channel), guild(_guild) {

    auto find = data.FindMember("nsfw");
    if (find != data.MemberEnd())
        this->nsfw = find->value.GetBool();
    this->name = data["name"].GetString();
    this->parent_id = 0;
    //this->parent = nullptr;
    find = data.FindMember("parent_id");
    if (find != data.MemberEnd() && !find->value.IsNull()) {
        this->parent_id = atol(find->value.GetString());
        //this->parent = &this->guild.FetchChannel(this->parent_id);
    }
    this->position = data["position"].GetInt();

    for (const Value& overwrite : data["permission_overwrites"].GetArray())
        this->permission_overwrites.push_back(PermissionOverwrite(overwrite));

    this->guild.channels.push_back(this);
}

// CHANNEL_DELETE
GuildChannel::~GuildChannel(){
    auto find = std::find_if(this->guild.channels.begin(),
                this->guild.channels.end(), [&](auto& c)
                { return c->channel.id == this->channel.id; });
    this->guild.channels.erase(find);
}

// CHANNEL_UPDATE
void GuildChannel::Update(const Value& data) {
    // TODO
    return;
}

}

// GUILD_CREATE
/*{
    "user_limit": 0,
    "type": 4,
    "position": 0,
    "permission_overwrites": [
        {
            "type": "role",
            "id": "371194211631890454",
            "deny": 871824465,
            "allow": 65536
        }
    ],
    "name": "IMPORTANT",
    "id": "371194212198383616",
    "bitrate": 64000
},*/
