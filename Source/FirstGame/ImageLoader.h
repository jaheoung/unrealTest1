// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ImageLoader.generated.h"

DECLARE_DELEGATE_OneParam( ASyncGroupCallback, TArray<UTexture2D*>);

/**
 * 
 */
UCLASS()
class FIRSTGAME_API UImageLoader : public UObject
{
	GENERATED_BODY()

public:

	UImageLoader();

	UTexture2D* SyncLoad();
	TArray<UTexture2D*> SyncGroupLoad();
	UTexture2D * ASyncLoad();

	TSharedPtr<struct FStreamableHandle> asyncGroupHandle;
	ASyncGroupCallback asyncGroupLoadCallback;
	void ASyncGroupLoad();
	void ASyncGroupLoadCompleate();
};
