#include "pch.h"
#include "../Header/FortProjectileBase.h"

#include "Neon/Runtime/Runtime.h"

void AFortProjectileBase::OnStopCallback(AFortProjectileBase* Projectile, FFrame& Stack)
{
    FHitResult Result;
    Stack.StepCompiledIn(&Result);
    Stack.IncrementCode();
    
    UE_LOG(LogNeon, Log, "Base: %s", Projectile->GetFName().ToString().ToString().c_str());

    struct { FHitResult Hit;} Params{Result};
    
    callExecOG(Projectile, "/Script/FortniteGame.FortProjectileBase.OnStopCallback", OnStopCallback, Params);
}
