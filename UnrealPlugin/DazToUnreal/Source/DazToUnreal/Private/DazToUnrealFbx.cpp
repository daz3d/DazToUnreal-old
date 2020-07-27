#include "DazToUnrealFbx.h"



void FDazToUnrealFbx::RenameDuplicateBones(FbxNode* RootNode)
{
	TMap<FString, int> ExistingBones;
	RenameDuplicateBones(RootNode, ExistingBones);
}

void FDazToUnrealFbx::RenameDuplicateBones(FbxNode* RootNode, TMap<FString, int>& ExistingBones)
{
	if (RootNode == nullptr) return;

	FbxNodeAttribute* Attr = RootNode->GetNodeAttribute();
	if (Attr && Attr->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		FString BoneName = UTF8_TO_TCHAR(RootNode->GetName());
		if (ExistingBones.Contains(BoneName))
		{
			ExistingBones[BoneName] += 1;
			BoneName = FString::Printf(TEXT("%s_RENAMED_%d"), *BoneName, ExistingBones[BoneName]);
			RootNode->SetName(TCHAR_TO_UTF8(*BoneName));
		}
		else
		{
			ExistingBones.Add(BoneName, 1);
		}
	}

	for (int ChildIndex = 0; ChildIndex < RootNode->GetChildCount(); ++ChildIndex)
	{
		FbxNode * ChildNode = RootNode->GetChild(ChildIndex);
		RenameDuplicateBones(ChildNode, ExistingBones);
	}
}