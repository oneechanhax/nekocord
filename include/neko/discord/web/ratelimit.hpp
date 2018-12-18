
#pragma once

#include <cpr/cpr.h>

namespace neko::discord::web {

// https://discordapp.com/developers/docs/topics/rate-limits#rate-limits
class RateLimiter {
public:
    enum class Type { kChannels, kGuilds, kWebhooks, kGlobal };
    static Type GetType(std::string_view url);

    inline Type Enforce(std::string_view url) {
        Type t = this->GetType(url);
        this->Enforce(t);
        return t;
    }
    void Enforce(Type); // throws if u cant send payload
    void Refresh(Type, const cpr::Response&); // refresh the limit
private:
    class Limit {
    public:
        bool set = false; // whether we set it the first time
        int limit;
        int remaining;
        std::chrono::system_clock::time_point reset;
        bool CanSend();
        void Refresh(const cpr::Response&);
    };
    Limit channel_id;
    Limit guild_id;
    Limit webhook_id;
    Limit global;
};

}
