// Copyright Epic Games, Inc. All Rights Reserved.

#include "FirstGameGameModeBase.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "MyBaseWidget.h"
#include "ServerActor.h"
#include "ToolCameraActor.h"
#include "DrawDebugHelpers.h"
#include "hpBarWidget.h"
#include "HUDWidget.h"
#include "InteractionActor.h"
#include "InventoryWidget.h"
#include "MainWidget.h"
#include "MyCamActor.h"
#include "MyUnit.h"
#include "MyNpc.h"
#include "MyPlayerController.h"
#include "OptionWidget.h"
#include "Weapon.h"
#include "ServerActor.h"
#include "ToolMeshActor.h"
#include "Blueprint/WidgetTree.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "MatineeCameraShake.h"

AFirstGameGameModeBase::AFirstGameGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	auto camAsset = ConstructorHelpers::FClassFinder<AMyCamActor>(TEXT("Blueprint'/Game/MyResource/BP_MyCamActor.BP_MyCamActor_C'"));

	if (camAsset.Succeeded())
		characterCamClass = camAsset.Class;

	auto mainWidgetAsset = ConstructorHelpers::FClassFinder<UMainWidget>(TEXT("WidgetBlueprint'/Game/MyResource/BP_MainWidget.BP_MainWidget_C'"));

	if (mainWidgetAsset.Succeeded())
		mainWidgetClass = mainWidgetAsset.Class;

	// AddUIWidgetClass(WIDGET_TYPE::HUD, TEXT("WidgetBlueprint'/Game/MyResource/BP_HUD.BP_HUD_C'"));
	// AddUIWidgetClass(WIDGET_TYPE::INVENTORY, TEXT("WidgetBlueprint'/Game/MyResource/BP_Inventory.BP_Inventory_C'"));
	// AddUIWidgetClass(WIDGET_TYPE::OPTION, TEXT("WidgetBlueprint'/Game/MyResource/BP_Option.BP_Option_C'"));
}

void AFirstGameGameModeBase::AddUIWidgetClass(WIDGET_TYPE type, const TCHAR* path)
{
	auto asset = ConstructorHelpers::FClassFinder<UUserWidget>(path);

	if (asset.Succeeded())
		uiWidgetClassMap.Add(type, asset.Class);
}


void AFirstGameGameModeBase::StartPlay()
{
	Super::StartPlay();

	// 런타임 로드. // 항상 로드되어야 하는거면. ConstructorHelpers 로 로드가 이점이 있을까? 아님 동적이 이점이 있을까..
	TArray<FSoftObjectPath> uiPaths;
	uiPaths.AddUnique(FString(TEXT("WidgetBlueprint'/Game/MyResource/BP_HUD.BP_HUD_C'")));
	uiPaths.AddUnique(FString(TEXT("WidgetBlueprint'/Game/MyResource/BP_Inventory.BP_Inventory_C'")));
	uiPaths.AddUnique(FString(TEXT("WidgetBlueprint'/Game/MyResource/BP_Option.BP_Option_C'")));

	TSharedPtr<FStreamableHandle> handle = UAssetManager::GetStreamableManager().RequestSyncLoad(uiPaths);
	if (handle != nullptr && handle.IsValid())
	{
		TArray<UObject*> assets;
		handle->GetLoadedAssets(assets);

		for (auto elem : assets)
		{
			UClass* getClass = Cast<UClass>(elem); // 블루프린트 클래스.

			if (getClass->IsChildOf(UHUDWidget::StaticClass()))
				uiWidgetClassMap.Add(WIDGET_TYPE::HUD, getClass);
			else if (getClass->IsChildOf(UInventoryWidget::StaticClass()))
				uiWidgetClassMap.Add(WIDGET_TYPE::INVENTORY, getClass);
			else if (getClass->IsChildOf(UOptionWidget::StaticClass()))
				uiWidgetClassMap.Add(WIDGET_TYPE::OPTION, getClass);
		}
	}

	UWorld* world = GetWorld();

	if (world == nullptr)
		return;
	
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(world->GetFirstPlayerController());

	if (PlayerController != nullptr)
	{
		PlayerController->bAutoManageActiveCameraTarget = false;
		PlayerController->bShowMouseCursor = true;
		PlayerController->bEnableClickEvents = true;
	}

	// height map 로드.
	ReadHeightMap();
	
	// 툴이 실행중인지 확인.
	ReadToolSetting();

	if (isOnMapTool == true) // 맵툴이 실행되어 있다면.
	{
		PlayerController->isMapToolMode = true;
		PlayerController->compressionMapWidth = mapWidth;

		toolCam = world->SpawnActor<AToolCameraActor>();
		toolCam->Rename(TEXT("Tool_Camera"));

		PlayerController->SetViewTarget(toolCam);
		toolCam->SetView();
		toolCam->EnableInput(PlayerController);

		EnableInput(PlayerController);
		InputComponent->BindAction("WheelUp", IE_Pressed, toolCam, &AToolCameraActor::ZoomIn);
		InputComponent->BindAction("WheelDown", IE_Pressed, toolCam, &AToolCameraActor::ZoomOut);
		InputComponent->BindAxis("Forward", toolCam, &AToolCameraActor::Forward);
		InputComponent->BindAxis("Side", toolCam, &AToolCameraActor::Side);
		
		return;
	}

	characterCam = Cast<AMyCamActor>(world->SpawnActor(characterCamClass));

	if (characterCam != nullptr)
	{
		PlayerController->SetViewTarget(characterCam);
		characterCam->EnableInput(PlayerController);
	}
	
	serverActor = world->SpawnActor<AServerActor>();

	if (serverActor != nullptr)
	{
		serverActor->ansDelegate.BindUObject(this, &AFirstGameGameModeBase::AnsCallback);
	}

	// 메인 위젯 붙임.
	UUserWidget* newMainWidget = CreateWidget(world, mainWidgetClass);

	if (newMainWidget != nullptr)
	{
		mainWidget = Cast<UMainWidget>(newMainWidget);
		mainWidget->AddToViewport();
	}
	
	OpenWidget(WIDGET_TYPE::HUD);

	serverActor->StartServer();


	// test
	// int size = serverActor->astar->m_grid.size();
	// int w = FMath::Sqrt(size);
	// for (int i = 0; i < size; ++i)
	// {
	// 	if (serverActor->astar->m_grid[i])
	// 	{
	// 		int x = (i / w) * serverActor->astar->compressionSize;
	// 		int y = (i % w) * serverActor->astar->compressionSize;
	// 		AInteractionActor* actor = world->SpawnActor<AInteractionActor>();
	// 		actor->SetActorLocation(FVector(x, y, 350));
	// 	}
	// }
	
}

void AFirstGameGameModeBase::ReadToolSetting()
{
	FString path = FPaths::ProjectDir();
	path.Append(TEXT("toolSetting.bin"));
	std::ifstream readFile(*path, std::ios::in | std::ios::binary);
	
	if (readFile.is_open())
	{
		readFile.read((char*)&isOnMapTool, sizeof(bool)); // 첫번째는 맵툴 on off 정보.
		readFile.close();
	}
}


void AFirstGameGameModeBase::ReadHeightMap()
{
	heightMapdatas.Empty();

	FString hpath = FPaths::ProjectContentDir();
	hpath.Append(TEXT("CustomFiles/heightMap.bin"));
	std::ifstream readFile(*hpath, std::ios::in | std::ios::binary);

	if (readFile.is_open())
	{
		readFile.read((char*)&mapWidth, sizeof(int));

		readFile.seekg(0, readFile.end);
		size_t fileSize = readFile.tellg();
		fileSize -= sizeof(int);
		readFile.seekg(sizeof(int));
	
		size_t arrSize = fileSize / sizeof(float);
		float* buf = new float[arrSize];

		// float 단위 총 읽을 갯수로 array 를 생성하고
		// file 에서는 실제 사이즈 만큼 read 해야한다.
	
		readFile.read((char *) buf, fileSize);
	
		heightMapdatas.Append(buf, arrSize);

		delete[] buf;

		readFile.close();
	}
}


float AFirstGameGameModeBase::GetHeight(float& x, float& y)
{
	if (heightMapdatas.Num() == 0)
		return 0;
	
	if (heightMapWidth == 0)
		heightMapWidth = FMath::Sqrt(heightMapdatas.Num());

	if (x < 0 || x >= mapWidth || y < 0 || y >= mapWidth)
		return 0;

	if (heightMapRate <= 0)
		heightMapRate = heightMapWidth / (float)mapWidth;

	int&&  index = heightMapWidth * (int)(heightMapRate * x) + (int)(heightMapRate * y); // x 가 top view 기준 위쪽이라.. 헷깔림.
	// UE_LOG(LogTemp, Warning, TEXT("w : %d, x : %d, y : %d, rate : %f         getHeight : %f"), heightMapWidth, (int)(heightMapRate * x), (int)(heightMapRate * y), heightMapRate, heightMapdatas[index]);
	
	return heightMapdatas[index]; 
}


void AFirstGameGameModeBase::OpenWidget(WIDGET_TYPE type)
{
	if (mainWidget == nullptr)
		return;
	
	UUserWidget** getWidget = uiWidgetMap.Find(type);

	if (getWidget == nullptr)
	{
		UUserWidget* newWidget = CreateWidget(GetWorld(), *uiWidgetClassMap[type]);

		if (newWidget != nullptr)
		{
			UMyBaseWidget* getBaseWidget = Cast<UMyBaseWidget>(newWidget);

			if (getBaseWidget != nullptr)
			{
				getBaseWidget->widgetType = type;
				getBaseWidget->Open();
			}
			
			uiWidgetMap.Emplace(type, newWidget);
			mainWidget->AddChild(newWidget);
		}
	}
	else
	{
		(*getWidget)->SetVisibility(ESlateVisibility::Visible);
		
		UMyBaseWidget* getBaseWidget = Cast<UMyBaseWidget>(*getWidget);

		if (getBaseWidget != nullptr)
		{
			getBaseWidget->Open();
			mainWidget->TopWidget(getBaseWidget);
		}
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

void AFirstGameGameModeBase::JoyUpDown(float amount)
{
	if (amount == 0.0)
		return;
	
	if (characterCam == nullptr)
		return;

	characterCam->SetPosOffset(true, amount * 0.01);
}

void AFirstGameGameModeBase::JoyLeftRight(float amount)
{
	if (amount == 0.0)
		return;
	
	if (characterCam == nullptr)
		return;

	characterCam->SetPosOffset(false, amount * 0.01);
}



void AFirstGameGameModeBase::TestButton()
{
	// 코드로 실패...
	// UMatineeCameraShake* shake = NewObject<UMatineeCameraShake>();
	// UMyMatineeCameraShake* shake = NewObject<UMyMatineeCameraShake>();
	// shake->OscillationDuration = 0.25f;
	// shake->OscillationBlendInTime = 0.5;
	// shake->OscillationBlendOutTime = 0.5;
	// shake->RotOscillation.Pitch.Amplitude = FMath::RandRange(5.0f, 10.0f);
	// shake->RotOscillation.Pitch.Frequency = FMath::RandRange(25.0f, 30.0f);
	// shake->StartShake(PlayerController->PlayerCameraManager, 2, ECameraShakePlaySpace::World);

	
	// myUnit->SetMove(FVector(FMath::FRandRange(-500, 500), FMath::FRandRange(-500, 500), 0));
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

void AFirstGameGameModeBase::CamShake()
{
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	PlayerController->ClientStartCameraShake(shake, 1);
}


void AFirstGameGameModeBase::InitPlayerController()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	if (PlayerController != nullptr)
	{
		EnableInput(PlayerController);
		
		InputComponent->BindAxis("Forward", myUnit, &AMyUnit::Forward);
		InputComponent->BindAxis("Side", myUnit, &AMyUnit::Side);
		InputComponent->BindAction("WheelUp", IE_Pressed, this, &AFirstGameGameModeBase::ZoomIn);
		InputComponent->BindAction("WheelDown", IE_Pressed, this, &AFirstGameGameModeBase::ZoomOut);
		InputComponent->BindAction("DefaultSkill", IE_Pressed, myUnit, &AMyUnit::DefaultSkill);
		InputComponent->BindAction("EvationSkill", IE_Pressed, myUnit, &AMyUnit::EvationSkill);
		InputComponent->BindAction("TestButton", IE_Pressed, this, &AFirstGameGameModeBase::TestButton);
		InputComponent->BindAxis("JoyUpDown", this, &AFirstGameGameModeBase::JoyUpDown);
		InputComponent->BindAxis("JoyLeftRight", this, &AFirstGameGameModeBase::JoyLeftRight);
	}
}

void AFirstGameGameModeBase::AppearMyPlayer(TSharedPtr<FAnsPCAppearPacket> packet)
{
	if (packet == nullptr || packet.IsValid() == false)
		return;
	
	const FPCInfo& pcInfo = packet->info.pcInfo;
	
	myUnit = GetWorld()->SpawnActor<AMyUnit>();
	
	characterCam->SetChaseTarget(myUnit, FVector(-1, 0, 1), FVector(0, 0, 100), 500);

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
		(*getNpc)->SetActorLocation(FVector(packet->x, packet->y, GetHeight(packet->x, packet->y)));
		(*getNpc)->SetActorRotation(FRotator(0, packet->rot, 0));
		
		if (packet->isMove)
			(*getNpc)->SetAnim(NPC_STATE::RUN);
		else
			(*getNpc)->SetAnim(NPC_STATE::IDLE);
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
		interactionActor->SetActorLocation(FVector(packet->x, packet->y, GetHeight(packet->x, packet->y)));
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

