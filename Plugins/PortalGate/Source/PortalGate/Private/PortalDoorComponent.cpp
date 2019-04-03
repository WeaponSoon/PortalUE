// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalDoorComponent.h"
#include "RenderingThread.h"
#include "RenderResource.h"
#include "PrimitiveViewRelevance.h"
#include "PrimitiveSceneProxy.h"
#include "VertexFactory.h"
#include "MaterialShared.h"
#include "Engine/CollisionProfile.h"
#include "Materials/Material.h"
#include "LocalVertexFactory.h"
#include "SceneManagement.h"
#include "DynamicMeshBuilder.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "StaticMeshResources.h"

class FPortalDoorSceneProxy final : public FPrimitiveSceneProxy
{
private:
	UMaterialInterface* Material;
	FStaticMeshVertexBuffers VertexBuffers;
	FDynamicMeshIndexBuffer32 IndexBuffer;
	FLocalVertexFactory VertexFactory;
	FMaterialRelevance MaterialRelevance;

public:
	SIZE_T GetTypeHash() const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}

    FPortalDoorSceneProxy(UPortalDoorComponent* Component)
		: FPrimitiveSceneProxy(Component)
		, VertexFactory(GetScene().GetFeatureLevel(), "FCustomMeshSceneProxy")
		, MaterialRelevance(Component->GetMaterialRelevance(GetScene().GetFeatureLevel()))
	{
		const FColor VertexColor(255,255,255);

		TArray<FDynamicMeshVertex> Vertices;
		const int32 NumTris = Component->CustomMeshTris.Num();
		Vertices.AddUninitialized(NumTris * 3);
		IndexBuffer.Indices.AddUninitialized(NumTris * 3);
		// Add each triangle to the vertex/index buffer
		for(int32 TriIdx = 0; TriIdx < NumTris; TriIdx++)
		{
			FPortalDoorMeshTriangle& Tri = Component->CustomMeshTris[TriIdx];

			const FVector Edge01 = (Tri.Vertex1 - Tri.Vertex0);
			const FVector Edge02 = (Tri.Vertex2 - Tri.Vertex0);

			const FVector TangentX = Edge01.GetSafeNormal();
			const FVector TangentZ = (Edge02 ^ Edge01).GetSafeNormal();
			const FVector TangentY = (TangentX ^ TangentZ).GetSafeNormal();

			FDynamicMeshVertex Vert;
			
			Vert.Color = VertexColor;
			Vert.SetTangents(TangentX, TangentY, TangentZ);

			Vert.Position = Tri.Vertex0;
			Vertices[TriIdx * 3 + 0] = Vert;
			IndexBuffer.Indices[TriIdx * 3 + 0] = TriIdx * 3 + 0;

			Vert.Position = Tri.Vertex1;
			Vertices[TriIdx * 3 + 1] = Vert;
			IndexBuffer.Indices[TriIdx * 3 + 1] = TriIdx * 3 + 1;

			Vert.Position = Tri.Vertex2;
			Vertices[TriIdx * 3 + 2] = Vert;
			IndexBuffer.Indices[TriIdx * 3 + 2] = TriIdx * 3 + 2;
		}

		VertexBuffers.InitFromDynamicVertex(&VertexFactory, Vertices);

		// Enqueue initialization of render resource
		BeginInitResource(&VertexBuffers.PositionVertexBuffer);
		BeginInitResource(&VertexBuffers.StaticMeshVertexBuffer);
		BeginInitResource(&VertexBuffers.ColorVertexBuffer);
		BeginInitResource(&IndexBuffer);
		BeginInitResource(&VertexFactory);

		// Grab material
		Material = Component->GetMaterial(0);
		if(Material == NULL)
		{
			Material = UMaterial::GetDefaultMaterial(MD_Surface);
		}
	}

	virtual ~FPortalDoorSceneProxy()
	{
		VertexBuffers.PositionVertexBuffer.ReleaseResource();
		VertexBuffers.StaticMeshVertexBuffer.ReleaseResource();
		VertexBuffers.ColorVertexBuffer.ReleaseResource();
		IndexBuffer.ReleaseResource();
		VertexFactory.ReleaseResource();
	}
	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_CustomMeshSceneProxy_GetDynamicMeshElements);

		const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;

		auto WireframeMaterialInstance = new FColoredMaterialRenderProxy(
			GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy(IsSelected()) : NULL,
			FLinearColor(0, 0.5f, 1.f)
		);

		Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);

		FMaterialRenderProxy* MaterialProxy = NULL;
		if (bWireframe)
		{
			MaterialProxy = WireframeMaterialInstance;
		}
		else
		{
			MaterialProxy = Material->GetRenderProxy(IsSelected());
		}

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				const FSceneView* View = Views[ViewIndex];
				// Draw the mesh.
				FMeshBatch& Mesh = Collector.AllocateMesh();
				FMeshBatchElement& BatchElement = Mesh.Elements[0];
				BatchElement.IndexBuffer = &IndexBuffer;
				Mesh.bWireframe = bWireframe;
				Mesh.VertexFactory = &VertexFactory;
				Mesh.MaterialRenderProxy = MaterialProxy;
				BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, UseEditorDepthTest());
				BatchElement.FirstIndex = 0;
				BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / 3;
				BatchElement.MinVertexIndex = 0;
				BatchElement.MaxVertexIndex = VertexBuffers.PositionVertexBuffer.GetNumVertices() - 1;
				Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
				Mesh.Type = PT_TriangleList;
				Mesh.DepthPriorityGroup = SDPG_World;
				Mesh.bCanApplyViewModeOverrides = false;
				Collector.AddMesh(ViewIndex, Mesh);
			}
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = IsShown(View);
		Result.bShadowRelevance = IsShadowCast(View);
		Result.bDynamicRelevance = true;
		Result.bRenderInMainPass = ShouldRenderInMainPass();
		Result.bUsesLightingChannels = GetLightingChannelMask() != GetDefaultLightingChannelMask();
		Result.bRenderCustomDepth = ShouldRenderCustomDepth();
		MaterialRelevance.SetPrimitiveViewRelevance(Result);
		return Result;
	}

	virtual bool CanBeOccluded() const override
	{
		return !MaterialRelevance.bDisableDepthTest;
	}

	virtual uint32 GetMemoryFootprint(void) const override { return(sizeof(*this) + GetAllocatedSize()); }

	uint32 GetAllocatedSize(void) const { return(FPrimitiveSceneProxy::GetAllocatedSize()); }
};

TArray<UPortalDoorComponent*> UPortalDoorComponent::AllPortalDoors;
const TArray<UPortalDoorComponent*>& UPortalDoorComponent::GetAllPortalDoors()
{
	return AllPortalDoors;
}

UPortalDoorComponent::UPortalDoorComponent(const FObjectInitializer& initializer) : Super(initializer)
{
	SetPortalDoorWidth(300);
	SetPortalDoorHeight(400);
}

void UPortalDoorComponent::SetPortalDoorHeight(float height)
{
	m_PortalDoorHeight = height;
	FPortalDoorMeshTriangle triggle;
	triggle.Vertex0 = FVector(0, -m_PortalDoorWidth, m_PortalDoorHeight);
	triggle.Vertex1 = FVector(0, m_PortalDoorWidth, m_PortalDoorHeight);
	triggle.Vertex2 = FVector(0, -m_PortalDoorWidth, -m_PortalDoorHeight);
	FPortalDoorMeshTriangle triggle1;
	triggle1.Vertex0 = FVector(0, -m_PortalDoorWidth, -m_PortalDoorHeight);
	triggle1.Vertex1 = FVector(0, m_PortalDoorWidth, m_PortalDoorHeight);
	triggle1.Vertex2 = FVector(0, m_PortalDoorWidth, -m_PortalDoorHeight);
	SetPortalDoorMeshTriangles({ triggle, triggle1 });
}

void UPortalDoorComponent::SetPortalDoorWidth(float width)
{
	m_PortalDoorWidth = width;
	FPortalDoorMeshTriangle triggle;
	triggle.Vertex0 = FVector(0, -m_PortalDoorWidth, m_PortalDoorHeight);
	triggle.Vertex1 = FVector(0, m_PortalDoorWidth, m_PortalDoorHeight);
	triggle.Vertex2 = FVector(0, -m_PortalDoorWidth, -m_PortalDoorHeight);
	FPortalDoorMeshTriangle triggle1;
	triggle1.Vertex0 = FVector(0, -m_PortalDoorWidth, -m_PortalDoorHeight);
	triggle1.Vertex1 = FVector(0, m_PortalDoorWidth, m_PortalDoorHeight);
	triggle1.Vertex2 = FVector(0, m_PortalDoorWidth, -m_PortalDoorHeight);
	SetPortalDoorMeshTriangles({ triggle, triggle1 });
}

float UPortalDoorComponent::GetPortalDoorHeight() const
{
	return m_PortalDoorHeight;
}

float UPortalDoorComponent::GetPortalDoorWidth() const
{
	return m_PortalDoorWidth;
}

bool UPortalDoorComponent::SetPortalDoorMeshTriangles(const TArray<FPortalDoorMeshTriangle>& Triangles)
{
	CustomMeshTris = Triangles;

	//重新创建SceneProxy
	MarkRenderStateDirty();
	UpdateBounds();

	return true;
}

void UPortalDoorComponent::AddPortalDoorMeshTriangles(const TArray<FPortalDoorMeshTriangle>& Triangles)
{
	CustomMeshTris.Append(Triangles);
	MarkRenderStateDirty();
	UpdateBounds();
}

void UPortalDoorComponent::ClearPortalDoorMeshTriangles()
{
	CustomMeshTris.Reset();
	MarkRenderStateDirty();
	UpdateBounds();
}

void UPortalDoorComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!AllPortalDoors.Contains(this))
	{
		AllPortalDoors.Add(this);
	}
}

void UPortalDoorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AllPortalDoors.Contains(this))
	{
		AllPortalDoors.Remove(this);
	}
	Super::EndPlay(EndPlayReason);
}

FPrimitiveSceneProxy* UPortalDoorComponent::CreateSceneProxy()
{
	FPrimitiveSceneProxy* Proxy = NULL;
	if (CustomMeshTris.Num() > 0)
	{
		Proxy = new FPortalDoorSceneProxy(this);
	}
	return Proxy;
}

int32 UPortalDoorComponent::GetNumMaterials() const
{
	return 1;
}


FBoxSphereBounds UPortalDoorComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	FBox BoundingBox(ForceInit);

	for (int32 Index = 0; Index < CustomMeshTris.Num(); ++Index)
	{
		BoundingBox += LocalToWorld.TransformPosition(CustomMeshTris[Index].Vertex0);
		BoundingBox += LocalToWorld.TransformPosition(CustomMeshTris[Index].Vertex1);
		BoundingBox += LocalToWorld.TransformPosition(CustomMeshTris[Index].Vertex2);
	}

	FBoxSphereBounds NewBounds;
	NewBounds.BoxExtent = BoundingBox.GetExtent();
	NewBounds.Origin = BoundingBox.GetCenter();
	NewBounds.SphereRadius = NewBounds.BoxExtent.Size();

	return NewBounds;
}