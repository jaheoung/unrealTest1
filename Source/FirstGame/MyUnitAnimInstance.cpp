// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUnitAnimInstance.h"
#include "MyFinishAnimNotify.h"
#include <UObject/UObjectBase.h>
#include "FirstGameGameModeBase.h"
#include "Weapon.h"
#include "MyUnit.h"

void UMyUnitAnimInstance::ApplyState()
{
	// �ѹ��� �÷��� �ϴ°͵��� ��Ÿ�ַ�...
	// ���̵��̳� �� ���� �ִϸ��̼��� ���� ������ �ʿ� ��� ������Ʈ �ӽ����� �ϰ�
	// ��ų���� �ִϸ��̼� ��� �ʿ��Ұ� ������ ��Ÿ�ַ� �÷����ߴ�.
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