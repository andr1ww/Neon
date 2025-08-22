#pragma once
#include "pch.h"

class UGameEngine;

class UEngine : public UObject
{
public:
    DEFINE_MEMBER(class UGameViewportClient*, UEngine, GameViewport);
    DECLARE_STATIC_CLASS(UEngine);
    DECLARE_DEFAULT_OBJECT(UEngine);
    static UEngine* GetEngine();
    static UGameEngine* GetGameEngine();
};

class UGameEngine : public UEngine
{
public:
};

class UFortEngine : public UEngine
{
public:
 
};

class UGameViewportClient : public UObject
{
public:
    DEFINE_PTR(class UWorld, UGameViewportClient, World);
public:
    DECLARE_STATIC_CLASS(UGameViewportClient);
    DECLARE_DEFAULT_OBJECT(UGameViewportClient);
};