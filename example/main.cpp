
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

#include <neko/discord/nekocord.hpp>

// cpp 20 where are you ;(
static bool StartsWith(std::string_view str, std::string_view with) {
    if (str.size() < with.size())
        return false;
    for (int i = 0; i < with.size(); i++)
        if (str[i] != with[i])
            return false;
    return true;
}

using namespace neko::discord;
class Discord : public BaseClient {
public:
    using BaseClient::BaseClient;
    void onReady() {
        puts("Ready!\n");
    }
    void onGuildCreate(Guild& guild) {
        printf("Found new guild: %s!\n", guild.name.c_str());
    }
    void onMessageCreate(Message& msg) {
        if (StartsWith(msg.content, "!ping"))
            msg.Reply("Pong!");
    }
};

const char token_path[] = "token.txt";
int main(int argc, const char* args[]) {

    // Get and ensure we have a token
    std::string token;
    if (argc > 1)
        token = args[1];

    std::ifstream in(token_path);
    if (!in) {
        if (token.empty())
            throw std::runtime_error("Please write your token to token.txt in this directory.");
        else {
            std::fstream write(token_path, std::ios_base::out);
            write.write(token.data(), token.size());
        }
    } else if (token.empty())
        std::getline(in, token);

    // Login desu~
    Discord client;
    client.Login(token);

    for(;;){}
}
