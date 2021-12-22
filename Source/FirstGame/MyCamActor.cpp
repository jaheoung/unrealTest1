// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCamActor.h"

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

	baseCamPosOffset = posOffset;
	camRotOffset = rotOffset;
	hasChaseTarget = true;
	chaseTarget = unit;
	zoomDistance = _zoomDistance;

	FRotator rot = GetActorRotation();
	rot.Pitch = camRotOffset.Y;
	rot.Roll = camRotOffset.X;
	rot.Yaw = camRotOffset.Z;
	SetActorRotation(rot);

	UpdateZoom();
}

void AMyCamActor::SetZoom(float plusZoom)
{
	zoomDistance += plusZoom;
}

void AMyCamActor::UpdateZoom()
{
	camPosOffset = baseCamPosOffset + (baseCamPosOffset.GetSafeNormal() * zoomDistance);
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
	FVector resultPos = targetPos - (targetPos.ForwardVector * camPosOffset.X);
	resultPos += targetPos.LeftVector * camPosOffset.Y;
	resultPos += targetPos.UpVector * camPosOffset.Z;
	SetActorLocation(resultPos);
}

