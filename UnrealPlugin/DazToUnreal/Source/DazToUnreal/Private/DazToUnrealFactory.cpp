// Copyright 2018-2019 David Vodhanel. All Rights Reserved.

#include "DazToUnrealFactory.h"
#include "DazToUnreal.h"
#include "Modules/ModuleManager.h"
#include "Misc/PackageName.h"
#include "AssetData.h"
#include "AssetRegistryModule.h"
#include "PackageTools.h"
#include "Misc/FileHelper.h"
#include "Engine/SkeletalMesh.h"

#define LOCTEXT_NAMESPACE "DazToUnreal"

/////////////////////////////////////////////////////
// UDazToUnrealFactory

UDazToUnrealFactory::UDazToUnrealFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = false;
	bEditorImport = true;
	Formats.Add(TEXT("dtu;DazToUnreal description file"));
	SupportedClass = USkeletalMesh::StaticClass();
}


UObject* UDazToUnrealFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	FString Message;
	FFileHelper::LoadFileToString(Message, *Filename);
	TArray<FString> Parts;
	Message.ParseIntoArray(Parts, TEXT("|"));
	FString FBXPath;
	FString AssetName;
	FString AssetType;
	FString ImportFolder;
	for (FString Part : Parts)
	{
		FString Name, Value;
		Part.Split(TEXT("="), &Name, &Value);
		if (Name == TEXT("FBXFILE"))
		{
			FBXPath = Value;
		}
		if (Name == TEXT("ASSETNAME"))
		{
			AssetName = Value;
		}
		if (Name == TEXT("ASSETTYPE"))
		{
			AssetType = Value;
		}
		if (Name == TEXT("IMPORTFOLDER"))
		{
			ImportFolder = Value.TrimEnd();
		}
	}

	DazAssetType AssetTypeEnum = DazAssetType::StaticMesh;
	if (AssetType == TEXT("SkeletalMesh"))
	{
		AssetTypeEnum = DazAssetType::SkeletalMesh;
	}
	if (AssetType == TEXT("Animation"))
	{
		AssetTypeEnum = DazAssetType::Animation;;
	}

	return FDazToUnrealModule::Get().ImportFromDaz(FBXPath, AssetName, AssetTypeEnum, ImportFolder);
}

#undef LOCTEXT_NAMESPACE
