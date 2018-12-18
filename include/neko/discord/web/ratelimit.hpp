
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
