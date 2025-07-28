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

    SDK::StaticClassImpl("FortKismetLibrary")->GetClassDefaultObject()->ProcessEvent(Func, &Params);

    return Params.ret;
}
