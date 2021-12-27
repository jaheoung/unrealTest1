// Fill out your copyright notice in the Description page of Project Settings.


#include "MyNpc.h"
#include "MyCamActor.h"
#include "Blueprint/UserWidget.h"
#include "hpBarWidget.h"
#include "Components/WidgetComponent.h"
#include "FirstGameGameModeBase.h"
#include "MyNpcAnimInstance.h"

AMyNpc::AMyNpc()
{
	myMesh = CreateDefaultSubobject<USkeletalMeshComponent>("mySMesh");

	RootComponent = myMesh;

	auto MeshAsset = ConstructorHelpers::FObjectFinder<USkeletalMesh>(
		TEXT("SkeletalMesh'/Game/MyResource/MyNpc/myNpc.myNpc'"));

	if (MeshAsset.Object != nullptr)
	{
		myMesh->SetSkeletalMesh(MeshAsset.Object);
	}

	auto getAnim = ConstructorHelpers::FClassFinder<UAnimInstance>(
		TEXT("AnimBlueprint'/Game/MyResource/BP_MyNpcAnim_base.BP_MyNpcAnim_base_C'"));

	if (getAnim.Succeeded())
	{
		myMesh->SetAnimInstanceClass(getAnim.Class);
	}
	
	hpBarComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("myWidgetComponent"));
	hpBarComp->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), GetAttachParentSocketName());

	auto hpBarAsset = ConstructorHelpers::FClassFinder<UhpBarWidget>(TEXT("WidgetBlueprint'/Game/MyResource/BP_HpBar.BP_HpBar_C'"));
	if (hpBarAsset.Succeeded())
	{
		hpBarComp->SetWidgetSpace(EWidgetSpace::World);
		hpBarComp->SetWidgetClass(hpBarAsset.Class);
		hpBarComp->SetWorldLocation(FVector(0, 0, 400)); // ����� ���� ��ġ�� �ؾ��ҵ�.
		hpBarComp->SetDrawSize(FVector2D(200, 200));
	}
	
}

void AMyNpc::SetAnim(NPC_STATE animState)
{
	if (myAnim == nullptr)
	{
		// getAnim �� ��� �ִϸ��̼� �������Ʈ ���ҽ��� UMyNpcAnimInstance �� ��ӹ޾Ƽ� �������.
		myAnim = (UMyNpcAnimInstance*)(myMesh->GetAnimInstance());
	}
	
	if (myAnim != nullptr)
	{
		myAnim->myNpcState = animState;
	}
}

void AMyNpc::SetHp(const float& hp, const float& maxHp)
{
	if (hpBar == nullptr)
		hpBar = Cast<UhpBarWidget>(hpBarComp->GetWidget());
	
	if (hpBar == nullptr)
		return;

	hpBar->SetHp(hp, maxHp);
}


void AMyNpc::ClearData()
{
	Super::ClearData();
}

void AMyNpc::DestroyData()
{
	Super::DestroyData();

	if (hpBarComp != nullptr)
	{
		hpBarComp->DestroyComponent(false);
		hpBarComp = nullptr;
	}

	if (myMesh != nullptr)
		myMesh = nullptr;

	RootComponent = nullptr;
}

void AMyNpc::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (hpBarComp != nullptr)
	{
		AFirstGameGameModeBase* gameMode = GetWorld()->GetAuthGameMode<AFirstGameGameModeBase>();
		FRotator camRot = gameMode->characterCam->GetActorRotation();
		FRotator resultRot = FRotator::ZeroRotator;
		resultRot.Yaw += 180; // npc �� ȸ���Ѹ�ŭ ȸ���Ѱ� �ƴ϶� �׳� ���� �������� 0������;
		resultRot += camRot.GetInverse();
		hpBarComp->SetWorldRotation(resultRot);
	}
}
