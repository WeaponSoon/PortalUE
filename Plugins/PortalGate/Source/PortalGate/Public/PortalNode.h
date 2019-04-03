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
	/*Start-�������͵Ķ���ᱻ�������գ���Ա��Ҫ���Ϊǿ����*/
	///�˽ڵ��Ӧ�Ĵ����ŵ���ȾĿ��
    class UTextureRenderTarget2D* renderTarget;
	///�˽ڵ��Ӧ�Ĵ��������
	class UPortalDoorComponent* portalDoorComponent;
	/* End -�������͵Ķ���ᱻ�������գ���Ա��Ҫ���Ϊǿ����*/

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
