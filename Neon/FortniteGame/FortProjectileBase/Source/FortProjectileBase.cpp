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

    if (ProjectileBase->IsA<AB_Prj_Athena_PlaysetGrenade_C>())
    {
        if (auto* Playset = ((AB_Prj_Athena_PlaysetGrenade_C*)ProjectileBase)->GetCachedPlayset())
        {
            if (Playset->GetClass())
            {
                std::string PathName = UKismetSystemLibrary::GetDefaultObj()->CallFunc<FString>(
                    "KismetSystemLibrary", "GetPathName", Playset).ToString().c_str();

                size_t LastDot = PathName.find_last_of('.');
                if (LastDot != std::string::npos)
                {
                    std::string AssetName = PathName.substr(LastDot + 1);

                    if (AssetName.rfind("PID_", 0) == 0)
                    {
                        size_t pgPos = AssetName.find("PG_");
                        size_t cpPos = AssetName.find("CP_");
                        if (pgPos != std::string::npos) 
                            AssetName = AssetName.substr(pgPos);
                        else if (cpPos != std::string::npos) 
                            AssetName = AssetName.substr(cpPos);
                    }

                    if (AssetName.rfind("CP_", 0) == 0 || AssetName.rfind("PG_", 0) == 0)
                    {
                        std::vector<std::string> parts;
                        size_t start = 0, pos = 0;
                        while ((pos = AssetName.find('_', start)) != std::string::npos)
                        {
                            if (pos > start) parts.push_back(AssetName.substr(start, pos - start));
                            start = pos + 1;
                        }
                        if (start < AssetName.length()) 
                            parts.push_back(AssetName.substr(start));

                        if (parts.size() >= 3)
                        {
                            std::string BasePath;
                            if (parts[1] == "Devices")
                                BasePath = "/Game/Creative/Maps/Devices/";
                            else if (AssetName.rfind("CP_", 0) == 0)
                                BasePath = "/Game/Creative/Maps/Prefabs/";
                            else
                                BasePath = "/Game/Playgrounds/Maps/Playsets/";

                            std::string AssetFullName = AssetName + "." + AssetName;

                            std::string ConstructedPath;
                            UWorld* World = nullptr;

                            if (AssetName.find("Gallery") != std::string::npos)
                            {
                                ConstructedPath = BasePath + parts[1] + "/Gallery/" + AssetFullName;
                                World = Runtime::StaticLoadObject<UWorld>(ConstructedPath);
                            }

                            if (!World)
                            {
                                ConstructedPath = BasePath + parts[1] + "/" + AssetFullName;
                                World = Runtime::StaticLoadObject<UWorld>(ConstructedPath);
                            }

                            if (!World)
                            {
                                ConstructedPath = BasePath + AssetFullName;
                                World = Runtime::StaticLoadObject<UWorld>(ConstructedPath);
                            }

                            if (!World)
                            {
                                std::string SizeFolder;
                                if (parts[2] == "S") SizeFolder = "Small/";
                                else if (parts[2] == "M") SizeFolder = "Medium/";
                                else if (parts[2] == "L") SizeFolder = "Large/";
                                else if (parts[2] == "XL") SizeFolder = "XLarge/";

                                ConstructedPath = BasePath + parts[1] + "/" + SizeFolder + AssetFullName;
                                World = Runtime::StaticLoadObject<UWorld>(ConstructedPath);
                            }

                            if (World)
                            {
                                FVector ProjectileLocation = ProjectileBase->K2_GetActorLocation();
                                if (ULevel* Level = World->GetPersistentLevel())
                                {
                                    auto& Actors = *reinterpret_cast<TArray<class AActor*>*>(
                                        reinterpret_cast<uintptr_t>(Level) + 0x98);
                                    for (auto& Actor : Actors)
                                    {
                                        if (Actor && Actor->GetRootComponent())
                                        {
                                            FVector SpawnLocation = ProjectileLocation + Actor->GetRootComponent()->GetRelativeLocation();
                                            UGameplayStatics::SpawnActorOG<AActor>(
                                                Actor->GetClass(), 
                                                SpawnLocation, 
                                                Actor->GetRootComponent()->GetRelativeRotation()
                                            );
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    struct { FHitResult Hit; } Params{Result};
    callExecOG(ProjectileBase, "/Script/FortniteGame.FortProjectileBase.OnStopCallback", OnStopCallback, Params);

    ProjectileBase->K2_DestroyActor();
}
