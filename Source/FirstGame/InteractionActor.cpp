// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionActor.h"

#include "FirstGameGameModeBase.h"
#include "Components/WidgetComponent.h"
#include "InteractionWidget.h"
#include "MyUnit.h"
#include "MyWidgetComponent.h"
#include "Components/WidgetInteractionComponent.h"

// Sets default values
AInteractionActor::AInteractionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UStaticMeshComponent* myMesh = CreateDefaultSubobject<UStaticMeshComponent>("myMesh");
	
	myMesh->SetWorldScale3D(FVector(0.5,0.5,0.5));

	RootComponent = myMesh;

	auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(
		TEXT("StaticMesh'/Game/MyResource/Box/Box.Box'"));

	if (MeshAsset.Object != nullptr)
	{
		myMesh->SetStaticMesh(MeshAsset.Object);
	}

	interactionWidgetComp = CreateDefaultSubobject<UMyWidgetComponent>(TEXT("myWidgetComponent"));
	interactionWidgetComp->SetupAttachment(RootComponent, GetAttachParentSocketName());
	// interactionWidgetComp->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), GetAttachParentSocketName());

	auto interactionActorAsset = ConstructorHelpers::FClassFinder<UInteractionWidget>(TEXT("WidgetBlueprint'/Game/MyResource/BP_Interaction.BP_Interaction_C'"));
	if (interactionActorAsset.Succeeded())
	{
		interactionWidgetComp->SetWidgetClass(interactionActorAsset.Class);
		interactionWidgetComp->SetRelativeLocation(FVector(0, 0, 200)); // 상대적 소켓 위치로 해야할듯.
		interactionWidgetComp->SetDrawSize(FVector2D(200, 200));
	}
}

void AInteractionActor::ClearData()
{
	
}

// Called when the game starts or when spawned
void AInteractionActor::BeginPlay()
{
	Super::BeginPlay();

	UInteractionWidget* widget = Cast<UInteractionWidget>(interactionWidgetComp->GetWidget());

	if (widget != nullptr)
		widget->interactionBtn->OnClicked.AddDynamic(this, &AInteractionActor::InteractionButtonClick);
}

// Called every frame
void AInteractionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UWorld* world = GetWorld();
	
	UMyBaseWidget::LookAtWidgetComponent(world, interactionWidgetComp);

	AFirstGameGameModeBase* gameMode = world->GetAuthGameMode<AFirstGameGameModeBase>();

	if (gameMode == nullptr || gameMode->myUnit == nullptr)
		return;

	FVector pcPos = gameMode->myUnit->GetActorLocation();
	FVector interactionPos = GetActorLocation();
	pcPos.Z = interactionPos.Z = 0;
	FVector dis = pcPos - interactionPos;
	interactionWidgetComp->SetVisibility((pcPos - interactionPos).SizeSquared() < (200 * 200));
}

void AInteractionActor::InteractionButtonClick()
{
	AFirstGameGameModeBase* gameMode = GetWorld()->GetAuthGameMode<AFirstGameGameModeBase>();

	if (gameMode != nullptr && gameMode->serverActor != nullptr)
	{
		TSharedPtr<FAskInteractionPacket> packet = gameMode->serverActor->CreateAskPacket<FAskInteractionPacket>(PACKET_TYPE::INTERACTION);
			
		if (packet != nullptr && packet.IsValid())
		{
			packet->ClearData();
			packet->uniqId = uniqId;
			gameMode->serverActor->RegAskPacket(packet);
		}
	}
}

