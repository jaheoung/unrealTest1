// Fill out your copyright notice in the Description page of Project Settings.

#include "MyUnit.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/PlayerInput.h"
#include "MyUnitAnimInstance.h"
#include "Weapon.h"
#include <Misc/AssertionMacros.h>
#include "FirstGameGameModeBase.h"



AMyUnit::AMyUnit()
{
	myMesh = CreateDefaultSubobject<USkeletalMeshComponent>("mySMesh");

	RootComponent = myMesh;

	auto MeshAsset = ConstructorHelpers::FObjectFinder<USkeletalMesh>(
		TEXT("SkeletalMesh'/Game/MyResource/MyUnit/myUnit_edit.myUnit_edit'"));

	if (MeshAsset.Object != nullptr)
	{
		myMesh->SetSkeletalMesh(MeshAsset.Object);
	}

	auto getAnim = ConstructorHelpers::FClassFinder<UAnimInstance>(
		TEXT("AnimBlueprint'/Game/MyResource/BP_MyUnitAnim_base.BP_MyUnitAnim_base_C'"));

	animMap.Add((int)(WEAPON_TYPE::SINGLE_SWORD), getAnim.Class);

	auto getAnim2 = ConstructorHelpers::FClassFinder<UAnimInstance>(
		TEXT("AnimBlueprint'/Game/MyResource/BP_MyUnitAnim_singleGun.BP_MyUnitAnim_singleGun_C'"));

	animMap.Add((int)(WEAPON_TYPE::SINGLE_GUN), getAnim2.Class);
}

void AMyUnit::BeginPlay()
{
	Super::BeginPlay();

	lastInputVector = FVector2D(1, 0);
}

void AMyUnit::Equip(WEAPON_TYPE weaponType)
{
	if (unitState != UNIT_STATE::IDLE)
		return;

	currentWeapon = weaponType;

	if (rightWeapon != nullptr)
		GetWorld()->DestroyActor(rightWeapon);

	if (leftWeapon != nullptr)
		GetWorld()->DestroyActor(leftWeapon);

	if (rightHandSocket == nullptr)
		rightHandSocket = myMesh->GetSocketByName("rightHand");

	if (leftHandSocket == nullptr)
		leftHandSocket = myMesh->GetSocketByName("leftHand");

	if (rightHandSocket == nullptr || leftHandSocket == nullptr)
		return;

	switch (weaponType)
	{
	case WEAPON_TYPE::SINGLE_SWORD:
	{
		rightWeapon = GetWorld()->SpawnActor<AWeapon>();
		rightWeapon->SetWeapon(WEAPON_TYPE::SINGLE_SWORD);

		rightHandSocket->AttachActor(rightWeapon, myMesh);
	}
		break;
	case WEAPON_TYPE::SINGLE_GUN:
	{
		rightWeapon = GetWorld()->SpawnActor<AWeapon>();
		rightWeapon->SetWeapon(WEAPON_TYPE::SINGLE_GUN);
		rightHandSocket->AttachActor(rightWeapon, myMesh);

		leftWeapon = GetWorld()->SpawnActor<AWeapon>();
		leftWeapon->SetWeapon(WEAPON_TYPE::SINGLE_GUN);
		leftHandSocket->AttachActor(leftWeapon, myMesh);
	}
		break;
	}
}

void AMyUnit::SetAnim(WEAPON_TYPE weaponType)
{
	if (unitState != UNIT_STATE::IDLE)
		return;

	if (myMesh != nullptr)
	{
		TSubclassOf<UAnimInstance>* getAnim = animMap.Find((int)weaponType);

		if (getAnim != nullptr)
			myMesh->SetAnimInstanceClass(*getAnim);

		// animMap 에 담긴 애니메이션 블루프린트 리소스는 UMyUnitAnimInstance 를 상속받아서 만들어짐.
		myAnim = (UMyUnitAnimInstance*)(myMesh->GetAnimInstance());
	}

	if (myAnim != nullptr)
		myAnim->SetAnimByWeapon(weaponType);
}

void AMyUnit::Forward(float amount)
{
	if (amount != 0.f)
		StopMovePath();

	if (IsUsingSkill())
		return;

	inputVector.X = amount;

	if (IsUsingEvationSkill() == false)
	{
		if (inputVector == FVector2D::ZeroVector)
			SetState(UNIT_STATE::IDLE);
		else
			SetState(UNIT_STATE::RUN);
	}
}

void AMyUnit::Side(float amount)
{
	if (amount != 0.f)
		StopMovePath();

	if (IsUsingSkill())
		return;

	inputVector.Y = amount;

	if (IsUsingEvationSkill() == false)
	{
		if (inputVector == FVector2D::ZeroVector)
			SetState(UNIT_STATE::IDLE);
		else
			SetState(UNIT_STATE::RUN);
	}
}

void AMyUnit::DefaultSkill()
{
	if (IsUsingEvationSkill())
		return;

	SetState(UNIT_STATE::DEFAULT_SKILL);

	// test skill
	AFirstGameGameModeBase* gameMode = GetWorld()->GetAuthGameMode<AFirstGameGameModeBase>();

	if (gameMode != nullptr && gameMode->serverActor != nullptr)
	{
		TSharedPtr<FAskSkillPacket> packet = gameMode->serverActor->CreateAskPacket<FAskSkillPacket>(PACKET_TYPE::SKILL);
			
		if (packet != nullptr && packet.IsValid())
		{
			packet->ClearData();

			FVector myPos = GetActorLocation();
			
			packet->castUnitUniqId = uniqId;
			packet->dirNormal = FVector::ZeroVector; // 지금은 서버에 저장되어 있는걸 그냥씀. 아직 클라에서 이정보를 가지고 있지 않음.
			packet->isPc = true;
			packet->skillType = SKILL_TYPE::PC_DEFAULT_SKILL;
			gameMode->serverActor->RegAskPacket(packet);
		}
	}

	gameMode->CamShake();
}

void AMyUnit::EvationSkill()
{
	// 회피 시작전 한번만 설정.
	if (IsUsingEvationSkill() == false)
	{
		curEvationSkillMoveSpeed = 0;

		// 회피 최대 스피드까지 가속하는 시간 설정.
		UWorld* world = GetWorld();
		if (world != nullptr)
			evationUpSpeedCheckTime = world->GetTimeSeconds() + evationUpSpeedTime;
	}

	SetState(UNIT_STATE::EVATION_SKILL);
}

void AMyUnit::SetState(UNIT_STATE state, bool isUserControll)
{
	if (unitState == state)
		return;

	unitState = state;

	if (myAnim != nullptr)
	{
		myAnim->myUnitState = state;
		myAnim->ApplyState();
	}
}

bool AMyUnit::IsUsingSkill()
{
	return unitState == UNIT_STATE::DEFAULT_SKILL;
}

bool AMyUnit::IsUsingEvationSkill()
{
	return unitState == UNIT_STATE::EVATION_SKILL;
}

WEAPON_TYPE AMyUnit::GetWeaponType()
{
	return currentWeapon;
}

float AMyUnit::GetMoveSpeed()
{
	if (IsUsingEvationSkill())
		return curEvationSkillMoveSpeed;
	else
		return Super::GetMoveSpeed();
}

void AMyUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsUsingEvationSkill())
	{
		if (curEvationSkillMoveSpeed < evationMoveSpeed)
		{
			UWorld* world = GetWorld();

			if (world != nullptr)
			{
				float curTime = world->GetTimeSeconds();
				float passTime = evationUpSpeedTime - (evationUpSpeedCheckTime - curTime);
				// 최대 회피속도까지 가속.
				if (passTime < evationUpSpeedTime)
					curEvationSkillMoveSpeed = FMath::Lerp<float, float>(0.f, evationMoveSpeed, passTime / evationUpSpeedTime);
				else
					curEvationSkillMoveSpeed = evationMoveSpeed;
			}
		}

		// 정지상태라면 마지막 이동 방향으로 회피.
		if (inputVector == FVector2D::ZeroVector)
			inputVector = lastInputVector;
	}

	if (IsMovePath())
	{
		if (lastNextMovePathPos != FVector::ZeroVector)
			SetState(UNIT_STATE::RUN, false);
		else
			SetState(UNIT_STATE::IDLE, false);
	}
}

bool AMyUnit::SetLastPos(float& x, float& y)
{
	if (Super::SetLastPos(x, y))
	{
		AFirstGameGameModeBase* gameMode = GetWorld()->GetAuthGameMode<AFirstGameGameModeBase>();

		if (gameMode != nullptr && gameMode->serverActor != nullptr)
		{
			TSharedPtr<FAskPCMovePacket> packet = gameMode->serverActor->CreateAskPacket<FAskPCMovePacket>(PACKET_TYPE::PC_MOVE);
			
			if (packet != nullptr && packet.IsValid())
			{
				packet->ClearData();
				packet->uniqId = uniqId;
				packet->x = x;
				packet->y = y;
				gameMode->serverActor->RegAskPacket(packet);
			}
		}
		
		return  true;
	}
	
	return false;
}

