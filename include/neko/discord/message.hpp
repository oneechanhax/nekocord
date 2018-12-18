
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

#include <rapidjson/document.h>

#include "snowflake.hpp"

namespace neko::discord {

class Message;
class Attachment {
    friend Message;
    Attachment(const rapidjson::Value&);
public:
    Snowflake id;
    std::string filename;
    size_t size;
    std::string url;
    std::string proxy_url;
    int height;
    int width;
};

class Embed {
public:
    Embed(){}
    Embed(const rapidjson::Value&);
    class Thumbnail {
    public:
        Thumbnail(){}
        Thumbnail(const rapidjson::Value&);
        std::string url;
        std::string proxy_url;
        int height = 0;
        int width = 0;
    };
    class Video {
    public:
        Video(){}
        Video(const rapidjson::Value&);
        std::string url;
        int height = 0;
        int width = 0;
    };
    class Image {
    public:
        Image(){}
        Image(const rapidjson::Value&);
        std::string url;
        std::string proxy_url;
        int height = 0;
        int width = 0;
    };
    class Provider {
    public:
        Provider(){}
        Provider(const rapidjson::Value&);
        std::string name;
        std::string url;
    };
    class Author {
    public:
        Author(){}
        Author(const rapidjson::Value&);
        std::string name;
        std::string url;
        std::string icon_url;
        std::string proxy_icon_url;
    };
    class Footer {
    public:
        Footer(){}
        Footer(const rapidjson::Value&);
        std::string text;
        std::string icon_url;
        std::string proxy_icon_url;
    };
    class Field {
    public:
        Field(){}
        Field(const rapidjson::Value&);
        std::string name;
        std::string value;
        bool _inline = false;
    };
    std::string title;
    std::string type;
    std::string description;
    std::string url;

    int color;
    std::optional<Footer> footer;
    std::optional<Image> image;
    std::optional<Thumbnail> thumbnail;
    std::optional<Video> video;
    std::optional<Provider> provider;
    std::optional<Author> author;
    std::vector<Field> fields;
};

class BaseClient;
class Emoji;
class Reaction {
    friend Message;
    Reaction(BaseClient&, const rapidjson::Value&);
public:
    int count;
    bool me;
    // Emoji info
    Snowflake id;
    std::string name;
    Emoji* emoji;
};

class Activity {
    friend Message;
    Activity(const rapidjson::Value&);
public:
    enum class Type { kJoin, kSpectate, kListen, kJoinRequest } type;
    std::string party_id;
};

class Application {
    friend Message;
    Application(const rapidjson::Value&);
public:
    Snowflake id;
    std::string cover_image;
    std::string description;
    std::string icon;
    std::string name;
};

class Channel;
class User;
class GuildMember;
class Guild;
class Role;
class Message {
public:
    Message(BaseClient& client, const rapidjson::Value&);
    BaseClient& client;
    Snowflake id;

    // Who
    User* author = nullptr;
    GuildMember* member = nullptr;
    // Where
    Channel& channel;
    Guild* guild = nullptr;
    // What
    std::string content;
    bool mention_everyone;
    std::vector<User*> mentions;
    std::vector<Role*> mention_roles;
    std::vector<Attachment> attachments;
    std::vector<Embed> embeds;
    std::vector<Reaction> reactions;
    bool tts;
    Snowflake nonce = 0;
    bool pinned;
    Snowflake webhook_id = 0;
    enum class Type { kDefault, kRecipientAdd, kRecipientRemove,
                    kCall, kChannelNameChange, kChannelPinnedMessage,
                    kGuildMemberJoin
    } type;
    std::optional<Activity> activity;
    std::optional<Application> application;
    // When
    std::string timestamp; // TODO sterlize to cpp data values
    std::string edited_timestamp;

    void Reply(std::string_view);
};

}

/*{
    "type": 0,
    "topic": "Updates to nekohook will be posted here!",
    "rate_limit_per_user": 0,
    "position": 2,
    "permission_overwrites": [
        {
            "type": "role",
            "id": "371269070647197699",
            "deny": 871823441,
            "allow": 66560
        },
        {
            "type": "role",
            "id": "371194211631890454",
            "deny": 871890001,
            "allow": 0
        }
    ],
    "parent_id": "371197833602400266",
    "nsfw": false,
    "name": "updates",
    "last_message_id": "521318321262428162",
    "id": "371198436931928066",
    "guild_id": "371194211631890454"
}*/
