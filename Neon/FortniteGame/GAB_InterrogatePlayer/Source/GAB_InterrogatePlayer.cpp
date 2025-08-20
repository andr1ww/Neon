#include "pch.h"
#include "../Header/GAB_InterrogatePlayer.h"

#include "Engine/GameplayStatics/Header/GameplayStatics.h"

void UGAB_InterrogatePlayer_Reveal_C::EndInterrogation(UGAB_InterrogatePlayer_Reveal_C* _this, FFrame& Stack)
{
    Stack.IncrementCode();
    static auto ChestClass = Runtime::StaticLoadObject<UClass>("/Game/Building/ActorBlueprints/Containers/Tiered_Chest_Athena.Tiered_Chest_Athena_C");
    
    TArray<AActor*> Actors = UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), ChestClass);

    _this->SetActorsToMark(Actors);

    UE_LOG(LogNeon, Log, __FUNCTION__);
}
