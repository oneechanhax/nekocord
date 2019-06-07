
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

#include <fstream>

#include <neko/filesystem.hpp>
#include <rapidjson/writer.h>

#include "user.hpp"
#include "client.hpp"

#include "api/rest.hpp"

namespace neko::discord::api {

RestAPI::RestAPI(BaseClient* _client) : client(_client) {}

cpr::Header RestAPI::GetBaseHeader() {
    cpr::Header header;
    // Authorize
    if (this->client->user) { // lets ensure we at least got READY
        header.insert({"Authorization",
        this->client->user->bot ? "Bot " + this->client->token
        : this->client->token});
    }
    // The useragent
    header.insert({"User-Agent",
        "DiscordBot (https://github.com/oneechanhax/TODO, "
        "vTODO)"});
    return header;
}

std::string RestAPI::Get(const std::string& url) {
    cpr::Response reply = cpr::Get(cpr::Url("https://discordapp.com/api" + url), RestAPI::GetBaseHeader());
    if (reply.error)
        throw std::runtime_error("RestAPI: error getting! " + reply.error.message);
    return reply.text;
}

void RestAPI::Post(const std::string& url, const rapidjson::Value& msg) {
    using namespace rapidjson;
    StringBuffer buf;
    Writer<StringBuffer> writer(buf);
    msg.Accept(writer);
    // Send away
    return this->Post(url, std::string(buf.GetString(), buf.GetSize()));
}
void RestAPI::Post(const std::string& url, const std::string& msg) {

    cpr::Header header = RestAPI::GetBaseHeader();
    header.insert({"Content-Type", "application/json"});
    header.insert({"Content-Length", std::to_string(msg.size())});
    cpr::Response reply = cpr::Post(cpr::Url("https://discordapp.com/api" + url),
                          cpr::Body(msg), header);
    if (reply.error)
        throw std::runtime_error("RestAPI: error posting! " + reply.error.message);
}
void RestAPI::Put(const std::string& url) {
    cpr::Response reply = cpr::Put(cpr::Url("https://discordapp.com/api" + url), RestAPI::GetBaseHeader());
    if (reply.error)
        throw std::runtime_error("RestAPI: error putting! " + reply.error.message);
}
// Utils
std::string RestAPI::GetGateway(bool force) {

    static const fs::path cache_dir = fs::temp_directory_path() / "nekocord";
    static const fs::path cache_path = cache_dir / "gateway.txt";
    if (!force) {
        std::ifstream read(cache_path);
        if (read) {
            std::string ret;
            std::getline(read, ret);
            return ret;
        }
    }

    cpr::Response responce = cpr::Get(cpr::Url("https://discordapp.com/api/gateway"));
    if (responce.error) {
        std::cerr << "RestAPI: Failed to get gateway, using fallback!" << std::endl;
        return "wss://gateway.discord.gg";
    }

    using namespace rapidjson;
    Document json;
    json.Parse(responce.text.data(), responce.text.size());
    Value& i = json["url"];
    std::string gateway = std::string(i.GetString(), i.GetStringLength());

    // Save it to cache
    fs::create_directories(cache_dir);
    std::fstream write(cache_path, std::ios_base::out);
    if (write)
        write.write(gateway.data(), gateway.size());
    else
        puts("Unable to cache gateway!\n");

    return gateway;
}

}
