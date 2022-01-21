// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerActor.h"

#include "DrawDebugHelpers.h"
#include "FirstGameGameModeBase.h"

// Sets default values
AServerActor::AServerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AServerActor::StartServer()
{
	UWorld* world = GetWorld();

	if (world == nullptr)
		return;
	
	myUnit = CreatePC();

	astar = new pf::AStar();
	astar->loadMap();
	astar->setDiagonalMovement(true);

	AFirstGameGameModeBase* gameMode = GetWorld()->GetAuthGameMode<AFirstGameGameModeBase>();
	
	if (myUnit != nullptr && myUnit.IsValid())
	{
		myUnit->ClearData();
		myUnit->uniqId = GetUniqueID();
		myUnit->SetPos(5610, 3070, 0);
		myUnit->rot = 0;
		myUnit->weaponType = WEAPON_TYPE::SINGLE_SWORD;
		myUnit->unitScale = 0.3f;
		myUnit->unitType = 1;
		pcMap.Emplace(myUnit->uniqId, myUnit);
	}

	float curTime = world->GetTimeSeconds();

	for (int i = 0; i < 400; ++i)
	{
		float x = FMath::RandRange(0, gameMode->mapWidth);
		float y = FMath::RandRange(0, gameMode->mapWidth);

		if (astar->CanPos(x, y) == false)
			continue;
		
		TSharedPtr<FNPCInfo> getNpc = CreateNPC();

		if (getNpc == nullptr || getNpc.IsValid() == false)
			continue;

		getNpc->ClearData();
		getNpc->uniqId = GetUnitUniqId();
		getNpc->SetPos(x, y, gameMode->GetHeight(x, y));
		getNpc->rot = FMath::RandRange(0, 360);
		getNpc->unitScale = 0.3;
		getNpc->hp = 300;
		getNpc->maxHp = 300;
		getNpc->isDie = false;
		getNpc->moveDelayGap = FMath::RandRange(2, 12);
		getNpc->GetDelayMove(curTime); // 갱신.
		npcMap.Emplace(getNpc->uniqId, getNpc);
	}

	for (int i = 0; i < 50; ++i)
	{
		TSharedPtr<FInteractionObjInfo> getInteractionObj = CreateInteraction();

		if (getInteractionObj == nullptr || getInteractionObj.IsValid() == false)
			continue;

		getInteractionObj->ClearData();
		getInteractionObj->uniqId = GetUnitUniqId();
		getInteractionObj->x = FMath::RandRange(0, gameMode->mapWidth);
		getInteractionObj->y = FMath::RandRange(0, gameMode->mapWidth);
		getInteractionObj->rot = FMath::RandRange(0, 360);
		interactionMap.Emplace(getInteractionObj->uniqId, getInteractionObj);
	}
}


// Called when the game starts or when spawned
void AServerActor::BeginPlay()
{
	Super::BeginPlay();
}

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
		case PACKET_TYPE::INTERACTION_APPEAR: getValue = TSharedPtr<FAnsInteractionAppearPacket>(new FAnsInteractionAppearPacket()); break;
		case PACKET_TYPE::INTERACTION_DISAPPEAR: getValue = TSharedPtr<FAnsInteractionDisappearPacket>(new FAnsInteractionDisappearPacket()); break;
		case PACKET_TYPE::UPDATE_INVENTORY: getValue = TSharedPtr<FAnsUpdateInventoryPacket>(new FAnsUpdateInventoryPacket()); break;
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

	UWorld* world = GetWorld();

	if (world == nullptr)
		return;

	float curTime = world->GetTimeSeconds();

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
			if (npcInfo->curPathIndex == 0 && npcInfo->GetDelayMove(curTime))
			{
				npcInfo->SetMove(FVector(FMath::RandRange(0, 5000), FMath::RandRange(0, 5000), 0), GetWorld(), astar);
			}
			else
			{
				npcInfo->MoveProcess(DeltaTime);
			}

			if (npcInfo->posChanged)
			{
				npcInfo->posChanged = false;

				if (npcInfo->isSpawned)
					SendNPCMove(npcInfo);
			}
		}
		
		for (const auto& entry:interactionMap)
		{
			TSharedPtr<FInteractionObjInfo> interactionInfo = entry.Value;

			if (interactionInfo == nullptr || interactionInfo.IsValid() == false)
				continue;

			// 시야 처리.
			if (myUnit->GetSqrDis(interactionInfo->x, interactionInfo->y) < mySqrSight)
			{
				// 시야 안.
				if (interactionInfo->isSpawned == false)
				{
					// 스폰시킨다.
					SendAppearInteractionObj(interactionInfo);
					interactionInfo->isSpawned = true;
				}
			}
			else
			{
				//  시야 밖.
				if (interactionInfo->isSpawned)
				{
					// 화면에서 제거 한다.
					SendDisappearInteractionObj(interactionInfo->uniqId);
					interactionInfo->isSpawned = false;
				}
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
				case PACKET_TYPE::SKILL: AnsSkill(getAskPacket); break;
				case PACKET_TYPE::PC_MOVE: AnsPCMove(getAskPacket); break;
				case PACKET_TYPE::INTERACTION: AnsInteraction(getAskPacket); break;
			}
			RestoreAskPacket(getAskPacket);
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

void AServerActor::AnsSkill(TSharedPtr<FAskPacket> packet)
{
	TSharedPtr<FAskSkillPacket> getPacket = StaticCastSharedPtr<FAskSkillPacket>(packet);

	if (getPacket == nullptr || getPacket.IsValid() == false)
		return;

	if (getPacket->isPc)
	{
		float skillDisSqr = 0.0;
		float skillHalfAngle = 0.0;
		float skillDamage = 0.0;
		switch (getPacket->skillType)
		{
		case SKILL_TYPE::PC_DEFAULT_SKILL:
			skillDisSqr = 300 * 300;
			skillHalfAngle = 30;
			skillDamage = 100;
			break;
		}
		
		
		TSharedPtr<FPCInfo>* getPc = pcMap.Find(getPacket->castUnitUniqId);

		if (getPc != nullptr && (*getPc) != nullptr && (*getPc).IsValid())
		{
			FVector castPos = FVector((*getPc)->x, (*getPc)->y, 0);
			FVector castDirNormal = (*getPc)->lastDirNoNormal.GetSafeNormal(); // 패킷에서 받아오려고 했는데 클라에 정보가 없음.
			for (auto elem : npcMap)
			{
				TSharedPtr<FNPCInfo> npc = elem.Value;

				if (npc == nullptr || npc.IsValid() == false)
					continue;

				FVector npcPos = FVector(npc->x, npc->y, 0);
				FVector targetDirVec = npcPos - castPos;

				float dot = FVector::DotProduct(castDirNormal, targetDirVec); 

				// 전방 체크.
				if (dot < 0.0) 
					continue;

				//test
				/*
				DrawDebugLine(GetWorld(), npcPos, npcPos + FVector::UpVector * 1000, FColor::Green, false, 2);
				FVector myv = castPos;
				FVector targetv = myv + (castDirNormal * FMath::Sqrt(skillDisSqr));
				targetv.Z = myv.Z = 80;
				DrawDebugLine(GetWorld(), myv, targetv, FColor::Red, false, 2);

				FVector rightRotV = castDirNormal.RotateAngleAxis(skillHalfAngle, FVector::UpVector) * FMath::Sqrt(skillDisSqr);
				FVector leftRotV = castDirNormal.RotateAngleAxis(-skillHalfAngle, FVector::UpVector) * FMath::Sqrt(skillDisSqr);

				DrawDebugLine(GetWorld(), myv, myv + rightRotV, FColor::Red, false, 2);
				DrawDebugLine(GetWorld(), myv, myv + leftRotV, FColor::Red, false, 2);
				*/

				// 거리 체크.
				if (targetDirVec.SizeSquared() > skillDisSqr)
					continue;

				// 각도 체크.
				float rad = FMath::Acos(dot / targetDirVec.Size());
				float angle = FMath::RadiansToDegrees(rad);

				if (angle > skillHalfAngle)
					continue;

				npc->hp -= skillDamage;

				if (npc->hp <= 0)
				{
					tempDieNpcs.Emplace(npc);
				}
				else
				{
					SendUpdateUnitInfo(npc->uniqId, npc->hp, npc->maxHp);
				}
			}
		}
	}
	else
	{
		
	}

	// 죽은 유닛 제거.
	if (tempDieNpcs.Num() > 0)
	{
		for (const auto& elem : tempDieNpcs)
		{
			SendDisappearNPC(elem->uniqId);
			RestoreNPC(elem);
			npcMap.Remove(elem->uniqId);
		}

		tempDieNpcs.Empty();
	}
}

void AServerActor::AnsPCMove(TSharedPtr<FAskPacket> packet)
{
	TSharedPtr<FAskPCMovePacket> getPacket = StaticCastSharedPtr<FAskPCMovePacket>(packet);

	if (getPacket == nullptr || getPacket.IsValid() == false)
		return;

	TSharedPtr<FPCInfo>* getPcInfo = pcMap.Find(getPacket->uniqId);

	if (getPcInfo == nullptr || (*getPcInfo) == nullptr || (*getPcInfo).IsValid() == false)
		return;

	(*getPcInfo)->SetPos(getPacket->x, getPacket->y, 0);
}

void AServerActor::AnsInteraction(TSharedPtr<FAskPacket> packet)
{
	TSharedPtr<FAskInteractionPacket> getPacket = StaticCastSharedPtr<FAskInteractionPacket>(packet);

	if (getPacket == nullptr || getPacket.IsValid() == false)
		return;

	TSharedPtr<FInteractionObjInfo>* getInteractionInfo = interactionMap.Find(getPacket->uniqId);

	if (getInteractionInfo == nullptr || (*getInteractionInfo) == nullptr || (*getInteractionInfo).IsValid() == false)
		return;

	// 테스트 아이템 획득.
	bool find = false;
	for (const auto elem : myItems)
	{
		if (elem->type == ITEM_TYPE::MEAT)
		{
			++elem->count;
			find = true;
			break;
		}
	}

	if (find == false)
	{
		TSharedPtr<FMyItemInfo> newItem = CreateItem();

		if (newItem != nullptr || newItem.IsValid())
		{
			newItem->uniqId = GetUnitUniqId();
			newItem->type = ITEM_TYPE::MEAT;
			newItem->count = 1;
			myItems.Add(newItem);
		}
	}

	SendUpdateInventory();
	
	

	SendDisappearInteractionObj(getPacket->uniqId);
	
	(*getInteractionInfo)->ClearData();
	RestoreInteraction((*getInteractionInfo));
	interactionMap.Remove(getPacket->uniqId);
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
		packet->isMove = npcInfo->isMove;
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

void AServerActor::SendUpdateUnitInfo(const uint32_t& uniqId, const float& hp, const float& maxHp)
{
	TSharedPtr<FAnsUpdateUnitInfoPacket> packet = CreateAnsPacket<FAnsUpdateUnitInfoPacket>(PACKET_TYPE::UPDATE_UNIT_INFO);

	if (packet != nullptr && packet.IsValid())
	{
		packet->ClearData();
		packet->ret = PACKET_RET::SUCCESS;
		packet->uniqId = uniqId;
		packet->hp = hp;
		packet->maxHp = maxHp;
		RegAnsPacket(packet);
	}
}

void AServerActor::SendAppearInteractionObj(TSharedPtr<FInteractionObjInfo> interactionInfo)
{
	if (interactionInfo == nullptr || interactionInfo.IsValid() == false)
		return;
	
	TSharedPtr<FAnsInteractionAppearPacket> packet = CreateAnsPacket<FAnsInteractionAppearPacket>(PACKET_TYPE::INTERACTION_APPEAR);

	if (packet != nullptr && packet.IsValid())
	{
		packet->ClearData();
		packet->ret = PACKET_RET::SUCCESS;
		packet->uniqId = interactionInfo->uniqId;
		packet->x = interactionInfo->x;
		packet->y = interactionInfo->y;
		packet->rot = interactionInfo->rot;
		RegAnsPacket(packet);
	}
}

void AServerActor::SendDisappearInteractionObj(const uint32_t& uniqId)
{
	TSharedPtr<FAnsInteractionDisappearPacket> packet = CreateAnsPacket<FAnsInteractionDisappearPacket>(PACKET_TYPE::INTERACTION_DISAPPEAR);

	if (packet != nullptr && packet.IsValid())
	{
		packet->ClearData();
		packet->ret = PACKET_RET::SUCCESS;
		packet->uniqId = uniqId;
		RegAnsPacket(packet);
	}
}

void AServerActor::SendUpdateInventory()
{
	TSharedPtr<FAnsUpdateInventoryPacket> packet = CreateAnsPacket<FAnsUpdateInventoryPacket>(PACKET_TYPE::UPDATE_INVENTORY);

	if (packet != nullptr && packet.IsValid())
	{
		int makeCount = myItems.Num() - packet->items.Num();

		if (makeCount > 0)
		{
			for (int i = 0; i < makeCount; ++i)
			{
				packet->items.Emplace(*(new FMyItemInfo()));
			}
		}

		packet->ClearData();
		packet->ret = PACKET_RET::SUCCESS;
		packet->itemCount = myItems.Num();
		
		for (int i = 0; i < packet->itemCount; ++i)
		{
			FMyItemInfo& sendInfo = packet->items[i];
			TSharedPtr<FMyItemInfo> myInfo = myItems[i];

			sendInfo.uniqId = myInfo->uniqId;
			sendInfo.type = myInfo->type;
			sendInfo.count = myInfo->count;
		}
		
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
	npcInfo->ClearData();
	npcPool.Enqueue(npcInfo);
}

TSharedPtr<FInteractionObjInfo> AServerActor::CreateInteraction()
{
	if (interactionPool.IsEmpty())
	{
		TSharedPtr<FInteractionObjInfo> getValue = TSharedPtr<FInteractionObjInfo>(new FInteractionObjInfo());
		getValue->ClearData();
		return getValue;
	}
	else
	{
		TSharedPtr<FInteractionObjInfo> getValue;
		if (interactionPool.Dequeue(getValue))
		{
			getValue->ClearData();
			return getValue;
		}
	}

	return nullptr;
}

void AServerActor::RestoreInteraction(TSharedPtr<FInteractionObjInfo> interactionInfo)
{
	interactionPool.Enqueue(interactionInfo);
}

TSharedPtr<FMyItemInfo> AServerActor::CreateItem()
{
	if (itemPool.IsEmpty())
	{
		TSharedPtr<FMyItemInfo> getValue = TSharedPtr<FMyItemInfo>(new FMyItemInfo());
		getValue->ClearData();
		return getValue;
	}
	else
	{
		TSharedPtr<FMyItemInfo> getValue;
		if (itemPool.Dequeue(getValue))
		{
			getValue->ClearData();
			return getValue;
		}
	}

	return nullptr;
}

void AServerActor::RestoreItem(TSharedPtr<FMyItemInfo> itemInfo)
{
	itemPool.Enqueue(itemInfo);
}






uint32_t AServerActor::GetUnitUniqId()
{
	return ++temp_uniqId;
}









