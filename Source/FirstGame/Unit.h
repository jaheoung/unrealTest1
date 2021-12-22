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
	// 이 객체를 삭제 시 불러준다.
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
	/// <summary>true 이면 다음틱에 inputValue 에 대한 회전을 하지 않는다.</summary>
	bool notNextTickInputRot;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	FVector GetNextMovePathPos();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
