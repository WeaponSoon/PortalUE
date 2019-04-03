// Fill out your copyright notice in the Description page of Project Settings.

#include "PT1Manager.h"


APT1Manager* APT1Manager::m_PTManager = nullptr;
UWorld* APT1Manager::m_CurrentWorld = nullptr;
UWorld* APT1Manager::GetCurrentWorld()
{
	return m_CurrentWorld;
}
APT1Manager* APT1Manager::GetPT1Manager()
{
	return m_PTManager;
}
// Sets default values
APT1Manager::APT1Manager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APT1Manager::BeginPlay()
{
	Super::BeginPlay();
	m_CurrentWorld = GetWorld();
	m_PTManager = this;
}

// Called every frame
void APT1Manager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APT1Manager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	m_PTManager = nullptr;
	m_CurrentWorld = nullptr;
}

const TArray<APT1Manager::IgnorePair>& APT1Manager::GetIgnoreComponentsPair() const
{
	return ignoreComponentsPair;
}

// 添加一对物理物体使其忽略之间的物理碰撞
bool APT1Manager::IgnoreBetween(USceneComponent* a, USceneComponent* b)
{

	TArray<int32> recoveryPair;
	for (int32 i = 0; i < ignoreComponentsPair.Num(); ++i)
	{
		if (!ignoreComponentsPair[i].obj1.IsValid() || !ignoreComponentsPair[i].obj2.IsValid())
		{
			recoveryPair.Add(i);
		}
	}
	for (int32 i = 0; i < recoveryPair.Num(); ++i)
	{
		ignoreComponentsPair.RemoveAt(recoveryPair[i]);
	}
	recoveryPair.Empty();
	for (int32 i = 0; i < ignoreComponentsPair.Num(); ++i)
	{
		if ((ignoreComponentsPair[i].obj1.Get() == a && ignoreComponentsPair[i].obj2.Get() == b) ||
			(ignoreComponentsPair[i].obj2.Get() == a && ignoreComponentsPair[i].obj1.Get() == b))
		{
			recoveryPair.Add(i);
		}
	}
	for (int32 i = 0; i < recoveryPair.Num(); ++i)
	{
		ignoreComponentsPair.RemoveAt(recoveryPair[i]);
	}
	if (a != nullptr && b != nullptr)
	{
		IgnorePair temp;
		temp.obj1 = a;
		temp.obj2 = b;
		ignoreComponentsPair.Add(temp);
		return true;
	}
	return false;
}

// 移除一对已经标记为互相忽略物理的物体
bool APT1Manager::RemoveIgnoreBetween(USceneComponent * a, USceneComponent * b)
{
	IgnoreBetween(nullptr, nullptr);
	TArray<int32> willDels;
	for (int32 i = 0; i < ignoreComponentsPair.Num(); ++i)
	{
		if ((ignoreComponentsPair[i].obj1.Get() == a && ignoreComponentsPair[i].obj2.Get() == b) ||
			(ignoreComponentsPair[i].obj2.Get() == a && ignoreComponentsPair[i].obj1.Get() == b))
		{
			willDels.Add(i);
		}
	}
	bool res = false;
	for (int32 i = 0; i < willDels.Num(); ++i)
	{
		ignoreComponentsPair.RemoveAt(willDels[i]);
		res = true;
	}
	return res;
}

void APT1Manager::Init(UWorld* world)
{
	m_CurrentWorld = world;
	m_PTManager = this;
}