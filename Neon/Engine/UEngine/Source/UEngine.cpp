#include "pch.h"
#include "../Header/UEngine.h"

UEngine* UEngine::GetEngine()
{
    return (UEngine*)TUObjectArray::FindFirstObject(L"FortEngine");
}

UGameEngine* UEngine::GetGameEngine()
{
	return static_cast<UGameEngine*>(UEngine::GetEngine());
}
