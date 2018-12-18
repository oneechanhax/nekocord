
/*
 * Nekocord: A lightning fast discord library for c++
 * Copyright (C) 2018 Rebekah Rowe
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <vector>

#include <rapidjson/document.h>

#include "snowflake.hpp"

namespace neko::discord {

class Channel;
class User;
class DMChannel {
    friend Channel;
    DMChannel(Channel& channel, const rapidjson::Value&);
    ~DMChannel(){}
public:
    Channel& channel;
    Snowflake last_message_id;
    User& recipient;
};

class GroupDMChannel {
    friend Channel;
    GroupDMChannel(Channel& channel, const rapidjson::Value&);
    ~GroupDMChannel(){}
public:
    void Update(const rapidjson::Value&);
    Channel& channel;
    std::string icon;
    Snowflake last_message_id;
    Snowflake owner_id;
    std::vector<User*> recipients;
};

class BaseClient;
class Guild;
class GuildChannel;
class Embed;
class Channel {
protected:
    friend BaseClient;
    friend Guild;
    Channel& Update(const rapidjson::Value&); // CHANNEL_UPDATE
    // CHANNEL_CREATE
    Channel(BaseClient& client, const rapidjson::Value&);
    Channel(BaseClient& client, Snowflake id, const rapidjson::Value&);
    // GUILD_CREATE
    Channel(Guild& guild, const rapidjson::Value&);
    ~Channel(); // CHANNEL_DESTROY
public:
    BaseClient& client;
    Snowflake id;

    enum class Type {
        kNone = -1, // For init
        kDm = 1,
        kGroupDm = 3,
        kText = 0, // Guild only below
        kVoice = 2,
        kCategory = 4
    } type = Type::kNone;

    DMChannel& GetDMChannel();
    GroupDMChannel& GetGroupDMChannel();
    GuildChannel& GetGuildChannel();
private:
    union {
        DMChannel* dm_channel;
        GroupDMChannel* group_dm_channel;
        GuildChannel* guild_channel;
    };

// Methods
public:
    void SendMessage(std::string_view, bool tts = false);
    void SendMessage(const Embed&);
};

}

/*{
    "type": 1,
    "recipients": [
        {
            "username": "Onee",
            "id": "119674592712196098",
            "discriminator": "0695",
            "avatar": "b428a73b19ad37a69387d96300e393cd"
        }
    ],

}*/
