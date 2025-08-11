#include "pch.h"
#include "../Header/FortGameState.h"

void AFortGameStateAthena::OnRep_CurrentPlaylistId()
{
    static class UFunction* Func = nullptr;
    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortGameStateAthena", "OnRep_CurrentPlaylistId");

    if (Func == nullptr)
        Func = Info.Func;
    if (!Func)
        return;
    
    this->ProcessEvent(Func, nullptr);
}

void AFortGameStateAthena::OnRep_CurrentPlaylistInfo()
{
    static class UFunction* Func = nullptr;
    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortGameStateAthena", "OnRep_CurrentPlaylistInfo");

    if (Func == nullptr)
        Func = Info.Func;
    if (!Func)
        return;
    
    this->ProcessEvent(Func, nullptr);
}

void AFortGameStateAthena::OnRep_CurrentPlaylistData()
{
    static class UFunction* Func = nullptr;
    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortGameStateAthena", "OnRep_CurrentPlaylistData");

    if (Func == nullptr)
        Func = Info.Func;
    if (!Func)
        return;
    
    this->ProcessEvent(Func, nullptr);
}