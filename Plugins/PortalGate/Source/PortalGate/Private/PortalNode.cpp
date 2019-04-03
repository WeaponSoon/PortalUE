// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalNode.h"
#include "Engine/TextureRenderTarget2D.h"
#include "PortalTree.h"
PortalNode::PortalNode()
{
	renderTarget = nullptr;
	portalDoorComponent = nullptr;
	parent = nullptr;
}

PortalNode::~PortalNode()
{
}
void PortalNode::SetLayer(int layer)
{
	belongToLayer = layer;
}
int PortalNode::GetLayer() const
{
	return belongToLayer;
}
void PortalNode::SetRenderTexture(class UTextureRenderTarget2D* target)
{
    this->renderTarget = target;
}

UTextureRenderTarget2D* PortalNode::GetRenderTexture2D() const
{
    return this->renderTarget;
}

void PortalNode::AddChild(const PortalNode* child)
{
    this->children.Add(const_cast<PortalNode*>(child));
}

void PortalNode::RecycleChildren()
{
    
}

void PortalNode::Reset()
{
	portalDoorComponent = nullptr;
	children.Reset();
	motherTree = nullptr;
	parent = nullptr;
}

void PortalNode::AddReferencedObjects( FReferenceCollector& Collector )
{
    Collector.AddReferencedObject(renderTarget);
	Collector.AddReferencedObject(portalDoorComponent);
}
