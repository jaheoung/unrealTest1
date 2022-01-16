// Fill out your copyright notice in the Description page of Project Settings.


#include "ToolCameraActor.h"
#include "Camera/CameraComponent.h"

void AToolCameraActor::SetView()
{
	camComp = GetCameraComponent();
	camComp->ProjectionMode = ECameraProjectionMode::Orthographic;
	camComp->OrthoWidth = 35000;

	zoomDistance = 400;
	
	SetActorLocation(FVector(9400, 8800, 10000));
	SetActorRotation(FRotator(-90, 0, 0));
}

void AToolCameraActor::ZoomIn()
{
	if (camComp != nullptr)
		camComp->OrthoWidth -= zoomDistance;
}

void AToolCameraActor::ZoomOut()
{
	if (camComp != nullptr)
		camComp->OrthoWidth += zoomDistance;
}

void AToolCameraActor::Forward(float amount)
{
	FVector pos = GetActorLocation();
	pos.X += amount * 50;
	SetActorLocation(pos);
}

void AToolCameraActor::Side(float amount)
{
	FVector pos = GetActorLocation();
	pos.Y += amount * 50;
	SetActorLocation(pos);
}







