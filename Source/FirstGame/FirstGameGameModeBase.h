// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MyBaseWidget.h"
#include "ServerActor.h"
#include "GameFramework/GameModeBase.h"
#include "FirstGameGameModeBase.generated.h"

class AMyUnit;
class AMyNpc;
class AMyCamActor;
class AServerActor;

UENUM()
enum class WIDGET_TYPE
{
	HUD = 0,
	INVENTORY,
};

/**
 * 
 */
UCLASS()
class FIRSTGAME_API AFirstGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AFirstGameGameModeBase();
	
	AServerActor* serverActor;

	TArray<float> heightMapdatas;
	int heightMapSize = 0;
	int heightMapWidth = 0;
	float heightMapRate = 0.0;
	float GetHeight(float& x, float&y);
	void ReadHeightMap();

	void AnsCallback(const TSharedPtr<FAnsPacket> packet);
	
	UPROPERTY(BlueprintReadWrite)
		AMyUnit* myUnit;

	TArray<TSharedPtr<FMyItemInfo>> myItems;
	TQueue<TSharedPtr<FMyItemInfo>> myItemPool;

	TMap<WIDGET_TYPE, TSubclassOf<UUserWidget>> uiWidgetClassMap;
	TMap<WIDGET_TYPE, UUserWidget*> uiWidgetMap;
	
	TMap<uint32, AMyNpc*> npcMap;

	TQueue<AMyNpc*> npcPool;
	AMyNpc* CreateNpc();
	void RestoreNpc(AMyNpc* npc);

	TMap<uint32, class AInteractionActor*> interactionObjMap;

	TQueue<AInteractionActor*> interactionObjPool;
	AInteractionActor* CreateInteractionObj();
	void RestoreInteractionObj(AInteractionActor* interactionObj);

	float myUnitScale = 0.24;

	AMyCamActor* characterCam;

	virtual void StartPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void ZoomIn();
	void ZoomOut();
	void TestButton();

	void OpenWidget(WIDGET_TYPE type);
	void CloseWidget(WIDGET_TYPE type);
	template<typename T>
	T* GetWidget(WIDGET_TYPE type)
	{
		UUserWidget** getWidget = uiWidgetMap.Find(type);

		if (getWidget != nullptr)
		{
			return Cast<T>(*getWidget);
		}
		return nullptr;
	}
	
private:
	void AppearMyPlayer(TSharedPtr<FAnsPCAppearPacket> packet);
	void InitPlayerController();
	void AppearNpc(TSharedPtr<FAnsNPCAppearPacket> packet);
	void DisappearNpc(TSharedPtr<FAnsNPCDisappearPacket> packet);
	void NpcMove(TSharedPtr<FAnsNpcMovePacket> packet);
	void UpdateUnitInfo(TSharedPtr<FAnsUpdateUnitInfoPacket> packet);
	void AppearInteractionObj(TSharedPtr<FAnsInteractionAppearPacket> packet);
	void DisappearInteractionObj(TSharedPtr<FAnsInteractionDisappearPacket> packet);
	void UpdateInventory(TSharedPtr<FAnsUpdateInventoryPacket> packet);
	
	
};
