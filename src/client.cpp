
#include <fstream>

#include <neko/filesystem.hpp>
#include <rapidjson/writer.h>

#include "guild/guild.hpp"
#include "channel.hpp"
#include "message.hpp"
#include "user.hpp"

#include "client.hpp"

namespace neko::discord {
using namespace rapidjson;

BaseClient::BaseClient()
    : http(*this), websocket(*this) {}

void BaseClient::Login(std::string_view _token) {
    assert(this->websocket.GetState() == web::Websocket::State::kDisconnected);
    this->token = _token;
    this->websocket.Connect();
}

// Events
// Our main role with the events is to effitently keep our cache updated
// Events send alot of data so we can do this as much as we can

void BaseClient::EmitEvent(std::string_view event, const Value& data) {
    using namespace std::string_view_literals;

    // TMP dumper lol
    std::string path = std::string(event) + ".txt";
    if (!std::ifstream(path)) {

        StringBuffer buf;
        Writer<StringBuffer> writer(buf);
        data.Accept(writer);

        std::fstream stream(path, std::ios_base::out);
        if (!stream.is_open())
            throw std::runtime_error("FStream broke!");

        stream.write(buf.GetString(), buf.GetSize());
        printf("New Event: %.*s! Dumped to %s\n", event.size(), event.data(), path.c_str());
    }

    // Comparing string_views is extremely fast uwu
    // not sure if it compares size first but it should be fast
    if (event == "READY"sv)
        this->onReady(data);
    else if (event == "GUILD_CREATE"sv)
        this->onGuildCreate(data);
    else if (event == "MESSAGE_CREATE"sv)
        this->onMessageCreate(data);
    else if (event == "CHANNEL_CREATE"sv)
        this->onChannelCreate(data);
    else if (event == "GUILD_UPDATE"sv)
        this->onGuildUpdate(data);
}

void BaseClient::onReady(const Value& data) {
    // Save our session data
    const Value& ses = data["session_id"];
    this->session_id = std::string(ses.GetString(), ses.GetStringLength());

    // Our user info
    this->user = &this->FetchUser(data["user"]);
    // new ClientUser((*this->users.insert({our_id, User(*this, our_id, our_user)}).first).second);

    // We always recieve guild data
    for (const Value& v : data["guilds"].GetArray())
        this->guilds.push_back(new Guild(*this, v));


    this->onReady();
}

void BaseClient::onGuildCreate(const Value& data) {
    // Since user accounts get guilds on the start, we need to check if we added it already
    Snowflake id = ExtractId(data);
    auto find = std::find_if(this->guilds.begin(), this->guilds.end(), [&](auto& guild){
        return guild->id == id;
    });
    if (find == this->guilds.end()) {
        Guild* new_guild = new Guild(*this, id, data);
        this->guilds.push_back(new_guild);
        this->onGuildCreate(*new_guild);
    } else
        this->onGuildCreate((*find)->Create(data));
}

void BaseClient::onMessageCreate(const Value& data) {
    Message msg(*this, data);
    this->onMessageCreate(msg);
}

void BaseClient::onChannelCreate(const Value& data) {
    Channel* channel = new Channel(*this, data);
    this->channels.push_back(channel);
    this->onChannelCreate(*channel);
}

void BaseClient::onGuildUpdate(const Value& data) {
    Snowflake id = ExtractId(data);
    auto find = std::find_if(this->guilds.begin(), this->guilds.end(), [&](auto& guild){ return guild->id == id; });
    assert(find != this->guilds.end());
    this->onGuildUpdate((*find)->Update(data));
}

// Info retrieval
User& BaseClient::FetchUser(Snowflake id, bool cache) {
    if (cache) {
        auto find = std::find_if(this->users.begin(), this->users.end(), [&](auto& user){ return user->id == id; });
        if (find != this->users.end())
            return **find;
    }
    std::string r = this->http.Get("/users/" + std::to_string(id));
    Document data;
    data.Parse(r.data(), r.size());
    User* new_user = new User(*this, id, data);
    this->users.push_back(new_user);
    return *new_user;
}
Guild& BaseClient::FetchGuild(Snowflake id, bool cache){
    if (cache) {
        auto find = std::find_if(this->guilds.begin(), this->guilds.end(), [&](auto guild){ return guild->id == id; });
        if (find != this->guilds.end())
            return **find;
    }
    std::string r = this->http.Get("/guilds/" + std::to_string(id));
    Document data;
    data.Parse(r.data(), r.size());
    Guild* new_guild = new Guild(*this, id, data);
    this->guilds.push_back(new_guild);
    return *new_guild;
}
Emoji& BaseClient::FetchEmoji(Snowflake id) {
    auto find = std::find_if(this->emojis.begin(), this->emojis.end(), [&](auto emoji){ return emoji->id == id; });
    if (find == this->emojis.end())
        throw std::logic_error("Unable to find emoji");
    return **find;
}
Channel& BaseClient::FetchChannel(Snowflake id) {
    auto find = std::find_if(this->channels.begin(), this->channels.end(), [&](auto chan){ return chan->id == id; });
    if (find != this->channels.end())
        return **find;
    std::string r = this->http.Get("/channels/" + std::to_string(id));
    Document data;
    data.Parse(r.data(), r.size());
    Channel* new_channel = new Channel(*this, id, data);
    this->channels.push_back(new_channel);
    return *new_channel;
}

// Cache
// Internally caches data we already have
User& BaseClient::FetchUser(Snowflake id, const rapidjson::Value& data) {
    auto find = std::find_if(this->users.begin(), this->users.end(), [&](auto user){ return user->id == id; });
    if (find != this->users.end())
        return **find;
    else {
        User* new_user = new User(*this, id, data);
        this->users.push_back(new_user);
        return *new_user;
    }
}
Guild& BaseClient::FetchGuild(Snowflake id, const rapidjson::Value& data) {
    auto find = std::find_if(this->guilds.begin(), this->guilds.end(), [&](auto guild){ return guild->id == id; });
    if (find != this->guilds.end())
        return **find;
    else {
        Guild* new_guild = new Guild(*this, id, data);
        this->guilds.push_back(new_guild);
        return *new_guild;
    }
}

Snowflake ExtractId(const rapidjson::Value& data) {
    return atol(data["id"].GetString());
}

}
