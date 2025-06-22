#pragma once
#include "pch.h"

#include "FortniteGame/FortPlayerController/Header/FortPlayerController.h"


struct FFortItemEntry : FFastArraySerializerItem
{
};

class AFortInventory : public UObject
{
public:
    void Update(AFortPlayerControllerAthena* PlayerController, UObject* Entry);
    UObject* GiveItem(AFortPlayerControllerAthena* PlayerController, UObject* Def, int Count, int LoadedAmmo, int Level);
};
