// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class WEAPON_TYPE : uint8
{
	NONE = 0,
	SINGLE_SWORD = 1,
	SINGLE_GUN = 2,
};

/**
 * 
 */
UCLASS()
class FIRSTGAME_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	AWeapon();
	void SetWeapon(WEAPON_TYPE _type);
	WEAPON_TYPE GetWeaponType();

private:
	UStaticMeshComponent* myMesh;
	WEAPON_TYPE type;
};
