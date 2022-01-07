// Fill out your copyright notice in the Description page of Project Settings.


#include "MinimapWidget.h"

#include "FirstGameGameModeBase.h"
#include "MyCamActor.h"
#include "MyNpc.h"
#include "MyUnit.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"

UMinimapWidget::UMinimapWidget()
{
	auto npcIconAsset = ConstructorHelpers::FClassFinder<UNpcIconWidget>(TEXT("WidgetBlueprint'/Game/MyResource/BP_MinimapNpcIcon.BP_MinimapNpcIcon_C'"));

	if (npcIconAsset.Succeeded())
		npcIconWidgetClass = npcIconAsset.Class;
}

void UMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 이미지 동적으로 로드할것.

	
}

void UMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UWorld* world = GetWorld();
	AFirstGameGameModeBase* gameMode = world->GetAuthGameMode<AFirstGameGameModeBase>();

	if (gameMode == nullptr || gameMode->mapSize <= 0.0)
		return;

	if (isInit == false)
	{
		FVector2D getClipSize = clippingPanel->GetCachedGeometry().Size;

		if (getClipSize.X > 0.0)
		{
			halfClippingW = getClipSize.X * 0.5;
			halfClippingH = getClipSize.Y * 0.5;

			imageW = mapImage->GetDesiredSize().X;
			imageH = mapImage->GetDesiredSize().Y;
			imgRate = imageW / gameMode->mapSize;

			FVector2D myIconSize = myUnitIcon->GetDesiredSize();
			myUnitIcon->SetRenderTranslation(FVector2D(halfClippingW - myIconSize.X * 0.5, halfClippingH - myIconSize.Y * 0.5));
			isInit = true;
		}
	}

	FVector myPos = gameMode->myUnit->GetActorLocation();
	mapPos.X = -(myPos.Y * imgRate) + halfClippingW;
	mapPos.Y = (myPos.X * imgRate) - imageH + halfClippingH;
	mapImage->SetRenderTranslation(mapPos);

	FRotator getRot = gameMode->myUnit->GetActorRotation();
	myUnitIcon->SetRenderTransformAngle(getRot.Yaw);

	for (auto elem : npcIconMap)
	{
		UMyBaseWidget* widget = elem.Value;
		if (widget == nullptr)
			continue;
		widget->SetVisibility(ESlateVisibility::Hidden);
	}

	for (auto elem : gameMode->npcMap)
	{
		AMyNpc* npc = elem.Value;

		FVector getPos = npc->GetActorLocation();
		getRot = npc->GetActorRotation();

		UNpcIconWidget** getIcon = npcIconMap.Find(npc->uniqId);
		UNpcIconWidget* getIconWidget = nullptr;
		if (getIcon == nullptr)
		{
			auto newWidget = CreateWidget(world, npcIconWidgetClass);

			if (newWidget != nullptr)
			{
				UNpcIconWidget* createWidget = Cast<UNpcIconWidget>(newWidget);
				npcIconMap.Add(npc->uniqId, createWidget);
				clippingPanel->AddChild(createWidget);
				getIconWidget = createWidget;
			}
		}
		else
		{
			getIconWidget = *getIcon;	
		}

		if (getIconWidget == nullptr)
			continue;

		getIconWidget->SetVisibility(ESlateVisibility::Visible);
		FVector2D size = getIconWidget->GetDesiredSize();
		FVector2D iconPos;
		iconPos.X = mapPos.X + (getPos.Y * imgRate) - (size.X * 0.5);
		iconPos.Y = mapPos.Y + (imageH - (getPos.X * imgRate)) - (size.Y * 0.5);

		getIconWidget->SetRenderTranslation(iconPos);
		// 위젯 자체를 돌리면 크게 회전하는데 그 위젯의 크기가 어디서 설정되는지 못찾음...
		getIconWidget->npcIcon->SetRenderTransformAngle(getRot.Yaw);
		
	}
}
