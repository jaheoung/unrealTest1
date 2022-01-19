// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCamActor.h"

#include "Kismet/KismetMathLibrary.h"

// Sets default values
AMyCamActor::AMyCamActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AMyCamActor::SetChaseTarget(AActor* unit, FVector posOffset, FVector rotOffset, float _zoomDistance)
{
	if (unit == nullptr)
		return;

	baseCamPosNormalOffset = posOffset.GetSafeNormal();
	camRotOffset = rotOffset;
	hasChaseTarget = true;
	chaseTarget = unit;
	zoomDistance = _zoomDistance;

	UpdateZoom();
}

void AMyCamActor::SetPosOffset(const bool& isUp, const float& amount)
{
	if (isUp)
		baseCamPosNormalOffset += GetActorUpVector() * amount;
	else
		baseCamPosNormalOffset += GetActorRightVector() * amount;
	
	baseCamPosNormalOffset = baseCamPosNormalOffset.GetSafeNormal();
	UpdateZoom();
}


void AMyCamActor::SetZoom(float plusZoom)
{
	zoomDistance += plusZoom;
}

void AMyCamActor::UpdateZoom()
{
	camPosOffset = baseCamPosNormalOffset * zoomDistance;
}

// Called when the game starts or when spawned
void AMyCamActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMyCamActor::Tick(float DeltaTime)
{
  	Super::Tick(DeltaTime);

	if (hasChaseTarget == false)
		return;

	if (chaseTarget == nullptr)
		return;
	
	FVector targetPos = chaseTarget->GetActorLocation();
	FVector resultPos = targetPos + camPosOffset;
	SetActorLocation(resultPos);
	
	targetPos += camRotOffset;
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(resultPos, targetPos));
}

