// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Unit.generated.h"


UCLASS()
class FIRSTGAME_API AUnit : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUnit();

	virtual void ClearData();
	// �� ��ü�� ���� �� �ҷ��ش�.
	virtual void DestroyData();

	uint32 uniqId;

	void SetMove(FVector targetPos);
	void StopMovePath();
	virtual float GetMoveSpeed();
	bool IsMovePath();

	float lastX;
	float lastY;
	virtual bool SetLastPos(float& x, float& y);

protected:
	TArray<FVector> myPath;
	int curPathIndex;
	FVector lastTargetPoint;
	float moveSpeed = 3.5f;
	class UNavigationSystemV1* navSys;
	FVector2D inputVector;
	FVector2D lastInputVector;
	FVector lastNextMovePathPos;
	/// <summary>true �̸� ����ƽ�� inputValue �� ���� ȸ���� ���� �ʴ´�.</summary>
	bool notNextTickInputRot;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	FVector GetNextMovePathPos();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
