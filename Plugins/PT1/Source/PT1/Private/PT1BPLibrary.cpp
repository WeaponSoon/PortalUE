// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "PT1BPLibrary.h"
#include "PT1.h"
#include "PT1Manager.h"
#include "Engine.h"
#include "Engine/World.h"

UPT1BPLibrary::UPT1BPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

float UPT1BPLibrary::PT1SampleFunction(float Param)
{
	
	return -1;
}

bool UPT1BPLibrary::RemoveIgnoreBetween(const UObject* WorldContextObject, USceneComponent* a, USceneComponent* b)
{
	auto manager = APT1Manager::GetPT1Manager();
	if (manager == nullptr)
	{
		UWorld* world = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
		if (world == nullptr)
			return false;
		manager = world->SpawnActor<APT1Manager>();
		manager->Init(world);
	}
	if (manager == nullptr)
		return false;
	return manager->RemoveIgnoreBetween(a, b);
}

bool UPT1BPLibrary::IgnoreBetween(const UObject* WorldContextObject, USceneComponent* a, USceneComponent* b)
{
	auto manager = APT1Manager::GetPT1Manager();
	if (manager == nullptr)
	{
		UWorld* world = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
		if (world == nullptr)
			return false;
		manager = world->SpawnActor<APT1Manager>();
		manager->Init(world);
	}
	if (manager == nullptr)
		return false;
	return manager->IgnoreBetween(a, b);
}