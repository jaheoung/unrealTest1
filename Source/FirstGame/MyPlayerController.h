// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTGAME_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	bool isMapToolMode = false;
	TArray<bool> mapToolData;
	// 맵 가로 사이즈 (정사각형)
	int toolMapSize = -1;
	// 이 간격으로 그리드를 쪼갠다.
	int toolMapGap = 10;;
	bool changedMapToolData = false;
	
private:
	void OnMouseDown();
	void OnMouseUp();

protected:
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaSeconds) override;
};
