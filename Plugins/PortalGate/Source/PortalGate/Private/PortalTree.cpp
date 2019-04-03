// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalTree.h"
#include "PortalNode.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine.h"
class PortalNodePool
{
public:
	PortalNodePool(int maxLayer)
	{
		maxLayer = maxLayer > 0 ? maxLayer : 1;
		for (int i = 0; i < maxLayer; ++i)
		{
			pool.Add(TArray<PortalNode*>());
		}
	}
	virtual ~PortalNodePool()
	{
		for (int i = 0; i < pool.Num(); ++i)
		{
			for (int j = 0; j < pool[i].Num(); ++j)
			{
				delete pool[i][j];
				pool[i][j] = nullptr;
			}
			pool[i].Empty();
		}
	}

	PortalNode* QueryPortalNode(const PortalTree* motherTree, int layer, PortalNode* parent)
	{
		layer = layer >= 0 ? layer : 0;
		layer = layer < pool.Num() ? layer : pool.Num() - 1;
		auto& layerPool = pool[layer];
		if (layerPool.Num() > 0)
		{
			auto temp = layerPool.Pop();
			temp->motherTree = const_cast<PortalTree*>(motherTree);
			temp->parent = parent;
			if (parent != nullptr)
			{
				parent->AddChild(temp);
			}
			return temp;
		}
		else
		{
			PortalNode* ret = new PortalNode();
			ret->Reset();
			ret->motherTree = const_cast<PortalTree*>(motherTree);
			ret->SetLayer(layer);
			ret->parent = parent;
			if (parent != nullptr)
			{
				parent->AddChild(ret);
			}
			ret->SetRenderTexture(NewObject<UTextureRenderTarget2D>());
			UTextureRenderTarget2D* temTxt = ret->GetRenderTexture2D();
			temTxt->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
			FIntPoint reslution = GEngine->GameViewport->Viewport->GetSizeXY();
			float ratio = FMath::Sqrt(layer + 1);
			temTxt->InitAutoFormat(reslution.X / ratio, reslution.Y / ratio);
			temTxt->UpdateResourceImmediate(true);
			return ret;
		}
	}

	void RecyclePortalNode(PortalNode* node)
	{
		if (node == nullptr)
			return;
		for (int i = 0; i < node->children.Num(); ++i)
		{
			RecyclePortalNode(node->children[i]);
		}
		node->Reset();
		pool[node->GetLayer()].Add(node);
	}

private:
	TArray<TArray<PortalNode*>> pool;
	
};


const int PortalTree::POOL_MAX_LAYER = 3;
PortalTree::PortalTree()
{
    root = nullptr;
	portalPool = new PortalNodePool(POOL_MAX_LAYER);
	root = portalPool->QueryPortalNode(this, 0, nullptr);
}

PortalTree::~PortalTree()
{
	RecyclePortalNode(root);
	root = nullptr;
	delete portalPool;
}

void PortalTree::AddReferencedObjects( FReferenceCollector& Collector )
{

}

void PortalTree::BuildPortalTreeInternal(PortalNode* root)
{

}

void PortalTree::RenderPortalTreeInternal(PortalNode * root)
{
}

void PortalTree::BuildPortalTree()
{

}

void PortalTree::RenderPortalTree()
{
}

void PortalTree::RecyclePortalNode(PortalNode * node)
{
	portalPool->RecyclePortalNode(node);
}
