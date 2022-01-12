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
	// �� ���� ������ (���簢��)
	int toolMapSize = -1;
	// �� �������� �׸��带 �ɰ���. (�� ���� �������� ����� ����)
	int toolMapGap = 10;;
	bool isChangedData = false;
	class AToolMeshActor* toolMeshActor;
	
private:
	void OnMouseUp();
	void OnMouseDownAxis(float amount);

protected:
	virtual void SetupInputComponent() override;
};
