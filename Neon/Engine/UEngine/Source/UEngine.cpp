#include "pch.h"
#include "../Header/UEngine.h"

UEngine* UEngine::GetEngine()
{
    static UEngine* GEngine = nullptr;
    if (!GEngine)
    {
        for (int i = 0; i < GUObjectArray.GetObjectArrayNum(); i++)
        {
            FUObjectItem* Object = GUObjectArray.IndexToObject(i);
            if (!Object) continue;
            UObjectBase* GObject = Object->Object;
            if (!GObject) continue;
            if (GObject->GetFName().ToString().ToString().contains("FortEngine_"))
            {
                GEngine = (UEngine*)Object->Object;
            }
        }
    }

    return GEngine;
}
