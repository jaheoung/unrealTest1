// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <stack>
#include <ThirdParty/openexr/Deploy/OpenEXR-2.3.0/OpenEXR/include/ImathMath.h>

#include "CoreMinimal.h"
#include "Astar.h"
#include "GameFramework/Actor.h"
#include "Weapon.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "ServerActor.generated.h"


//------------------------------------------------- 전달 구조체 시작.

enum class ITEM_TYPE
{
	NONE,
	MEAT,
	GOLD,
};

struct FMyItemInfo
{
	uint32_t uniqId;
	ITEM_TYPE type;
	uint32_t count;

	void ClearData()
	{
		uniqId = 0;
		type = ITEM_TYPE::NONE;
		count = 0;
	}
};

struct FUnitInfo
{
	// 1 : pc, 2 : npc
	char unitType;
	// 고유번호.
	uint32_t uniqId;
	// 현재 위치.
    float x;
	// 현재 위치.
    float y;
	// 현재 위치.
	float z;
	// 현재 위치.
    float rot;
	// 유닛의 스케일.
    float unitScale;

	// true 면 클라 기준 스폰 상태.
	bool isSpawned;
	// true 이면 위치 변경됨.
	bool posChanged = false;

	float hp;
	float maxHp;
	bool isDie;

	///------ 이하 이동 관련.
	// z 는 고려하지 않는다.
	FVector curPos;
	TArray<FVector> myPath;
	int curPathIndex;
	FVector lastTargetPoint;
	float moveSpeed = 0.9f;
	class UNavigationSystemV1* navSys;
	FVector lastDirNoNormal;

	// 현재위치 x,y,z 는 반영하지만 반환되는 방향은 x,y 로만 계산된다. (pc 는 z 값을 사용하지 않는다.)
	const FVector& SetPos(float _x, float _y, float _z)
	{
		posChanged = x != _x || y != _y || z != _z;

		lastDirNoNormal.X = x;
		lastDirNoNormal.Y = y;
		lastDirNoNormal.Z = 0;
		
		x = _x;
		y = _y;
		z = _z;
		
		curPos.X = _x;
		curPos.Y = _y;
		curPos.Z = 0;

		lastDirNoNormal = curPos - lastDirNoNormal;
		
		return lastDirNoNormal;
	}

	float GetSqrDis(const float& targetX, const float& targetY)
	{
		return FMath::Pow(targetX - x, 2) + FMath::Pow(targetY - y, 2);
	}
	
	virtual void ClearData()
	{
		unitType = 0;
		uniqId = 0;
		SetPos(0, 0, 0);
		rot = 0;
		unitScale = 0;
		isSpawned = false;
		myPath.Reset();
		curPathIndex = 0;
		lastTargetPoint.X = 0;
		lastTargetPoint.Y = 0;
		lastTargetPoint.Z = 0;
		navSys = nullptr;
		hp = 0;
		maxHp = 0;
		isDie = false;
	}

	void StopMovePath()
	{
		lastTargetPoint.X = 0;
		lastTargetPoint.Y = 0;
		lastTargetPoint.Z = 0;
		curPathIndex = 0;
	}

	void SetMove(FVector targetPos, UWorld* world, pf::AStar* astar)
	{
		if (targetPos != FVector::ZeroVector && lastTargetPoint == targetPos)
			return;

		if (world == nullptr)
			return;

		lastTargetPoint = targetPos;

		if (navSys == nullptr)
			navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(world);

		if (navSys == nullptr)
			return;

		//UNavigationPath* path = navSys->FindPathToLocationSynchronously(world, curPos, targetPos, NULL);

		auto path = astar->findPath(pf::Vec2i(curPos.X, curPos.Y), pf::Vec2i(targetPos.X, targetPos.Y), pf::heuristic::manhattan, 10);

		int pathCount = path.size();

		if (pathCount != myPath.Num())
			myPath.SetNum(pathCount);

		for (int i = 0; i < pathCount; ++i)
		{
			myPath[i] = FVector(path[i].x, path[i].y, 0);
		}

		curPathIndex = 1;
	}

	float GetRot(FVector& dir)
	{
		float dot = FVector2D::DotProduct(FVector2D(1, 0), FVector2D(dir.X, dir.Y));
		float arcRad = FMath::Acos(dot);
		FVector cross = FVector::CrossProduct(FVector::ForwardVector, FVector(dir.X, dir.Y, 0));

		if (cross.Z < 0.f)
			arcRad *= -1;
		
		return FMath::RadiansToDegrees(arcRad);
	}

	void MoveProcess()
	{
		FVector dir;
		if (curPathIndex > 0 && curPathIndex < myPath.Num())
		{
			FVector nextPos = myPath[curPathIndex];
			float backZ = nextPos.Z;
			curPos.Z = nextPos.Z = 0;

			FVector dirVec = nextPos - curPos;
			float checkDis = moveSpeed;
			checkDis *= checkDis;

			if (dirVec.SizeSquared() <= checkDis)
			{
				dir = SetPos(nextPos.X, nextPos.Y, backZ).GetSafeNormal();
				rot = GetRot(dir);
				// 현재 목적지 도착, 다음 목적지 설정.
				++curPathIndex;
			}
			else
			{
				dirVec.Normalize();
				nextPos = curPos + (dirVec * moveSpeed);
				dir = SetPos(nextPos.X, nextPos.Y, backZ).GetSafeNormal();
				rot = GetRot(dir);
			}

			
		}
		else
		{
			// 최종 목적지 도착.
			curPathIndex = 0;
		}
	}

	FUnitInfo& operator=(const FUnitInfo& info)
	{
		unitType = info.unitType;
		uniqId = info.uniqId;
		SetPos(info.x, info.y, info.z);
		rot = info.rot;
		unitScale = info.unitScale;
		isSpawned = info.isSpawned;
		return *this;
	}
};
struct FPCInfo : public FUnitInfo
{
	// 무기 타입.
    WEAPON_TYPE weaponType;

	virtual void ClearData() override
	{
		FUnitInfo::ClearData();
		weaponType = WEAPON_TYPE::NONE;
	}

	FPCInfo& operator=(const FPCInfo& info)
	{
		FUnitInfo::operator=(info);
		weaponType = info.weaponType;
		return *this;
	}
};

struct FNPCInfo : public FUnitInfo
{
	virtual void ClearData() override
	{
		FUnitInfo::ClearData();
	}

	FNPCInfo& operator=(const FNPCInfo& info)
	{
		FUnitInfo::operator=(info);
		return *this;
	}
};

// pc 스폰 정보.
struct FPCAppearInfo
{
private:
	FPCInfo _pcInfo;
public:
	FPCInfo& pcInfo = _pcInfo;
};

// pc 제거 정보.
struct FPCDisappearInfo
{
	uint32_t uniqId;
};

// npc 스폰 정보.
struct FNPCAppearInfo
{
private:
	FNPCInfo _npcInfo;
public:
	FNPCInfo& npcInfo = _npcInfo;
};

// npc 제거 정보.
struct FNPCDisappearInfo
{
	uint32_t uniqId;
};

struct FInteractionObjInfo
{
	uint32_t uniqId;
	float x;
	float y;
	float rot;
	bool isSpawned;

	void ClearData()
	{
		uniqId = 0;
		x = 0;
		y = 0;
		rot = 0;
		isSpawned = false;
	}
};

enum class SKILL_TYPE
{
	NONE,
	PC_DEFAULT_SKILL,
	NPC_DEFAULT_SKILL,
};

//------------------------------------------------- 전달 구조체 끝.
//
//------------------------------------------------- 패킷정보 시작.
enum class PACKET_TYPE
{
	NONE,
	SKILL,
	PC_MOVE,
	PC_APPEAR,
	PC_DISAPPEAR,
	NPC_APPEAR,
	NPC_DISAPPEAR,
	NPC_MOVE,
	UPDATE_UNIT_INFO,
	INTERACTION_APPEAR,
	INTERACTION_DISAPPEAR,
	INTERACTION,
	UPDATE_INVENTORY,
};

enum class PACKET_RET
{
	SUCCESS,
	FAIL,
};


struct FAskPacket
{
	virtual PACKET_TYPE GetPacketType()
	{
		return PACKET_TYPE::NONE;
	}

	virtual void ClearData()
	{
	}
};

struct FAnsPacket
{
	PACKET_RET ret;
	
	virtual PACKET_TYPE GetPacketType()
	{
		return PACKET_TYPE::NONE;
	}

	virtual void ClearData()
	{
		ret = PACKET_RET::FAIL;
	}
};

// 스킬 요청.
struct FAskSkillPacket : public FAskPacket
{
public:
	bool isPc;
	uint32_t castUnitUniqId;
	SKILL_TYPE skillType;
	FVector dirNormal;

	virtual PACKET_TYPE GetPacketType() override
	{
		return PACKET_TYPE::SKILL;
	}

	virtual void ClearData() override
	{
		FAskPacket::ClearData();

		isPc = false;
		castUnitUniqId = 0;
		skillType = SKILL_TYPE::NONE;
		dirNormal.X = 0;
		dirNormal.Y = 0;
		dirNormal.Z = 0;
	}
};

// 스킬 응답.
struct FAnsSkillPacket : public FAnsPacket
{
	uint32_t targetUnitUniqId;
	float targetHp;
	
	virtual PACKET_TYPE GetPacketType() override
	{
		return PACKET_TYPE::SKILL;
	}
	
	virtual void ClearData() override
	{
		FAnsPacket::ClearData();
	}
};

// pc 이동 요청 (ask 만 있음)
struct FAskPCMovePacket : public FAskPacket
{
public:
	uint32_t uniqId;
	float x;
	float y;
	
	virtual PACKET_TYPE GetPacketType() override
	{
		return PACKET_TYPE::PC_MOVE;
	}

	virtual void ClearData() override
	{
		FAskPacket::ClearData();

		uniqId = 0;
		x = 0;
		y = 0;
	}
};

// npc 이동.
struct FAnsNpcMovePacket : public FAnsPacket
{
	uint32_t uniqId;
	float x;
	float y;
	float z;
	float rot;
	
	virtual PACKET_TYPE GetPacketType() override
	{
		return PACKET_TYPE::NPC_MOVE;
	}

	virtual void ClearData() override
	{
		FAnsPacket::ClearData();
		
		uniqId = 0;
		x = 0;
		y = 0;
		z = 0;
		rot = 0;
	}
};

// 갱신할 유닛 정보가 있을때 사용. (move 보다는 덜 사용한다.)
struct FAnsUpdateUnitInfoPacket : public FAnsPacket
{
	uint32_t uniqId;
	float hp;
	float maxHp;
	
	virtual PACKET_TYPE GetPacketType() override
	{
		return PACKET_TYPE::UPDATE_UNIT_INFO;
	}

	virtual void ClearData() override
	{
		FAnsPacket::ClearData();
		
		uniqId = 0;
		hp = 0;
		maxHp = 0;
	}
};

// pc 스폰.
struct FAnsPCAppearPacket : public FAnsPacket
{
private:
	FPCAppearInfo _info;
public:
	FPCAppearInfo& info = _info;
	
	virtual PACKET_TYPE GetPacketType() override
	{
		return PACKET_TYPE::PC_APPEAR;
	}

	virtual void ClearData() override
	{
		FAnsPacket::ClearData();
	}
};

// pc 제거.
struct FAnsPCDisappearPacket : public FAnsPacket
{
private:
	FPCDisappearInfo _info;
public:
	FPCDisappearInfo& info = _info;
	
	virtual PACKET_TYPE GetPacketType() override
	{
		return PACKET_TYPE::PC_DISAPPEAR;
	}

	virtual void ClearData() override
	{
		FAnsPacket::ClearData();
	}
};

// npc 스폰.
struct FAnsNPCAppearPacket : public FAnsPacket
{
private:
	FNPCAppearInfo _info;
public:
	FNPCAppearInfo& info = _info;
	
	virtual PACKET_TYPE GetPacketType() override
	{
		return PACKET_TYPE::NPC_APPEAR;
	}

	virtual void ClearData() override
	{
		FAnsPacket::ClearData();
	}
};

// npc 제거.
struct FAnsNPCDisappearPacket : public FAnsPacket
{
private:
	FNPCDisappearInfo _info;
public:
	FNPCDisappearInfo& info = _info;
	
	virtual PACKET_TYPE GetPacketType() override
	{
		return PACKET_TYPE::NPC_DISAPPEAR;
	}

	virtual void ClearData() override
	{
		FAnsPacket::ClearData();
	}
};

// 인터렉션 오브젝트 스폰.
struct FAnsInteractionAppearPacket : public FAnsPacket
{
public:
	uint32_t uniqId;
	float x;
	float y;
	float rot;
	
	virtual PACKET_TYPE GetPacketType() override
	{
		return PACKET_TYPE::INTERACTION_APPEAR;
	}

	virtual void ClearData() override
	{
		FAnsPacket::ClearData();
		uniqId = 0;
		x = 0;
		y = 0;
		rot = 0;
	}
};

// 인터렉션 오브젝트 제거.
struct FAnsInteractionDisappearPacket : public FAnsPacket
{
public:
	uint32_t uniqId;
	
	virtual PACKET_TYPE GetPacketType() override
	{
		return PACKET_TYPE::INTERACTION_DISAPPEAR;
	}

	virtual void ClearData() override
	{
		FAnsPacket::ClearData();
		uniqId = 0;
	}
};

// 인터렉션 요청.
struct FAskInteractionPacket : public FAskPacket
{
public:
	uint32_t uniqId;
	
	virtual PACKET_TYPE GetPacketType() override
	{
		return PACKET_TYPE::INTERACTION;
	}

	virtual void ClearData() override
	{
		FAskPacket::ClearData();

		uniqId = 0;
	}
};

struct FAnsUpdateInventoryPacket : public FAnsPacket
{
public:
	int itemCount;
	TArray<FMyItemInfo> items;
	
	virtual PACKET_TYPE GetPacketType() override
	{
		return PACKET_TYPE::UPDATE_INVENTORY;
	}

	virtual void ClearData() override
	{
		FAnsPacket::ClearData();

		for (auto& elem : items)
		{
			elem.ClearData();
		}
	}
};


//------------------------------------------------- 패킷정보 끝.

DECLARE_DELEGATE_OneParam(AnsDelegate, TSharedPtr<FAnsPacket>);

UCLASS()
class FIRSTGAME_API AServerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AServerActor();

	pf::AStar* astar;

	void StartServer();

	// 패킷 재사용 구조체를 가져온다. (헤더에 정의된 이유는 외부에서 템플릿으로 사용하면 선언부와 정의부가 같이 있지 않으면 에러나기 때문)
	template<typename T>
	TSharedPtr<T> CreateAskPacket(PACKET_TYPE type)
	{
		TArray<TSharedPtr<FAskPacket>>* getArray = askPacketPool.Find(type);

		TSharedPtr<FAskPacket> getValue = nullptr;
	
		if (getArray == nullptr || getArray->Num() == 0)
		{
			switch (type)
			{
			case PACKET_TYPE::SKILL: getValue = TSharedPtr<FAskSkillPacket>(new FAskSkillPacket()); break;
			case PACKET_TYPE::PC_MOVE: getValue = TSharedPtr<FAskPCMovePacket>(new FAskPCMovePacket()); break;
			case PACKET_TYPE::INTERACTION: getValue = TSharedPtr<FAskInteractionPacket>(new FAskInteractionPacket()); break;
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
	// 패킷 재사용 구조체를 가져온다.
	template<typename T>
	TSharedPtr<T> CreateAnsPacket(PACKET_TYPE type);

	void RestoreAskPacket(TSharedPtr<FAskPacket> packet);
	void RestoreAnsPacket(TSharedPtr<FAnsPacket> packet);

	// 패킷 응답 콜백.
	AnsDelegate ansDelegate;
	
	// 요청 패킷을 등록.
	void RegAskPacket(TSharedPtr<FAskPacket> packet);
	// 응답 패킷을 등록.
	void RegAnsPacket(TSharedPtr<FAnsPacket> packet);

private:
	// 다음 프레임에 처리할 요청 패킷 목록.
	TQueue<TSharedPtr<FAskPacket>> askPackets;
	// 다음 프레임에 처리할 응답 패킷 목록.
	TQueue<TSharedPtr<FAnsPacket>> ansPackets;

	// 재사용 ask 패킷 목록 (key : 패킷종류, value : 해당 패킷 종류의 패킷 큐)
	TMap<PACKET_TYPE, TArray<TSharedPtr<FAskPacket>>> askPacketPool;
	// 재사용 ans 패킷 목록 (key : 패킷종류, value : 해당 패킷 종류의 패킷 큐)
	TMap<PACKET_TYPE, TArray<TSharedPtr<FAnsPacket>>> ansPacketPool;

	// 요청받은 스킬을 처리.
	void AnsSkill(TSharedPtr<FAskPacket> packet);
	void AnsPCMove(TSharedPtr<FAskPacket> packet);
	void AnsInteraction(TSharedPtr<FAskPacket> packet);
	
	void SendNPCMove(TSharedPtr<FNPCInfo> npcInfo);
	void SendAppearPC(TSharedPtr<FPCInfo> pcInfo);
	void SendAppearNPC(TSharedPtr<FNPCInfo> npcInfo);
	void SendDisappearPC(const uint32_t& uniqId);
	void SendDisappearNPC(const uint32_t& uniqId);
	void SendUpdateUnitInfo(const uint32_t& uniqId, const float& hp, const float& maxHp);
	void SendAppearInteractionObj(TSharedPtr<FInteractionObjInfo> interactionInfo);
	void SendDisappearInteractionObj(const uint32_t& uniqId);
	void SendUpdateInventory();

	TArray<TSharedPtr<FMyItemInfo>> myItems;
	
	// 내 시야 거리.
	float mySqrSight = 800.0 * 800.0;
	TSharedPtr<FPCInfo> myUnit;
	TMap<uint32_t, TSharedPtr<FPCInfo>> pcMap;
	TMap<uint32_t, TSharedPtr<FNPCInfo>> npcMap;
	TMap<uint32_t, TSharedPtr<FInteractionObjInfo>> interactionMap;

	TQueue<TSharedPtr<FPCInfo>> pcPool;
	TQueue<TSharedPtr<FNPCInfo>> npcPool;
	TQueue<TSharedPtr<FInteractionObjInfo>> interactionPool;
	TQueue<TSharedPtr<FMyItemInfo>> itemPool;

	TArray<TSharedPtr<FNPCInfo>> tempDieNpcs;

	uint32_t temp_uniqId = 0;

	TSharedPtr<FPCInfo> CreatePC();
	void RestorePC(TSharedPtr<FPCInfo> pcInfo);
	TSharedPtr<FNPCInfo> CreateNPC();
	void RestoreNPC(TSharedPtr<FNPCInfo> npcInfo);
	TSharedPtr<FInteractionObjInfo> CreateInteraction();
	void RestoreInteraction(TSharedPtr<FInteractionObjInfo> interactionInfo);
	TSharedPtr<FMyItemInfo> CreateItem();
	void RestoreItem(TSharedPtr<FMyItemInfo> itemInfo);

	
	uint32_t GetUnitUniqId();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
