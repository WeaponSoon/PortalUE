// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/MeshComponent.h"
#include "PortalDoorComponent.generated.h"

/**
 * 
 */
class FPrimitiveSceneProxy;

USTRUCT(BlueprintType)
struct FPortalDoorMeshTriangle
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Triangle)
		FVector Vertex0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Triangle)
		FVector Vertex1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Triangle)
		FVector Vertex2;
};

UCLASS(meta = (BlueprintSpawnableComponent), ClassGroup = Rendering)
class PORTALGATE_API UPortalDoorComponent : public UMeshComponent
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintSetter, Category = "Components|PortalDoor")
		void SetPortalDoorHeight(float height);
	UFUNCTION(BlueprintSetter, Category = "Components|PortalDoor")
		void SetPortalDoorWidth(float width);
	UFUNCTION(BlueprintGetter, Category = "Components|PortalDoor")
		float GetPortalDoorHeight() const;
	UFUNCTION(BlueprintGetter, Category = "Components|PortalDoor")
		float GetPortalDoorWidth() const;

	UFUNCTION(BlueprintCallable, Category = "Components|PortalDoor")
		bool SetPortalDoorMeshTriangles(const TArray<FPortalDoorMeshTriangle>& Triangles);

	UFUNCTION(BlueprintCallable, Category = "Components|PortalDoor")
		void AddPortalDoorMeshTriangles(const TArray<FPortalDoorMeshTriangle>& Triangles);

	UFUNCTION(BlueprintCallable, Category = "Components|PortalDoor")
		void ClearPortalDoorMeshTriangles();
private:
	float m_PortalDoorHeight;
	float m_PortalDoorWidth;

	TArray<FPortalDoorMeshTriangle> CustomMeshTris;

	friend class FPortalDoorSceneProxy;

	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	//~ End UPrimitiveComponent Interface.

	//~ Begin UMeshComponent Interface.
	virtual int32 GetNumMaterials() const override;
	//~ End UMeshComponent Interface.

	//~ Begin USceneComponent Interface.
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ Begin USceneComponent Interface.
};
