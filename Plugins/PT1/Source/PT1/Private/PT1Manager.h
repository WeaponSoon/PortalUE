// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PT1Manager.generated.h"

UCLASS()
class APT1Manager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APT1Manager();

	struct IgnorePair
	{
		TWeakObjectPtr<USceneComponent> obj1;
		TWeakObjectPtr<USceneComponent> obj2;
	};

	TArray<IgnorePair> ignoreComponentsPair;

protected:
	static APT1Manager* m_PTManager;
	static class UWorld* m_CurrentWorld;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	const TArray<IgnorePair>& GetIgnoreComponentsPair() const;
	bool IgnoreBetween(USceneComponent* a, USceneComponent* b);
	bool RemoveIgnoreBetween(USceneComponent* a, USceneComponent* b);

	virtual void Init(class UWorld* world);

	static APT1Manager* GetPT1Manager();
	static UWorld* GetCurrentWorld();
};
