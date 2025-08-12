#pragma once
#include "pch.h"
#include <Neon/Neon/Nexa/Libraries/json.hpp>
using json = nlohmann::json;

namespace Nexa::Echo
{
    void CreateEchoSession();
    string FetchEchoSessionPlaylist();
    void EchoSessionStarted();
}