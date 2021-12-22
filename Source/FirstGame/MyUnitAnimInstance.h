// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyUnitAnimInstance.generated.h"

enum class UNIT_STATE : uint8;
enum class WEAPON_TYPE : uint8;

/**
 * 
 */
UCLASS()
class FIRSTGAME_API UMyUnitAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UAnimMontage* defaultSkillMontage;
	UAnimMontage* evationSkillMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNIT_STATE myUnitState;

	void ApplyState();

	UMyUnitAnimInstance();
	void EndMontage(UAnimMontage* montage, bool isComplete);
	void SetAnimByWeapon(WEAPON_TYPE weaponType);
};
