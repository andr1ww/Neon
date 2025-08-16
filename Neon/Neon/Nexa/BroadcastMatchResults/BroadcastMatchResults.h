#pragma once
#include "pch.h"

#include "FortniteGame/FortGameMode/Header/FortGameMode.h"
#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"
#include <Neon/Neon/Nexa/Libraries/json.hpp>

using json = nlohmann::json;

namespace Nexa
{
    void BroadcastMatchResults(AFortPlayerControllerAthena*, AFortPlayerStateAthena*, FPlaylistData, AFortGameModeAthena*);
}
