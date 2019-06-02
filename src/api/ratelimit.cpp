
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

#include <cassert>

#include <neko/string.hpp>

#include "api/ratelimit.hpp"

namespace neko::discord::web {
using namespace std::string_view_literals;

// Some string urils
static bool IsDigits(std::string_view str) {
    for (char i : str)
        if (!std::isdigit(i))
            return false;
    return true;
}

static bool StartsWith(std::string_view str, std::string_view with) {
    if (str.size() < with.size())
        return false;
    for (int i = 0; i < with.size(); i++)
        if (str[i] != with[i])
            return false;
    return true;
}

RateLimiter::Type RateLimiter::GetType(std::string_view url){
    if (StartsWith("/channels/"sv, url))
        return Type::kChannels;
    if (StartsWith("/webhooks/"sv, url))
        return Type::kWebhooks;
    // theres one request that doesnt have numbers == is global
    const static std::string_view guild_major = "/guilds/"sv;
    if (StartsWith(guild_major, url)) {
        constexpr size_t flake_len = cexpr::strlen("427346422548004882"); // random one i picked
        if (url.size() < guild_major.size() + flake_len)
            return RateLimiter::Type::kGlobal;
        if (!IsDigits(url.substr(guild_major.size(), flake_len)))
            return RateLimiter::Type::kGlobal;
        return RateLimiter::Type::kGuilds;
    }
    return RateLimiter::Type::kGlobal;
}

void RateLimiter::Enforce(Type type) {
    switch (type){
    case Type::kChannels:
        if (!this->channel_id.CanSend())
            throw std::runtime_error("You are being rate limited.");
    case Type::kWebhooks:
        if (!this->webhook_id.CanSend())
            throw std::runtime_error("You are being rate limited.");
    case Type::kGuilds:
        if (!this->guild_id.CanSend())
            throw std::runtime_error("You are being rate limited.");
    default:
        if (!this->global.CanSend())
            throw std::runtime_error("You are being rate limited.");
    }
}
void RateLimiter::Refresh(Type type, const cpr::Response& reply) {
    assert(reply.status_code != 429); // If we reached this, then we failed to prevent it
    switch (type) {
    case Type::kChannels:
        return this->channel_id.Refresh(reply);
    case Type::kWebhooks:
        return this->webhook_id.Refresh(reply);
    case Type::kGuilds:
        return this->guild_id.Refresh(reply);
    default:
        return this->global.Refresh(reply);
    }
}

bool RateLimiter::Limit::CanSend(){
    if (!this->set)
        return true;
    if (this->remaining == 0) {
        if (this->reset > std::chrono::system_clock::now())
            return false;
        this->remaining = this->limit;
    }
    return true;
}

void RateLimiter::Limit::Refresh(const cpr::Response& reply) {
    std::string arr = "array bro";
    for (auto i : reply.header)
        arr += i.second;
    /*this->limit = std::stoi(reply.header.at("X-RateLimit-Limit"));
    this->remaining = std::stoi(reply.header.at("X-RateLimit-Remaining"));
    this->reset = std::chrono::system_clock::time_point(std::chrono::seconds(std::stol(reply.header.at("X-RateLimit-Reset"))));*/
}

}
