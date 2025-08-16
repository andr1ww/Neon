#include "pch.h"
#include "NexaAuth.h"

#include "Engine/NetDriver/Header/NetDriver.h"
#include "FortniteGame/FortGameState/Header/FortGameState.h"
#include "Neon/Config.h"

void NexaAuth::SetAuthToken()
{
    const std::string secretKey = "MzQ8vWxpRnYjF2+bUakCs1VeGZoT6wINrPy3X4D9JElqKfmH7LugSt5iOhcBA0d/QnYWzjXKe1R7s9VmFqOhcNZgSwIu2axLp8kET0vD6MfiPl3r4tGyH5";
        
    std::hash<std::string> hasher;
    size_t hv = hasher(Config::Echo::Session + secretKey);
        
    std::stringstream ss;
    ss << std::hex << hv;
    std::string hash = ss.str();
        
    const char* c = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string HASH;
        
    for (size_t i = 0; i < hash.length(); i += 3) {
        int n = (hash[i] << 16) + (i + 1 < hash.length() ? hash[i + 1] << 8 : 0) + (i + 2 < hash.length() ? hash[i + 2] : 0);
        HASH += c[n >> 18];
        HASH += c[(n >> 12) & 63];
        HASH += (i + 1 < hash.length()) ? c[(n >> 6) & 63] : '=';
        HASH += (i + 2 < hash.length()) ? c[n & 63] : '=';
    }
        
    std::string base = Config::Echo::Session + ":" + HASH;
        
    std::string token;
    for (size_t i = 0; i < base.length(); i += 3) {
        int n = (base[i] << 16) + (i + 1 < base.length() ? base[i + 1] << 8 : 0) + (i + 2 < base.length() ? base[i + 2] : 0);
        token += c[n >> 18];
        token += c[(n >> 12) & 63];
        token += (i + 1 < base.length()) ? c[(n >> 6) & 63] : '=';
        token += (i + 2 < base.length()) ? c[n & 63] : '=';
    }
    
    NexaAuthToken = token;
}
