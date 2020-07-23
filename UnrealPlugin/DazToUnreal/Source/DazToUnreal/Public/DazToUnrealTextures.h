// Copyright 2018-2020 David Vodhanel. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "DazToUnrealEnums.h"



class FDazToUnrealTextures
{
public:
	static FString GetSubSurfaceAlphaTexture(const DazCharacterType CharacterType, const FString& MaterialName);
};