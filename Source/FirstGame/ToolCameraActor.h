// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "ToolCameraActor.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTGAME_API AToolCameraActor : public ACameraActor
{
	GENERATED_BODY()

public:
	void SetView();

	class UCameraComponent* camComp;
	float zoomDistance;
	
	UFUNCTION()
	void ZoomIn();
	UFUNCTION()
	void ZoomOut();
	UFUNCTION()
	void Forward(float amount);
	UFUNCTION()
	void Side(float amount);
};
