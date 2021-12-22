// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "MyNpc.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTGAME_API AMyNpc : public AUnit
{
	GENERATED_BODY()
	
public:
	USkeletalMeshComponent* myMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MyNpc")
	class UWidgetComponent* hpBarComp;

	AMyNpc();

	virtual void Tick(float DeltaTime) override;
	virtual void ClearData() override;
	virtual void DestroyData() override;
};
