#include "pch.h"
#include "../Header/FortKismetLibrary.h"

UFortResourceItemDefinition* UFortKismetLibrary::K2_GetResourceItemDefinition(const EFortResourceType ResourceType)
{
    static SDK::UFunction* Func = nullptr;
    SDK::FFunctionInfo Info = SDK::PropLibrary->GetFunctionByName("FortKismetLibrary", "K2_GetResourceItemDefinition");

    if (Func == nullptr)
        Func = Info.Func;
    if (!Func)
        return nullptr;

    struct { EFortResourceType type; UFortResourceItemDefinition* ret; } Params{ResourceType};
    static UObject* Object = nullptr;
    if (!Object) Object = SDK::StaticClassImpl("FortKismetLibrary")->GetClassDefaultObject();
    
    Object->ProcessEvent(Func, &Params);

    return Params.ret;
}
