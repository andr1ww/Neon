#include "pch.h"
#include "../Header/FortProjectileBase.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"
#include "Engine/NetDriver/Header/NetDriver.h"
#include "Neon/Runtime/Runtime.h"

void AFortProjectileBase::OnStopCallback(AFortProjectileBase* ProjectileBase, FFrame& Stack)
{
    FHitResult Result;
    Stack.StepCompiledIn(&Result);
    Stack.IncrementCode();

    if (!ProjectileBase->IsA<AB_Prj_Athena_PlaysetGrenade_C>())
        return;

    auto* Playset = ((AB_Prj_Athena_PlaysetGrenade_C*)ProjectileBase)->GetCachedPlayset();
    if (!Playset || !Playset->GetClass())
        return;

    std::string PathName = UKismetSystemLibrary::GetDefaultObj()->CallFunc<FString>(
        "KismetSystemLibrary", "GetPathName", Playset).ToString().c_str();

    size_t LastDot = PathName.find_last_of('.');
    if (LastDot == std::string::npos) return;

    std::string AssetName = PathName.substr(LastDot + 1);

    if (AssetName.rfind("PID_", 0) == 0)
    {
        size_t pos = AssetName.find("PG_");
        if (pos == std::string::npos) pos = AssetName.find("CP_");
        if (pos != std::string::npos) AssetName = AssetName.substr(pos);
    }

    if (!(AssetName.rfind("CP_", 0) == 0 || AssetName.rfind("PG_", 0) == 0))
        return;

    std::vector<std::string> parts;
    size_t start = 0, pos = 0;
    while ((pos = AssetName.find('_', start)) != std::string::npos)
    {
        if (pos > start) parts.push_back(AssetName.substr(start, pos - start));
        start = pos + 1;
    }
    if (start < AssetName.length()) parts.push_back(AssetName.substr(start));
    if (parts.size() < 2) return;

    std::string BasePath;
    if (AssetName.rfind("CP_", 0) == 0)
    {
        if (parts[1] == "Devices")
            BasePath = "/Game/Creative/Maps/Devices/";
        else
            BasePath = "/Game/Creative/Maps/Prefabs/";
    }
    else
    {
        BasePath = "/Game/Playgrounds/Maps/Playsets/";
    }

    std::string AssetFull = AssetName + "." + AssetName;
    UWorld* World = nullptr;

    std::vector<std::string> TryPaths = {
        BasePath + parts[1] + "/Gallery/" + AssetFull,
        BasePath + parts[1] + "/" + AssetFull,
        BasePath + AssetFull
    };

    if (parts.size() >= 3)
    {
        if (parts[2] == "S") TryPaths.push_back(BasePath + parts[1] + "/Small/" + AssetFull);
        if (parts[2] == "M") TryPaths.push_back(BasePath + parts[1] + "/Medium/" + AssetFull);
        if (parts[2] == "L") TryPaths.push_back(BasePath + parts[1] + "/Large/" + AssetFull);
    }

    for (auto& Path : TryPaths)
    {
        World = Runtime::StaticLoadObject<UWorld>(Path);
        if (World) break;
    }

    if (World)
    {
        FVector Location = ProjectileBase->K2_GetActorLocation();
        if (ULevel* Level = World->GetPersistentLevel())
        {
            auto& Actors = *reinterpret_cast<TArray<class AActor*>*>(
                reinterpret_cast<uintptr_t>(Level) + 0x98);

            for (auto& Actor : Actors)
            {
                if (!Actor || !Actor->GetRootComponent()) continue;

                FVector SpawnLoc = Location + Actor->GetRootComponent()->GetRelativeLocation();
                FRotator SpawnRot = Actor->GetRootComponent()->GetRelativeRotation();

                UGameplayStatics::SpawnActorOG<AActor>(Actor->GetClass(), SpawnLoc, SpawnRot);
            }
        }
    }

    struct { FHitResult Hit; } Params{Result};
    callExecOG(ProjectileBase, "/Script/FortniteGame.FortProjectileBase.OnStopCallback", OnStopCallback, Params);
    ProjectileBase->K2_DestroyActor();
}
