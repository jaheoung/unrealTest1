// Fill out your copyright notice in the Description page of Project Settings.


#include "ImageLoader.h"

#include "Engine/AssetManager.h"
#include "Engine/LightMapTexture2D.h"

UImageLoader::UImageLoader()
{
	// imageWrapperModule = Cast<IImageWrapperModule>(FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("imageWrapper")));
}

UTexture2D* UImageLoader::SyncLoad()
{
	FSoftObjectPath Path = FString(TEXT("Texture2D'/Game/MyResource/Images/gold.gold'"));
	return UAssetManager::GetStreamableManager().LoadSynchronous<UTexture2D>(Path,false,nullptr);
	
}

TArray<UTexture2D*> UImageLoader::SyncGroupLoad()
{
	TArray<FSoftObjectPath> Paths;
	Paths.AddUnique(FString(TEXT("Texture2D'/Game/MyResource/Images/gold.gold'")));
	Paths.AddUnique(FString(TEXT("Texture2D'/Game/MyResource/Images/minimap.minimap'")));
	//Note: before the resource is loaded, the code will pause in this line to wait for the resource to load.
	TSharedPtr<FStreamableHandle> SyncStreamableHandle = UAssetManager::GetStreamableManager().RequestSyncLoad(Paths);
	TArray<UTexture2D*> result;
	if (SyncStreamableHandle)
	{
		TArray<UObject *>LoadedAssets;
		SyncStreamableHandle->GetLoadedAssets(LoadedAssets);

		for (auto elem : LoadedAssets)
		{
			result.Emplace(Cast<UTexture2D>(elem));
		}
	}

	return result;
}

UTexture2D * UImageLoader::ASyncLoad()
{
	FSoftObjectPath SmSyncGroupTexturePath = TEXT("Texture2D'/Game/MyResource/Images/gold.gold'");
	TSharedPtr<FStreamableHandle> OneHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(SmSyncGroupTexturePath);
	if (OneHandle)
	{
		return Cast<UTexture2D>(OneHandle->GetLoadedAsset());
	}
	return nullptr;
}

void UImageLoader::ASyncGroupLoad()
{
	TArray<FSoftObjectPath> Paths;
	Paths.AddUnique(FString(TEXT("Texture2D'/Game/MyResource/Images/gold.gold'")));
	Paths.AddUnique(FString(TEXT("Texture2D'/Game/MyResource/Images/minimap.minimap'")));

	asyncGroupHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(Paths, FStreamableDelegate::CreateUObject(this, &UImageLoader::ASyncGroupLoadCompleate));
}

void UImageLoader::ASyncGroupLoadCompleate()
{
	if (asyncGroupHandle != nullptr && asyncGroupHandle.IsValid())
	{
		TArray<UObject*>LoadedAssets;
		asyncGroupHandle->GetLoadedAssets(LoadedAssets);
		TArray<UTexture2D*> result;
		for (auto elem : LoadedAssets)
		{
			result.Add(Cast<UTexture2D>(elem));
		}
		asyncGroupLoadCallback.ExecuteIfBound(result);
	}
}




