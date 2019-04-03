// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/GCObject.h"

/**
 * 
 */
class PORTALGATE_API PortalNode : public FGCObject
{
public:
	PortalNode();
	~PortalNode();
public:
	/*Start-以下类型的对象会被垃圾回收，成员需要标记为强引用*/
	///此节点对应的传送门的渲染目标
    class UTextureRenderTarget2D* renderTarget;
	///此节点对应的传送门组件
	class UPortalDoorComponent* portalDoorComponent;
	/* End -以上类型的对象会被垃圾回收，成员需要标记为强引用*/

	class PortalTree* motherTree;
	PortalNode* parent;
	TArray<PortalNode*> children;
    FVector planePos;
    FVector planeNor;
    FTransform cameraPos;
	int belongToLayer;
public:
	void SetLayer(int layer);
	int GetLayer() const;
    void SetRenderTexture(class UTextureRenderTarget2D* target);
    class UTextureRenderTarget2D* GetRenderTexture2D() const;
    void AddChild(const PortalNode* child);
    void RecycleChildren();
	void Reset();
protected:
    virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;
};
