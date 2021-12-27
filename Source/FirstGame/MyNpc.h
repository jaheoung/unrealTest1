// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "MyNpc.generated.h"

UENUM(BlueprintType)
enum class NPC_STATE : uint8
{
	IDLE = 0,
	RUN = 1,
	ATTACK = 2,
};

/**
 * 
 */
UCLASS()
class FIRSTGAME_API AMyNpc : public AUnit
{
	GENERATED_BODY()
	
public:
	USkeletalMeshComponent* myMesh;
	class UMyNpcAnimInstance* myAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MyNpc")
	class UWidgetComponent* hpBarComp;

	class UhpBarWidget* hpBar;

	void SetAnim(NPC_STATE animState);
	void SetHp(const float& hp, const float& maxHp);

	AMyNpc();

	virtual void Tick(float DeltaTime) override;
	virtual void ClearData() override;
	virtual void DestroyData() override;
};
