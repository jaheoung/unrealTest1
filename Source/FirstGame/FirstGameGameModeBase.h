// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ServerActor.h"
#include "GameFramework/GameModeBase.h"
#include "FirstGameGameModeBase.generated.h"

class AMyUnit;
class AMyNpc;
class AMyCamActor;
class AServerActor;

/**
 * 
 */
UCLASS()
class FIRSTGAME_API AFirstGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AServerActor* serverActor;

	void AnsCallback(const TSharedPtr<FAnsPacket> packet);
	
	UPROPERTY(BlueprintReadWrite)
		AMyUnit* myUnit;

	TMap<uint32, AMyNpc*> npcMap;

	TQueue<AMyNpc*> npcPool;
	AMyNpc* CreateNpc();
	void RestoreNpc(AMyNpc* npc);

	float myUnitScale = 0.24;

	AMyCamActor* characterCam;

	virtual void StartPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void ZoomIn();
	void ZoomOut();
	void TestButton();
	
private:
	void AppearMyPlayer(TSharedPtr<FAnsPCAppearPacket> packet);
	void InitPlayerController();
	void AppearNpc(TSharedPtr<FAnsNPCAppearPacket> packet);
	void DisappearNpc(TSharedPtr<FAnsNPCDisappearPacket> packet);
	void NpcMove(TSharedPtr<FAnsNpcMovePacket> packet);
	void UpdateUnitInfo(TSharedPtr<FAnsUpdateUnitInfoPacket> packet);
	
};
