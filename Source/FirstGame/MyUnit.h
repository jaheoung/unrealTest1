// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "MyUnit.generated.h"

class AWeapon;
enum class UNIT_STATE : uint8;
enum class WEAPON_TYPE : uint8;

UENUM(BlueprintType)
enum class UNIT_STATE : uint8
{
	IDLE = 0,
	RUN = 1,
	DEFAULT_SKILL = 2,
	EVATION_SKILL = 3,
};

/**
 * 
 */
UCLASS()
class FIRSTGAME_API AMyUnit : public AUnit
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FirstGame)
		UNIT_STATE unitState = UNIT_STATE::IDLE;

	USkeletalMeshComponent* myMesh;
	TMap<int, TSubclassOf<UAnimInstance>> animMap;
	class UMyUnitAnimInstance* myAnim;
	
	
	float evationMoveSpeed = 9.f;
	float curEvationSkillMoveSpeed = 0;
	/// <summary>가속하는 시간.</summary>
	float evationUpSpeedTime = 0.6f;
	float evationUpSpeedCheckTime = 0;
	const USkeletalMeshSocket* rightHandSocket;
	const USkeletalMeshSocket* leftHandSocket;
	AWeapon* rightWeapon;
	AWeapon* leftWeapon;

	AMyUnit();

	void Equip(WEAPON_TYPE weaponType);
	void SetAnim(WEAPON_TYPE weaponType);
	void Forward(float amount);
	void Side(float amount);
	void DefaultSkill();
	void EvationSkill();
	void SetState(UNIT_STATE state, bool isUserControll = true);
	bool IsUsingSkill();
	bool IsUsingEvationSkill();
	WEAPON_TYPE GetWeaponType();
	virtual float GetMoveSpeed() override;

	virtual bool SetLastPos(float& x, float& y) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	WEAPON_TYPE currentWeapon;
};
