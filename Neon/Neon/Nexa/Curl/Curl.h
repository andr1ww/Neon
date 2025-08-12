#pragma once
#include "pch.h"
#include <Neon/Neon/Nexa/Libraries/curl/curl.h>
#include <vector>

namespace Nexa::Curl
{
    void Delete(std::string URL, const std::vector<const char*>& Headers = {"Content-Type: application/json"});
    std::string Get(std::string URL, const std::vector<const char*>& Headers = {});
    std::string Post(std::string URL, std::string JsonData, const std::vector<const char*>& Headers = {});
}