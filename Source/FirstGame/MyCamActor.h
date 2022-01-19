// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "MyCamActor.generated.h"

UCLASS()
class FIRSTGAME_API AMyCamActor : public ACameraActor
{
	GENERATED_BODY()

private:
	bool hasChaseTarget = false;
	FVector baseCamPosNormalOffset;
	FVector camPosOffset;
	FVector camRotOffset;
	AActor* chaseTarget;
	float zoomDistance;
	
public:	
	// Sets default values for this actor's properties
	AMyCamActor();

	/**
	 * @brief  ī�޶� ���� ����.
	 * @param  unit ���� ��� ����.
	 * @param  posOffset ī�޶�� ���� ������κ��� �� �������� zoomDistance ��ŭ ������ ��ġ�Ѵ�.
	 * @param  rotOffset ���� ����� ��ü�� �� ���� ������ ���� ��ġ�� ������ �� �ִ�.
	 * @param  _zoomDistance ���� ���� ī�޶��� �Ÿ�.
	 */
	void SetChaseTarget(AActor* unit, FVector posOffset, FVector rotOffset, float _zoomDistance);
	void SetZoom(float plusZoom);
	void UpdateZoom();
	void SetPosOffset(const bool& isUp, const float& amount);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
