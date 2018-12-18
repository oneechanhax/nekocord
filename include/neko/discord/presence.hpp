
#pragma once

#include <optional>

#include <rapidjson/document.h>

namespace neko::discord {

class Game {
public:
    Game(const rapidjson::Value&);
    int timestamp;
    std::string id;
    std::string name;
    std::string state;
    int type;
};

class User;
class Presence {
public:
    Presence(BaseClient&, const rapidjson::Value&);
    Presence(User&, const rapidjson::Value&);
    Presence& Update(const rapidjson::Value&);
    class Game {
        friend Presence;
        Game(const rapidjson::Value&);
    public:
        int timestamp;
        std::string id;
        std::string name;
        std::string state;
        int type;
    };
    User& user;
    std::optional<Game> game;
    std::string status;
};

}

// From GUILD_CREATE
/*{
    "user": {
        "id": "122437993691611137"
    },
    "status": "online",
    "game": {
        "type": 0,
        "timestamps": {
            "start": 1544907902518
        },
        "name": "Escape from Tarkov",
        "id": "28a57b2b3ac51a5d",
        "created_at": 1544907900815
    },
    "client_status": {
        "desktop": "online"
    },
    "activities": [
        {
            "type": 0,
            "timestamps": {
                "start": 1544907902518
            },
        "name": "Escape from Tarkov",
        "id": "28a57b2b3ac51a5d",
        "created_at": 1544907900815
        }
    ]
},*/

// From PRESENCE_UPDATE
/*{
    "user":{
        "id":"119674592712196098"
    },
    "status":"online",
    "roles":[
        "371196035667591175","371196416435027970","371269070647197699"],
    "nick":"butte",
    "guild_id":"371194211631890454",
    "game":{
        "type":0,"timestamps":{
        "start":1544886398000
        },
        "state":"Editing client.cpp",
        "name":"Atom Editor",
        "id":"212dce5c4186aa1b",
        "details":"Working on curl-test",
        "created_at":1544908450125,
        "assets":{
            "small_text":"Working with Atom",
            "small_image":"386325567483412490",
            "large_text":"Editing a C++ file",
            "large_image":"380684204326780929"
        },
        "application_id":"380510159094546443"
    },
    "client_status":{
        "desktop":"online"
    },
    "activities":[
        {
            "type":0,
            "timestamps":{
                "start":1544886398000
            },
            "state":"Editing client.cpp",
            "name":"Atom Editor","id":"212dce5c4186aa1b","details":"Working on curl-test","created_at":1544908450125,"assets":{"small_text":"Working with Atom","small_image":"386325567483412490","large_text":"Editing a C++ file","large_image":"380684204326780929"},"application_id":"380510159094546443"}]}

}*/
