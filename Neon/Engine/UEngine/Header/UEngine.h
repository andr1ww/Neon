#pragma once
#include "pch.h"

class UEngine : public UObject
{
public:
    DEFINE_MEMBER(class UGameViewportClient*, UEngine, GameViewport);
    DECLARE_STATIC_CLASS(UEngine);
    DECLARE_DEFAULT_OBJECT(UEngine);
    static UEngine* GetEngine();
};

class UGameViewportClient : public UObject
{
public:
    DEFINE_MEMBER(class UWorld*, UGameViewportClient, World);
public:
    DECLARE_STATIC_CLASS(UGameViewportClient);
    DECLARE_DEFAULT_OBJECT(UGameViewportClient);
};