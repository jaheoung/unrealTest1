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
	int compressionMapWidth = -1;
	// �� �������� �׸��带 �ɰ���. (�� ���� �������� ����� ����)
	int toolMapGap = 150;
	bool isChangedData = false;
	// 1 �� �Է��ϸ� ������ x,y �ֺ� 8������ 1 ��ŭ�� �� ��ĥ�ϰų� �����.
	int toolSelectRange = 0;
	class AToolMeshActor* toolMeshActor;

	void InitToolMeshActor(bool needMeshEmpty);
	void SetToolMesh(const float& _x, const float& _y, bool isSelect, const int& range = 0);
	
private:
	void OnMouseUp();
	void LeftClickAxis(float amount);
	void RightClickAxis(float amount);

protected:
	virtual void SetupInputComponent() override;
};
