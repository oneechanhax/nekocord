
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

#include "guild/guild.hpp"
#include "channel.hpp"
#include "client.hpp"
#include "user.hpp"

#include "message.hpp"

namespace neko::discord {

Attachment::Attachment(const json::Value& data){
    this->id = atol(data["id"].GetString());
    this->filename = data["filename"].GetString();
    this->size = data["size"].GetInt();
    this->url = data["url"].GetString();
    this->proxy_url = data["proxy_url"].GetString();
    this->height = data["height"].GetInt();
    this->width = data["width"].GetInt();
}

Embed::Thumbnail::Thumbnail(const json::Value& data) {
    this->url = data["url"].GetString();
    this->proxy_url = data["proxy_url"].GetString();
    this->height = data["height"].GetInt();
    this->width = data["width"].GetInt();
}

Embed::Video::Video(const json::Value& data) {
    this->url = data["url"].GetString();
    this->height = data["height"].GetInt();
    this->width = data["width"].GetInt();
}

Embed::Image::Image(const json::Value& data) {
    this->url = data["url"].GetString();
    this->proxy_url = data["proxy_url"].GetString();
    this->height = data["height"].GetInt();
    this->width = data["width"].GetInt();
}

Embed::Provider::Provider(const json::Value& data) {
    this->name = data["name"].GetString();
    this->url = data["url"].GetString();
}

Embed::Author::Author(const json::Value& data) {
    this->name = data["name"].GetString();
    this->url = data["url"].GetString();
    this->icon_url = data["icon_url"].GetString();
    this->proxy_icon_url = data["proxy_icon_url"].GetString();
}

Embed::Footer::Footer(const json::Value& data) {
    this->text = data["text"].GetString();
    this->proxy_icon_url = data["proxy_icon_url"].GetString();
    auto find = data.FindMember("icon_url");
    if (find != data.MemberEnd())
        this->icon_url = find->value.GetString();
}

Embed::Field::Field(const json::Value& data) {
    this->name = data["name"].GetString();
    this->value = data["value"].GetString();
    auto find = data.FindMember("inline");
    if (find != data.MemberEnd())
        this->_inline = find->value.GetBool();
}

Embed::Embed(const json::Value& data) {

    auto find = data.FindMember("title");
    if (find != data.MemberEnd())
        this->title = find->value.GetString();
    find = data.FindMember("type");
    if (find != data.MemberEnd())
        this->type = find->value.GetString();
    find = data.FindMember("description");
    if (find != data.MemberEnd())
        this->description = find->value.GetString();
    find = data.FindMember("url");
    if (find != data.MemberEnd())
        this->url = find->value.GetString();

    find = data.FindMember("color");
    if (find != data.MemberEnd())
        this->color = find->value.GetInt();

    find = data.FindMember("footer");
    if (find != data.MemberEnd())
        this->footer = Footer(find->value);
    find = data.FindMember("image");
    if (find != data.MemberEnd())
        this->image = Image(find->value);
    find = data.FindMember("thumbnail");
    if (find != data.MemberEnd())
        this->thumbnail = Thumbnail(find->value);
    find = data.FindMember("video");
    if (find != data.MemberEnd())
        this->video = Video(find->value);
    find = data.FindMember("provider");
    if (find != data.MemberEnd())
        this->provider = Provider(find->value);
    find = data.FindMember("author");
    if (find != data.MemberEnd())
        this->author = Author(find->value);

    find = data.FindMember("fields");
    if (find != data.MemberEnd())
        for (const json::Value& i : find->value.GetArray())
            this->fields.emplace_back(i);
}

Reaction::Reaction(BaseClient* client, const json::Value& data) {
    this->count = data["count"].GetInt();
    this->me = data["me"].GetBool();
    const json::Value& id = data["id"];
    if (!id.IsNull()) {
        this->id = atol(id.GetString());
        this->emoji = client->FetchEmoji(this->id);
    }
    this->name = data["name"].GetString();
};

Activity::Activity(const json::Value& data) {
    this->type = static_cast<Type>(data["type"].GetInt());
    this->party_id = data["party_id"].GetString();
}

Application::Application(const json::Value& data) {
    this->id = atol(data["id"].GetString());
    this->cover_image = data["cover_image"].GetString();
    this->description = data["description"].GetString();
    this->icon = data["icon"].GetString();
    this->name = data["name"].GetString();
}

Message::Message(BaseClient* _client, const json::Value& data)
    : client(_client), channel(client->FetchChannel(atol(data["channel_id"].GetString()))) {
    this->id = atol(data["id"].GetString());

    // Update channel info
    switch(this->channel->type) {
    case Channel::Type::kDm:
        this->channel->GetDMChannel()->last_message_id = this->id;
        break;
    case Channel::Type::kGroupDm:
        this->channel->GetGroupDMChannel()->last_message_id = this->id;
        break;
    default:
        std::cout << "Message: TODO, guild last id!" << std::endl;
    }

    // Who
    auto find = data.FindMember("webhook_id");
    if (find != data.MemberEnd()) // Webhook
        this->webhook_id = atol(find->value.GetString());
    else { // User
        this->author = this->client->FetchUser(data["author"]);
        // Guild
        find = data.FindMember("guild_id");
        if (find != data.MemberEnd()) {
            Guild* guild = this->client->FetchGuild(atol(find->value.GetString()));
            this->member = guild->FetchMember(this->author->id);
            for (const json::Value& role : data["mention_roles"].GetArray())
                this->mention_roles.push_back(this->guild->FetchRole(atol(role.GetString())));
        }
    }

    // When
    this->timestamp = data["timestamp"].GetString();
    const json::Value& edit_time = data["edited_timestamp"];
    if (!edit_time.IsNull())
        this->edited_timestamp = edit_time.GetString();

    // What
    this->content = data["content"].GetString();
    this->mention_everyone = data["mention_everyone"].GetBool();
    this->tts = data["tts"].GetBool();
    find = data.FindMember("nonce");
    if (find != data.MemberEnd() && !find->value.IsNull())
        this->nonce = atol(find->value.GetString());
    this->pinned = data["pinned"].GetBool();
    this->type = static_cast<Type>(data["type"].GetInt());
    find = data.FindMember("activity");
    if (find != data.MemberEnd())
        this->activity = Activity(find->value);
    find = data.FindMember("application");
    if (find != data.MemberEnd())
        this->application = Application(find->value);
    for (const json::Value& user : data["mentions"].GetArray())
        this->mentions.push_back(this->client->FetchUser(user));
    for (const json::Value& att : data["attachments"].GetArray())
        this->attachments.push_back(Attachment(att));
    for (const json::Value& embed : data["embeds"].GetArray())
        this->embeds.emplace_back(embed);
    find = data.FindMember("reactions");
    if (find != data.MemberEnd())
        for(const json::Value& react : find->value.GetArray())
            this->reactions.push_back(Reaction(this->client, react));
};

void Message::Reply(std::string_view msg) {
    if (this->author)
        this->channel->SendMessage("<@" + std::to_string(this->author->id) + ">, " + std::string(msg));
    else
        this->channel->SendMessage(msg);
}

}
