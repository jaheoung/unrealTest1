// Copyright Epic Games, Inc. All Rights Reserved.

#include "FirstGameGameModeBase.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "hpBarWidget.h"
#include "HUDWidget.h"
#include "InteractionActor.h"
#include "InventoryWidget.h"
#include "MyCamActor.h"
#include "MyUnit.h"
#include "MyNpc.h"
#include "Weapon.h"
#include "ServerActor.h"
#include "Commandlets/GatherTextCommandlet.h"
#include "Components/WidgetComponent.h"

AFirstGameGameModeBase::AFirstGameGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	auto hudAsset = ConstructorHelpers::FClassFinder<UUserWidget>(TEXT("WidgetBlueprint'/Game/MyResource/BP_HUD.BP_HUD_C'"));

	if (hudAsset.Succeeded())
		uiWidgetClassMap.Add(WIDGET_TYPE::HUD, hudAsset.Class);
	
	auto interactionAsset = ConstructorHelpers::FClassFinder<UUserWidget>(TEXT("WidgetBlueprint'/Game/MyResource/BP_Inventory.BP_Inventory_C'"));

	if (interactionAsset.Succeeded())
		uiWidgetClassMap.Add(WIDGET_TYPE::INVENTORY, interactionAsset.Class);
}

void AFirstGameGameModeBase::StartPlay()
{
	Super::StartPlay();

	UWorld* world = GetWorld();

	if (world == nullptr)
		return;;
	
	serverActor = world->SpawnActor<AServerActor>();

	if (serverActor != nullptr)
	{
		serverActor->ansDelegate.BindUObject(this, &AFirstGameGameModeBase::AnsCallback);
	}

	characterCam = world->SpawnActor<AMyCamActor>();
	characterCam->EnableInput(GetWorld()->GetFirstPlayerController());

	// hud 붙임.
	OpenWidget(WIDGET_TYPE::HUD);

	// height map 로드.
	ReadHeightMap();

	serverActor->StartServer();
}

void AFirstGameGameModeBase::ReadHeightMap()
{
	heightMapdatas.Empty();

	FString hpath = FPaths::ProjectDir();
	hpath.Append(TEXT("heightMap.bin"));
	std::ifstream readFile(*hpath, std::ios::in | std::ios::binary);

	readFile.read((char*)&heightMapSize, sizeof(int));

	readFile.seekg(0, readFile.end);
	size_t fileSize = readFile.tellg();
	readFile.seekg(0, readFile.beg);
	
	size_t arrSize = fileSize / sizeof(float);
	float* buf = new float[arrSize];

	// float 단위 총 읽을 갯수로 array 를 생성하고
	// file 에서는 실제 사이즈 만큼 read 해야한다.
	
	readFile.read((char *) buf, fileSize);
	
	heightMapdatas.Append(buf, arrSize);

	delete[] buf;

	readFile.close();
}


float AFirstGameGameModeBase::GetHeight(float& x, float& y)
{
	if (heightMapdatas.Num() == 0)
		return 0;
	
	if (heightMapWidth == 0)
		heightMapWidth = FMath::Sqrt(heightMapdatas.Num());

	if (x < 0 || x > heightMapSize || y < 0 || y > heightMapSize)
		return 0;

	if (heightMapRate <= 0)
		heightMapRate = heightMapWidth / (float)heightMapSize;

	int&&  index = heightMapWidth * (int)(heightMapRate * x) + (int)(heightMapRate * y); // x 가 top view 기준 위쪽이라.. 헷깔림.
	// UE_LOG(LogTemp, Warning, TEXT("w : %d, x : %d, y : %d, rate : %f         getHeight : %f"), heightMapWidth, (int)(heightMapRate * x), (int)(heightMapRate * y), heightMapRate, heightMapdatas[index]);
	
	return heightMapdatas[index]; 
}


void AFirstGameGameModeBase::OpenWidget(WIDGET_TYPE type)
{
	UUserWidget** getWidget = uiWidgetMap.Find(type);

	if (getWidget == nullptr)
	{
		auto newWidget = CreateWidget(GetWorld(), *uiWidgetClassMap[type]);

		if (newWidget != nullptr)
		{
			UMyBaseWidget* getBaseWidget = Cast<UMyBaseWidget>(newWidget);

			if (getBaseWidget != nullptr)
			{
				getBaseWidget->widgetType = type;
				getBaseWidget->Open();
			}
			
			uiWidgetMap.Emplace(type, newWidget);
			newWidget->AddToViewport();
		}
	}
	else
	{
		(*getWidget)->SetVisibility(ESlateVisibility::Visible);
		
		UMyBaseWidget* getBaseWidget = Cast<UMyBaseWidget>(*getWidget);

		if (getBaseWidget != nullptr)
			getBaseWidget->Open();
	}
}

void AFirstGameGameModeBase::CloseWidget(WIDGET_TYPE type)
{
	UUserWidget** getWidget = uiWidgetMap.Find(type);

	if (getWidget != nullptr)
	{
		(*getWidget)->SetVisibility(ESlateVisibility::Hidden);
		
		UMyBaseWidget* getBaseWidget = Cast<UMyBaseWidget>(*getWidget);

		if (getBaseWidget != nullptr)
			getBaseWidget->Close();
	}
}



void AFirstGameGameModeBase::Tick(float DeltaSeconds)
{
	if (myUnit != nullptr)
	{
		//test
		FVector pos = myUnit->GetActorLocation();
		pos.Z = GetHeight(pos.X, pos.Y);
		myUnit->SetActorLocation(pos);
	}
}


void AFirstGameGameModeBase::ZoomIn()
{
	if (characterCam == nullptr)
		return;

	characterCam->SetZoom(-50);
	characterCam->UpdateZoom();
}

void AFirstGameGameModeBase::ZoomOut()
{
	if (characterCam == nullptr)
		return;

	characterCam->SetZoom(50);
	characterCam->UpdateZoom();
}

void AFirstGameGameModeBase::TestButton()
{
	myUnit->SetMove(FVector(FMath::FRandRange(-500, 500), FMath::FRandRange(-500, 500), 0));
	/*if (myUnit->GetWeaponType() == WEAPON_TYPE::SINGLE_SWORD)
	{
		myUnit->Equip(WEAPON_TYPE::SINGLE_GUN);
		myUnit->SetAnim(WEAPON_TYPE::SINGLE_GUN);
	}
	else
	{
		myUnit->Equip(WEAPON_TYPE::SINGLE_SWORD);
		myUnit->SetAnim(WEAPON_TYPE::SINGLE_SWORD);
	}*/
}

void AFirstGameGameModeBase::InitPlayerController()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	if (PlayerController != nullptr)
	{
		PlayerController->bAutoManageActiveCameraTarget = false;
		PlayerController->bShowMouseCursor = true;

		if (characterCam != nullptr)
			PlayerController->SetViewTarget(characterCam);

		EnableInput(PlayerController);

		InputComponent->BindAxis("Forward", myUnit, &AMyUnit::Forward);
		InputComponent->BindAxis("Side", myUnit, &AMyUnit::Side);
		InputComponent->BindAction("WheelUp", IE_Pressed, this, &AFirstGameGameModeBase::ZoomIn);
		InputComponent->BindAction("WheelDown", IE_Pressed, this, &AFirstGameGameModeBase::ZoomOut);
		InputComponent->BindAction("DefaultSkill", IE_Pressed, myUnit, &AMyUnit::DefaultSkill);
		InputComponent->BindAction("EvationSkill", IE_Pressed, myUnit, &AMyUnit::EvationSkill);
		InputComponent->BindAction("TestButton", IE_Pressed, this, &AFirstGameGameModeBase::TestButton);

		//UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Jump", EKeys::N, 0, 0, 0, 0));
		//FInputActionKeyMapping jump("Jump", EKeys::SpaceBar, 0, 0, 0, 0);
	}
}

void AFirstGameGameModeBase::AppearMyPlayer(TSharedPtr<FAnsPCAppearPacket> packet)
{
	if (packet == nullptr || packet.IsValid() == false)
		return;
	
	const FPCInfo& pcInfo = packet->info.pcInfo;
	
	myUnit = GetWorld()->SpawnActor<AMyUnit>();
	
	characterCam->SetChaseTarget(myUnit, FVector(350, 0, 500), FVector(0, -50, 0), 300);

	InitPlayerController();

	myUnit->uniqId = pcInfo.uniqId;
	
	myUnit->SetActorScale3D(FVector(myUnitScale, myUnitScale, myUnitScale));
	myUnit->SetActorLocation(FVector(pcInfo.x, pcInfo.y, pcInfo.z));

	myUnit->Equip(pcInfo.weaponType);
	myUnit->SetAnim(pcInfo.weaponType);
	FRotator r = myUnit->GetActorRotation();
	r.Yaw += pcInfo.rot;
	myUnit->SetActorRotation(r);
}

void AFirstGameGameModeBase::AppearNpc(TSharedPtr<FAnsNPCAppearPacket> packet)
{
	if (packet == nullptr || packet.IsValid() == false)
		return;

	const FNPCInfo& npcInfo = packet->info.npcInfo;
	
	AMyNpc* npc = CreateNpc();

	if (npc != nullptr)
	{
		npc->SetActorLocation(FVector(npcInfo.x, npcInfo.y, npcInfo.z));
		npc->SetActorScale3D(FVector(npcInfo.unitScale, npcInfo.unitScale, npcInfo.unitScale));
		FRotator r = npc->GetActorRotation();
		r.Yaw += npcInfo.rot;
		npc->SetActorRotation(r);
		npc->uniqId = npcInfo.uniqId;
		npc->SetHp(npcInfo.hp, npcInfo.maxHp);
		npcMap.Add(npcInfo.uniqId, npc);
	}
}

void AFirstGameGameModeBase::DisappearNpc(TSharedPtr<FAnsNPCDisappearPacket> packet)
{
	if (packet == nullptr || packet.IsValid() == false)
		return;
	
	AMyNpc** getNpc = npcMap.Find(packet->info.uniqId);

	if (getNpc != nullptr)
	{
		RestoreNpc(*getNpc);
		npcMap.Remove(packet->info.uniqId);
	}
}

void AFirstGameGameModeBase::NpcMove(TSharedPtr<FAnsNpcMovePacket> packet)
{
	if (packet == nullptr || packet.IsValid() == false)
		return;
	
	AMyNpc** getNpc = npcMap.Find(packet->uniqId);

	if (getNpc != nullptr)
	{
		(*getNpc)->SetActorLocation(FVector(packet->x, packet->y, packet->z));
		(*getNpc)->SetActorRotation(FRotator(0, packet->rot, 0));
		(*getNpc)->SetAnim(NPC_STATE::RUN);
	}
}

void AFirstGameGameModeBase::UpdateUnitInfo(TSharedPtr<FAnsUpdateUnitInfoPacket> packet)
{
	if (packet == nullptr || packet.IsValid() == false)
		return;

	AMyNpc** getNpc = npcMap.Find(packet->uniqId);

	if (getNpc != nullptr)
	{
		(*getNpc)->SetHp(packet->hp, packet->maxHp);
	}
}

void AFirstGameGameModeBase::AppearInteractionObj(TSharedPtr<FAnsInteractionAppearPacket> packet)
{
	if (packet == nullptr || packet.IsValid() == false)
		return;

	AInteractionActor* interactionActor = CreateInteractionObj();

	if (interactionActor != nullptr)
	{
		interactionActor->ClearData();
		interactionActor->uniqId = packet->uniqId;
		interactionActor->SetActorLocation(FVector(packet->x, packet->y, 0));
		FRotator r = interactionActor->GetActorRotation();
		r.Yaw += packet->rot;
		interactionActor->SetActorRotation(r);
		interactionObjMap.Add(packet->uniqId, interactionActor);
	}
}

void AFirstGameGameModeBase::DisappearInteractionObj(TSharedPtr<FAnsInteractionDisappearPacket> packet)
{
	if (packet == nullptr || packet.IsValid() == false)
		return;

	AInteractionActor** getInteractionObj = interactionObjMap.Find(packet->uniqId);

	if (getInteractionObj != nullptr)
	{
		RestoreInteractionObj(*getInteractionObj);
		npcMap.Remove(packet->uniqId);
	}
}

void AFirstGameGameModeBase::UpdateInventory(TSharedPtr<FAnsUpdateInventoryPacket> packet)
{
	if (packet == nullptr || packet.IsValid() == false)
		return;

	for (auto elem : myItems)
	{
		elem->ClearData();
		myItemPool.Enqueue(elem);
	}
	myItems.Empty();

	for (int i = 0, k = packet->itemCount; i < k; ++i)
	{
		TSharedPtr<FMyItemInfo> item;
		if (myItemPool.IsEmpty())
		{
			item = TSharedPtr<FMyItemInfo>(new FMyItemInfo());
		}
		else
		{
			myItemPool.Dequeue(item);
		}

		if (item == nullptr || item.IsValid() == false)
			continue;

		FMyItemInfo& info = packet->items[i];

		item->uniqId = info.uniqId;
		item->type = info.type;
		item->count = info.count;

		myItems.Emplace(item);
	}

	UInventoryWidget* getWidget = GetWidget<UInventoryWidget>(WIDGET_TYPE::INVENTORY);

	if (getWidget != nullptr && getWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		getWidget->UpdateInventory();
	}
}



AMyNpc* AFirstGameGameModeBase::CreateNpc()
{
	if (npcPool.IsEmpty())
	{
		return GetWorld()->SpawnActor<AMyNpc>();
	}
	else
	{
		AMyNpc* getNpc;

		if (npcPool.Dequeue(getNpc))
		{
			getNpc->SetActorHiddenInGame(false);
			getNpc->ClearData();
			return getNpc;
		}
	}
	return nullptr;
}

void AFirstGameGameModeBase::RestoreNpc(AMyNpc* npc)
{
	npc->SetActorHiddenInGame(true);
	npc->ClearData();
	npcPool.Enqueue(npc);
}

AInteractionActor* AFirstGameGameModeBase::CreateInteractionObj()
{
	if (interactionObjPool.IsEmpty())
	{
		return GetWorld()->SpawnActor<AInteractionActor>();
	}
	else
	{
		AInteractionActor* getInteractionObj;

		if (interactionObjPool.Dequeue(getInteractionObj))
		{
			getInteractionObj->SetActorHiddenInGame(false);
			getInteractionObj->ClearData();
			return getInteractionObj;
		}
	}
	return nullptr;
}

void AFirstGameGameModeBase::RestoreInteractionObj(AInteractionActor* interactionObj)
{
	interactionObj->SetActorHiddenInGame(true);
	interactionObj->ClearData();
	interactionObjPool.Enqueue(interactionObj);
}



void AFirstGameGameModeBase::AnsCallback(const TSharedPtr<FAnsPacket> packet)
{
	if (packet == nullptr || packet.IsValid() == false)
		return;

	if (packet->ret == PACKET_RET::FAIL)
		return;
	
	switch (packet->GetPacketType())
	{
	case PACKET_TYPE::SKILL:
		{
			
		}
		break;
	case PACKET_TYPE::PC_APPEAR:
		{
			AppearMyPlayer(StaticCastSharedPtr<FAnsPCAppearPacket>(packet));
		}
		break;
	case PACKET_TYPE::PC_DISAPPEAR:
		{
			
		}
		break;
	case PACKET_TYPE::NPC_APPEAR:
		{
			AppearNpc(StaticCastSharedPtr<FAnsNPCAppearPacket>(packet));
		}
		break;
	case PACKET_TYPE::NPC_DISAPPEAR:
		{
			DisappearNpc(StaticCastSharedPtr<FAnsNPCDisappearPacket>(packet));
		}
		break;
	case PACKET_TYPE::NPC_MOVE:
		{
			NpcMove(StaticCastSharedPtr<FAnsNpcMovePacket>(packet));
		}
		break;
	case PACKET_TYPE::UPDATE_UNIT_INFO:
		{
			UpdateUnitInfo(StaticCastSharedPtr<FAnsUpdateUnitInfoPacket>(packet));
		}
		break;
	case PACKET_TYPE::INTERACTION_APPEAR:
		{
			AppearInteractionObj(StaticCastSharedPtr<FAnsInteractionAppearPacket>(packet));
		}
		break;
	case PACKET_TYPE::INTERACTION_DISAPPEAR:
		{
			DisappearInteractionObj(StaticCastSharedPtr<FAnsInteractionDisappearPacket>(packet));
		}
		break;
	case PACKET_TYPE::UPDATE_INVENTORY:
		{
			UpdateInventory(StaticCastSharedPtr<FAnsUpdateInventoryPacket>(packet));
		}
		break;
	}
}

