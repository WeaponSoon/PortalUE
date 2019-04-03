// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/GCObject.h"

/**
 * 
 */
class PORTALGATE_API PortalTree : FGCObject
{
public:
	PortalTree();
	~PortalTree();
private:
	class PortalNode* root;
	void BuildPortalTreeInternal(PortalNode* root);
	void RenderPortalTreeInternal(PortalNode* root);
	
	class PortalNodePool* portalPool;

public:
	static const int POOL_MAX_LAYER;
public:
	void BuildPortalTree();
	void RenderPortalTree();

	void RecyclePortalNode(PortalNode* node);

protected:
	virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;
};
