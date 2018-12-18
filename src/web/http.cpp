
#include <fstream>

#include <neko/filesystem.hpp>
#include <rapidjson/writer.h>

#include "user.hpp"
#include "client.hpp"

#include "web/http.hpp"

namespace neko::discord::web {

HttpMgr::HttpMgr(BaseClient& _client) : client(_client) {}

cpr::Header HttpMgr::GetBaseHeader() {
    cpr::Header header;
    // Authorize
    if (this->client.user) { // lets ensure we at least got READY
        header.insert({"Authorization",
        this->client.user->bot ? "Bot " + this->client.token
        : this->client.token});
    }
    // The useragent
    header.insert({"User-Agent",
        "DiscordBot (https://github.com/oneechanhax/TODO, "
        "vTODO)"});
    return header;
}

std::string HttpMgr::Get(const std::string& url) {
    RateLimiter::Type t = this->rate_limiter.Enforce(url);
    cpr::Response reply = cpr::Get(cpr::Url("https://discordapp.com/api" + url), this->GetBaseHeader());
    this->rate_limiter.Refresh(t, reply);
    return reply.text;
}

std::int32_t HttpMgr::Post(const std::string& url, const rapidjson::Value& msg) {
    using namespace rapidjson;
    StringBuffer buf;
    Writer<StringBuffer> writer(buf);
    msg.Accept(writer);
    // Send away
    return this->Post(url, std::string(buf.GetString(), buf.GetLength()));
}
std::int32_t HttpMgr::Post(const std::string& url, const std::string& msg) {
    RateLimiter::Type t = this->rate_limiter.Enforce(url);

    cpr::Header header = this->GetBaseHeader();
    header.insert({"Content-Type", "application/json"});
    header.insert({"Content-Length", std::to_string(msg.size())});
    cpr::Response reply = cpr::Post(cpr::Url("https://discordapp.com/api" + url),
                          cpr::Body(msg), header);

    this->rate_limiter.Refresh(t, reply);
    return reply.status_code;
}

// Utils
std::string HttpMgr::GetGateway(bool force) {

    static fs::path cache_dir = fs::temp_directory_path() / "nekocord";
    static fs::path cache_path = cache_dir / "gateway.txt";
    if (!force) {
        std::ifstream read(cache_path);
        if (read) {
            std::string ret;
            std::getline(read, ret);
            return ret;
        }
    }
    // Cache unavailable

    // Get our gateway
    cpr::Response responce = cpr::Get(cpr::Url("https://discordapp.com/api/gateway"));
    if (!responce.text.size())
        throw std::runtime_error("Failed to retrieve gateway, err: " + std::to_string(responce.status_code) + ".");

    // Seperate it from json
    using namespace rapidjson;
    Document json;
    json.Parse(responce.text.data(), responce.text.size());

    // {"url: "wss://gateway.discord.gg"}
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
