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
	// 맵 가로 사이즈 (정사각형)
	int toolMapSize = -1;
	// 이 간격으로 그리드를 쪼갠다. (맵 가로 사이즈의 약수만 가능)
	int toolMapGap = 10;;
	bool isChangedData = false;
	class AToolMeshActor* toolMeshActor;
	
private:
	void OnMouseUp();
	void OnMouseDownAxis(float amount);

protected:
	virtual void SetupInputComponent() override;
};
