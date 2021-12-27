// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyNpc.h" // UMyUnitAnimInstance 에는 전방선언으로 해결됐는데 왜 여긴 안될까...
#include "MyNpcAnimInstance.generated.h"


/**
 * 
 */
UCLASS()
class FIRSTGAME_API UMyNpcAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="myNpc")
	NPC_STATE myNpcState;

	UMyNpcAnimInstance();
};
