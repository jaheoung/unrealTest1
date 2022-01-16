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
	// ���� toolMapGap ��ŭ ����� �� ���� ������ (���簢��)
	int toolCompressionMapSize = -1;
	// �� �������� �׸��带 �ɰ���. (�� ���� �������� ����� ����)
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
