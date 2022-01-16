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
	// 툴용 toolMapGap 만큼 압축된 맵 가로 사이즈 (정사각형)
	int toolCompressionMapSize = -1;
	// 이 간격으로 그리드를 쪼갠다. (맵 가로 사이즈의 약수만 가능)
	int toolMapGap = 100;
	bool isChangedData = false;
	class AToolMeshActor* toolMeshActor;

	void InitToolMeshActor(bool needMeshEmpty);
	void SetToolMesh(const float& _x, const float& _y, bool&& isSelect);
	
private:
	void OnMouseUp();
	void LeftClickAxis(float amount);
	void RightClickAxis(float amount);

protected:
	virtual void SetupInputComponent() override;
};
