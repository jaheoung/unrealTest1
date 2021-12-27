// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerActor.h"

// Sets default values
AServerActor::AServerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AServerActor::BeginPlay()
{
	Super::BeginPlay();

	myUnit = CreatePC();

	if (myUnit != nullptr && myUnit.IsValid())
	{
		myUnit->ClearData();
		myUnit->uniqId = GetUniqueID();
		myUnit->SetPos(0, 0, 0);
		myUnit->rot = 0;
		myUnit->weaponType = WEAPON_TYPE::SINGLE_SWORD;
		myUnit->unitScale = 0.3f;
		myUnit->unitType = 1;
		pcMap.Emplace(myUnit->uniqId, myUnit);
	}

	for (int i = 0; i < 100; ++i)
	{
		TSharedPtr<FNPCInfo> getNpc = CreateNPC();

		if (getNpc == nullptr || getNpc.IsValid() == false)
			continue;

		getNpc->ClearData();
		getNpc->uniqId = GetUnitUniqId();
		getNpc->SetPos(FMath::RandRange(-2000, 2000), FMath::RandRange(-2000, 2000), 0);
		getNpc->rot = FMath::RandRange(0, 360);
		getNpc->unitScale = 0.3;
		getNpc->hp = 100;
		getNpc->maxHp = 300;
		getNpc->isDie = false;
		npcMap.Emplace(getNpc->uniqId, getNpc);
	}
	
	// test spawn... 나중엔 pcmap 구현하고 복사본을 넘겨야 할까?????????????????????????????????
	// TSharedPtr<FAnsPCAppearPacket> pcSpawnPacket = CreateAnsPacket<FAnsPCAppearPacket>(PACKET_TYPE::PC_APPEAR);
	//
	// if (pcSpawnPacket != nullptr && pcSpawnPacket.IsValid())
	// {
	// 	pcSpawnPacket->ret = PACKET_RET::SUCCESS;
	// 	pcSpawnPacket->ClearData();
	// 	
	// 	FPCInfo& pcInfo = pcSpawnPacket->info.pcInfo;
	//
	// 	pcInfo.ClearData();
	// 	pcInfo.uniqId = GetUniqueID();
	// 	pcInfo.x = 0;
	// 	pcInfo.y = 0;
	// 	pcInfo.dir = 0;
	// 	pcInfo.weaponType = WEAPON_TYPE::SINGLE_SWORD;
	// 	pcInfo.unitScale = 0.3;
	// 	pcInfo.unitType = 1;
	// 	RegAnsPacket(pcSpawnPacket);
	// }
	//
	//
	// for (int i = 0; i < 30; ++i)
	// {
	// 	TSharedPtr<FAnsNPCAppearPacket> npcSpawnPacket = CreateAnsPacket<FAnsNPCAppearPacket>(PACKET_TYPE::NPC_APPEAR);
	//
	// 	if (npcSpawnPacket != nullptr && npcSpawnPacket.IsValid())
	// 	{
	// 		npcSpawnPacket->ClearData();
	// 		FNPCInfo& npcInfo = npcSpawnPacket->info.npcInfo;
	//
	// 		npcInfo.ClearData();
	// 		npcInfo.uniqId = GetUnitUniqId();
	// 		npcInfo.x = FMath::RandRange(-1000, 1000);
	// 		npcInfo.y = FMath::RandRange(-1000, 1000);
	// 		npcInfo.dir = FMath::RandRange(0, 360);
	// 		npcInfo.unitScale = 0.19;
	// 		RegAnsPacket(npcSpawnPacket);
	// 	}
	// }
}

// template<typename T>
// TSharedPtr<T> AServerActor::CreateAskPacket(PACKET_TYPE type)
// {
// 	TArray<TSharedPtr<FAskPacket>>* getArray = askPacketPool.Find(type);
//
// 	TSharedPtr<FAskPacket> getValue = nullptr;
// 	
// 	if (getArray == nullptr || getArray->Num() == 0)
// 	{
// 		switch (type)
// 		{
// 		case PACKET_TYPE::SKILL: getValue = TSharedPtr<FAskSkillPacket>(new FAskSkillPacket()); break;
// 		case PACKET_TYPE::PC_MOVE: getValue = TSharedPtr<FAskPCMovePacket>(new FAskPCMovePacket()); break;
// 		}
//
// 		if (getValue != nullptr)
// 			return StaticCastSharedPtr<T>(getValue);
// 	}
// 	else
// 	{
// 		getValue = getArray->Pop();
// 		if (getValue != nullptr && getValue.IsValid())
// 			return StaticCastSharedPtr<T>(getValue);
// 	}
//
// 	return nullptr;
// }

template<typename T>
TSharedPtr<T> AServerActor::CreateAnsPacket(PACKET_TYPE type)
{
	TArray<TSharedPtr<FAnsPacket>>* getArray = ansPacketPool.Find(type);

	TSharedPtr<FAnsPacket> getValue = nullptr;
	
	if (getArray == nullptr || getArray->Num() == 0)
	{
		switch (type)
		{
		case PACKET_TYPE::SKILL: getValue = TSharedPtr<FAnsSkillPacket>(new FAnsSkillPacket()); break;
		case PACKET_TYPE::PC_APPEAR: getValue = TSharedPtr<FAnsPCAppearPacket>(new FAnsPCAppearPacket()); break;
		case PACKET_TYPE::PC_DISAPPEAR: getValue = TSharedPtr<FAnsPCDisappearPacket>(new FAnsPCDisappearPacket()); break;
		case PACKET_TYPE::NPC_APPEAR: getValue = TSharedPtr<FAnsNPCAppearPacket>(new FAnsNPCAppearPacket()); break;
		case PACKET_TYPE::NPC_DISAPPEAR: getValue = TSharedPtr<FAnsNPCDisappearPacket>(new FAnsNPCDisappearPacket()); break;
		case PACKET_TYPE::NPC_MOVE: getValue = TSharedPtr<FAnsNpcMovePacket>(new FAnsNpcMovePacket()); break;
		case PACKET_TYPE::UPDATE_UNIT_INFO: getValue = TSharedPtr<FAnsUpdateUnitInfoPacket>(new FAnsUpdateUnitInfoPacket()); break;
		}

		if (getValue != nullptr)
			return StaticCastSharedPtr<T>(getValue);
	}
	else
	{
		getValue = getArray->Pop();
		if (getValue != nullptr && getValue.IsValid())
			return StaticCastSharedPtr<T>(getValue);
	}

	return nullptr;
}

void AServerActor::RestoreAnsPacket(TSharedPtr<FAnsPacket> packet)
{
	if (packet == nullptr || packet.IsValid() == false)
		return;
	
	TArray<TSharedPtr<FAnsPacket>>* getArray = ansPacketPool.Find(packet->GetPacketType());

	if (getArray == nullptr)
	{
		TArray<TSharedPtr<FAnsPacket>> newArray;
		newArray.Emplace(packet);
		ansPacketPool.Emplace(packet->GetPacketType(), newArray);
	}
	else
	{
		getArray->Emplace(packet);
	}
}

void AServerActor::RestoreAskPacket(TSharedPtr<FAskPacket> packet)
{
	if (packet == nullptr || packet.IsValid() == false)
		return;
	
	TArray<TSharedPtr<FAskPacket>>* getArray = askPacketPool.Find(packet->GetPacketType());

	if (getArray == nullptr)
	{
		TArray<TSharedPtr<FAskPacket>> newArray;
		newArray.Emplace(packet);
		askPacketPool.Emplace(packet->GetPacketType(), newArray);
	}
	else
	{
		getArray->Emplace(packet);
	}
}

void AServerActor::RegAskPacket(TSharedPtr<FAskPacket> packet)
{
	askPackets.Enqueue(packet);
}

void AServerActor::RegAnsPacket(TSharedPtr<FAnsPacket> packet)
{
	ansPackets.Enqueue(packet);
}


// Called every frame
void AServerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (const auto& entry:pcMap)
	{
		TSharedPtr<FPCInfo> pcInfo = entry.Value;

		if (pcInfo == nullptr || pcInfo.IsValid() == false)
			continue;

		if (pcInfo->isSpawned == false)
		{
			SendAppearPC(pcInfo);
			pcInfo->isSpawned = true;
		}
	}

	if (myUnit != nullptr && myUnit.IsValid())
	{
		for (const auto& entry:npcMap)
		{
			TSharedPtr<FNPCInfo> npcInfo = entry.Value;

			if (npcInfo == nullptr || npcInfo.IsValid() == false)
				continue;

			// 시야 처리.
			if (myUnit->GetSqrDis(npcInfo->x, npcInfo->y) < mySqrSight)
			{
				// 시야 안.
				if (npcInfo->isSpawned == false)
				{
					// 스폰시킨다.
					SendAppearNPC(npcInfo);
					npcInfo->isSpawned = true;
				}
			}
			else
			{
				//  시야 밖.
				if (npcInfo->isSpawned)
				{
					// 화면에서 제거 한다.
					SendDisappearNPC(npcInfo->uniqId);
					npcInfo->isSpawned = false;
				}
			}
			
			// 이동 처리.
			if (npcInfo->curPathIndex == 0)
			{
				npcInfo->SetMove(FVector(FMath::RandRange(-2000, 2000), FMath::RandRange(-2000, 2000), 0), GetWorld());
			}
			else
			{
				npcInfo->MoveProcess();
			}

			if (npcInfo->posChanged)
			{
				npcInfo->posChanged = false;

				if (npcInfo->isSpawned)
					SendNPCMove(npcInfo);
			}
		}
	}

	// 패킷 처리.
	while (askPackets.IsEmpty() == false)
	{
		TSharedPtr<FAskPacket> getAskPacket;
		if (askPackets.Dequeue(getAskPacket))
		{
			switch (getAskPacket->GetPacketType())
			{
				case PACKET_TYPE::SKILL: SkillProcess(getAskPacket); break;
				case PACKET_TYPE::PC_MOVE: PCMoveProcess(getAskPacket); break;
			}
		}
	}

	while (ansPackets.IsEmpty() == false)
	{
		TSharedPtr<FAnsPacket> getAnsPacket;
		if (ansPackets.Dequeue(getAnsPacket))
		{
			if (ansDelegate.IsBound())
				ansDelegate.Execute(getAnsPacket);

			RestoreAnsPacket(getAnsPacket);
		}
	}
}

void AServerActor::SkillProcess(TSharedPtr<FAskPacket> packet)
{
	TSharedPtr<FAskSkillPacket> getPacket = StaticCastSharedPtr<FAskSkillPacket>(packet);

	if (getPacket == nullptr || getPacket.IsValid() == false)
		return;

	// 스킬 처리 후.
}

void AServerActor::PCMoveProcess(TSharedPtr<FAskPacket> packet)
{
	TSharedPtr<FAskPCMovePacket> getPacket = StaticCastSharedPtr<FAskPCMovePacket>(packet);

	if (getPacket == nullptr || getPacket.IsValid() == false)
		return;

	TSharedPtr<FPCInfo> getPcInfo = *pcMap.Find(getPacket->uniqId);

	if (getPcInfo == nullptr)
		return;

	getPcInfo->SetPos(getPacket->x, getPacket->y, 0);
}

void AServerActor::SendNPCMove(TSharedPtr<FNPCInfo> npcInfo)
{
	TSharedPtr<FAnsNpcMovePacket> packet = CreateAnsPacket<FAnsNpcMovePacket>(PACKET_TYPE::NPC_MOVE);

	if (packet != nullptr && packet.IsValid())
	{
		packet->ClearData();
		packet->ret = PACKET_RET::SUCCESS;
		packet->uniqId = npcInfo->uniqId;
		packet->x = npcInfo->x;
		packet->y = npcInfo->y;
		packet->z = npcInfo->z;
		packet->rot = npcInfo->rot;
		RegAnsPacket(packet);
	}
}


void AServerActor::SendAppearPC(TSharedPtr<FPCInfo> pcInfo)
{
	TSharedPtr<FAnsPCAppearPacket> packet = CreateAnsPacket<FAnsPCAppearPacket>(PACKET_TYPE::PC_APPEAR);

	if (packet != nullptr && packet.IsValid())
	{
		packet->ClearData();
		packet->ret = PACKET_RET::SUCCESS;
		
		FPCInfo& getPcInfo = packet->info.pcInfo;
		getPcInfo.ClearData();
		getPcInfo.uniqId = pcInfo->uniqId;
		getPcInfo.SetPos(pcInfo->x, pcInfo->y, pcInfo->z);
		getPcInfo.rot = pcInfo->rot;
		getPcInfo.weaponType = pcInfo->weaponType;
		getPcInfo.unitScale = pcInfo->unitScale;
		getPcInfo.unitType = pcInfo->unitType;
		getPcInfo.hp = pcInfo->hp;
		getPcInfo.maxHp = pcInfo->maxHp;
		getPcInfo.isDie = pcInfo->isDie;
		RegAnsPacket(packet);
	}
}

void AServerActor::SendAppearNPC(TSharedPtr<FNPCInfo> npcInfo)
{
	TSharedPtr<FAnsNPCAppearPacket> packet = CreateAnsPacket<FAnsNPCAppearPacket>(PACKET_TYPE::NPC_APPEAR);

	if (packet != nullptr && packet.IsValid())
	{
		packet->ClearData();
		packet->ret = PACKET_RET::SUCCESS;
		FNPCInfo& getNpcInfo = packet->info.npcInfo;
		
		getNpcInfo.ClearData();
		getNpcInfo.uniqId = npcInfo->uniqId;
		getNpcInfo.SetPos(npcInfo->x, npcInfo->y, npcInfo->z);
		getNpcInfo.rot = npcInfo->rot;
		getNpcInfo.unitScale = npcInfo->unitScale;
		getNpcInfo.hp = npcInfo->hp;
		getNpcInfo.maxHp = npcInfo->maxHp;
		getNpcInfo.isDie = npcInfo->isDie;
		RegAnsPacket(packet);
	}
}

void AServerActor::SendDisappearPC(const uint32_t& uniqId)
{
	TSharedPtr<FAnsPCDisappearPacket> packet = CreateAnsPacket<FAnsPCDisappearPacket>(PACKET_TYPE::PC_DISAPPEAR);

	if (packet != nullptr && packet.IsValid())
	{
		packet->ClearData();
		packet->ret = PACKET_RET::SUCCESS;
		packet->info.uniqId = uniqId;
		RegAnsPacket(packet);
	}
}

void AServerActor::SendDisappearNPC(const uint32_t& uniqId)
{
	TSharedPtr<FAnsNPCDisappearPacket> packet = CreateAnsPacket<FAnsNPCDisappearPacket>(PACKET_TYPE::NPC_DISAPPEAR);

	if (packet != nullptr && packet.IsValid())
	{
		packet->ClearData();
		packet->ret = PACKET_RET::SUCCESS;
		packet->info.uniqId = uniqId;
		RegAnsPacket(packet);
	}
}

void AServerActor::SendUpdateUnitInfo(const uint32_t& uniqId, const float& hp, const float& maxHp, const bool& isDie)
{
	TSharedPtr<FAnsUpdateUnitInfoPacket> packet = CreateAnsPacket<FAnsUpdateUnitInfoPacket>(PACKET_TYPE::UPDATE_UNIT_INFO);

	if (packet != nullptr && packet.IsValid())
	{
		packet->ClearData();
		packet->ret = PACKET_RET::SUCCESS;
		packet->uniqId = uniqId;
		packet->hp = hp;
		packet->maxHp = maxHp;
		packet->isDie = isDie;
		RegAnsPacket(packet);
	}
}


TSharedPtr<FPCInfo> AServerActor::CreatePC()
{
	if (pcPool.IsEmpty())
	{
		TSharedPtr<FPCInfo> getValue = TSharedPtr<FPCInfo>(new FPCInfo());
		getValue->ClearData();
		return getValue;
	}
	else
	{
		TSharedPtr<FPCInfo> getValue;
		if (pcPool.Dequeue(getValue))
		{
			getValue->ClearData();
			return getValue;
		}
	}

	return nullptr;
}

void AServerActor::RestorePC(TSharedPtr<FPCInfo> pcInfo)
{
	pcPool.Enqueue(pcInfo);
}

TSharedPtr<FNPCInfo> AServerActor::CreateNPC()
{
	if (npcPool.IsEmpty())
	{
		TSharedPtr<FNPCInfo> getValue = TSharedPtr<FNPCInfo>(new FNPCInfo());
		getValue->ClearData();
		return getValue;
	}
	else
	{
		TSharedPtr<FNPCInfo> getValue;
		if (npcPool.Dequeue(getValue))
		{
			getValue->ClearData();
			return getValue;
		}
	}

	return nullptr;
}

void AServerActor::RestoreNPC(TSharedPtr<FNPCInfo> npcInfo)
{
	npcPool.Enqueue(npcInfo);
}


uint32_t AServerActor::GetUnitUniqId()
{
	return ++temp_uniqId;
}









