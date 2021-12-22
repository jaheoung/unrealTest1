// Copyright Epic Games, Inc. All Rights Reserved.

#include "FirstGameGameModeBase.h"
#include "MyCamActor.h"
#include "MyUnit.h"
#include "MyNpc.h"
#include "Weapon.h"
#include "ServerActor.h"
#include "Components/WidgetComponent.h"

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

	// SpawnMyPlayer();

	/*for (int i = 0; i < 30; ++i)
	{
		int x = FMath::RandRange(-1000, 1000);
		int y = FMath::RandRange(-1000, 1000);
		float dirAngle = FMath::RandRange(0, 360);
		SpawnNpc((uint32)i, x, y, 0.19, dirAngle);	
	}*/
}

void AFirstGameGameModeBase::Tick(float DeltaSeconds)
{
	
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
	myUnit->SetActorLocation(FVector(pcInfo.x, pcInfo.y, 0));

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
		npc->SetActorLocation(FVector(npcInfo.x, npcInfo.y, 0));
		npc->SetActorScale3D(FVector(npcInfo.unitScale, npcInfo.unitScale, npcInfo.unitScale));
		FRotator r = npc->GetActorRotation();
		r.Yaw += npcInfo.rot;
		npc->SetActorRotation(r);
		npc->uniqId = npcInfo.uniqId;
		npcMap.Add(npcInfo.uniqId, npc);
	}
}

void AFirstGameGameModeBase::DisappearNpc(TSharedPtr<FAnsNPCDisappearPacket> packet)
{
	if (packet == nullptr || packet.IsValid() == false)
		return;
	
	// 나중에 재사용으로 바꿔야함!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	AMyNpc** getNpc = npcMap.Find(packet->info.uniqId);

	if (getNpc != nullptr)
	{
		RestoreNpc(*getNpc);
		// (*getNpc)->DestroyData();
		// GetWorld()->DestroyActor(*getNpc);
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
		(*getNpc)->SetActorLocation(FVector(packet->x, packet->y, 0));
		(*getNpc)->SetActorRotation(FRotator(0, packet->rot, 0));
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
	}
}

