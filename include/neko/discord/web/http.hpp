
#pragma once

#include <cpr/cpr.h>

#include "ratelimit.hpp"

namespace neko::discord {
class BaseClient;

namespace web {

// Handles http stuff uwu

class HttpMgr {
public:
    HttpMgr(BaseClient& _client);
    BaseClient& client;

    std::string Get(const std::string& url);
    // Returns status code
    std::int32_t Post(const std::string& url, const rapidjson::Value&);
    std::int32_t Post(const std::string& url, const std::string& msg);

    static std::string GetGateway(bool force = false); // for websocket
private:
    // Get the base header needed for auth
    // https://discordapp.com/developers/docs/reference#authentication
    // https://discordapp.com/developers/docs/reference#user-agent
    cpr::Header GetBaseHeader();
    // Ratelimiting is required
    RateLimiter rate_limiter;
};

}}
