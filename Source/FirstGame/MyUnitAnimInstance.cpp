// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUnitAnimInstance.h"
#include "MyFinishAnimNotify.h"
#include <UObject/UObjectBase.h>
#include "FirstGameGameModeBase.h"
#include "Weapon.h"
#include "MyUnit.h"

void UMyUnitAnimInstance::ApplyState()
{
	// 한번만 플레이 하는것들은 몽타주로...
	// 아이들이나 런 같은 애니메이션은 별도 조작이 필요 없어서 스테이트 머신으로 하고
	// 스킬같이 애니메이션 제어가 필요할것 같은건 몽타주로 플레이했다.
	switch (myUnitState)
	{
	case UNIT_STATE::DEFAULT_SKILL:
	{
		Montage_Play(defaultSkillMontage);
		FOnMontageEnded endDelegate;
		endDelegate.BindUObject(this, &UMyUnitAnimInstance::EndMontage);
		Montage_SetEndDelegate(endDelegate, defaultSkillMontage);
	}
	break;
	case UNIT_STATE::EVATION_SKILL:
	{
		Montage_Play(evationSkillMontage);
		FOnMontageEnded endDelegate;
		endDelegate.BindUObject(this, &UMyUnitAnimInstance::EndMontage);
		Montage_SetEndDelegate(endDelegate, evationSkillMontage);
	}
	break;
	}
}

UMyUnitAnimInstance::UMyUnitAnimInstance()
{
	/*auto getDefaultMontage = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("AnimMontage'/Game/MyResource/DefaultSkillMontage.DefaultSkillMontage'"));

	if (getDefaultMontage.Succeeded())
	{
		defaultSkillMontage = getDefaultMontage.Object;
	}

	auto getEvationMontage = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("AnimMontage'/Game/MyResource/EvationSkillMontage.EvationSkillMontage'"));

	if (getEvationMontage.Succeeded())
	{
		evationSkillMontage = getEvationMontage.Object;
	}*/
}


void UMyUnitAnimInstance::EndMontage(UAnimMontage* montage, bool isComplete)
{
	AFirstGameGameModeBase* gameMode = Cast<AFirstGameGameModeBase>(GetWorld()->GetAuthGameMode());

	if (gameMode != nullptr && gameMode->myUnit != nullptr)
		gameMode->myUnit->SetState(UNIT_STATE::IDLE);
}

void UMyUnitAnimInstance::SetAnimByWeapon(WEAPON_TYPE weaponType)
{
	FString defaultSkillPath = "";
	FString evationSkillPath = "";

	switch (weaponType)
	{
	case WEAPON_TYPE::SINGLE_SWORD:
		defaultSkillPath = TEXT("AnimMontage'/Game/MyResource/DefaultSkillMontage_singleSword.DefaultSkillMontage_singleSword'");
		evationSkillPath = TEXT("AnimMontage'/Game/MyResource/EvationSkillMontage_singleSword.EvationSkillMontage_singleSword'");
		break;
	case WEAPON_TYPE::SINGLE_GUN:
		defaultSkillPath = TEXT("AnimMontage'/Game/MyResource/DefaultSkillMontage_singleGun.DefaultSkillMontage_singleGun'");
		evationSkillPath = TEXT("AnimMontage'/Game/MyResource/EvationSkillMontage_singleSword.EvationSkillMontage_singleSword'");
		break;
	default:
		return;
	}

	defaultSkillMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *defaultSkillPath));
	evationSkillMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *evationSkillPath));
}