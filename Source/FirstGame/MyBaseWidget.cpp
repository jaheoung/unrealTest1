// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBaseWidget.h"
#include "FirstGameGameModeBase.h"
#include "ToolBuilderUtil.h"
#include "Components/WidgetComponent.h"
#include "MyCamActor.h"

void UMyBaseWidget::Open()
{
	
}


void UMyBaseWidget::Close()
{
}


void UMyBaseWidget::LookAtWidgetComponent(class UWorld* world,UWidgetComponent* comp)
{
	if (world == nullptr || comp == nullptr)
		return;
	
	AFirstGameGameModeBase* gameMode = world->GetAuthGameMode<AFirstGameGameModeBase>();

	if (gameMode == nullptr)
		return;
	
	FRotator camRot = gameMode->characterCam->GetActorRotation();
	FRotator resultRot = FRotator::ZeroRotator;
	resultRot.Yaw += 180; // npc 가 회전한만큼 회전한게 아니라 그냥 월드 기준으로 0도였음;
	resultRot += camRot.GetInverse();
	comp->SetWorldRotation(resultRot);
}
