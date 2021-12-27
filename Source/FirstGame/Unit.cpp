// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

// Sets default values
AUnit::AUnit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	notNextTickInputRot = false;
}

void AUnit::ClearData()
{
	
}

void AUnit::DestroyData()
{
	
}



void AUnit::SetMove(FVector targetPos)
{
	if (targetPos != FVector::ZeroVector && lastTargetPoint == targetPos)
		return;

	lastTargetPoint = targetPos;

	if (navSys == nullptr)
		navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (navSys == nullptr)
		return;
	
	UNavigationPath* path = navSys->FindPathToLocationSynchronously(GetWorld(), GetActorLocation(), targetPos, NULL);

	int pathCount = path->PathPoints.Num();

	if (pathCount != myPath.Num())
		myPath.SetNum(pathCount);

	// PathPoints[0] 은 내 위치다.
	// 모든 path의 z(위쪽)값은 해당 위치의 바운드 볼륨의 면의 위치이다.
	for (int i = 0; i < pathCount; ++i)
	{
		FVector pathElem = path->PathPoints[i];
		//pathElem.Z += 100; // 공중에 뜨게 하기 위해.
		myPath[i] = pathElem;
	}

	curPathIndex = 1;
}

void AUnit::StopMovePath()
{
	lastTargetPoint = FVector::ZeroVector;
	curPathIndex = 0;
}

float AUnit::GetMoveSpeed()
{
	return moveSpeed;
}

bool AUnit::IsMovePath()
{
	return curPathIndex > 0 && curPathIndex < myPath.Num();
}

// Called when the game starts or when spawned
void AUnit::BeginPlay()
{
	Super::BeginPlay();
}

FVector AUnit::GetNextMovePathPos()
{
	if (curPathIndex > 0 && curPathIndex < myPath.Num())
	{
		FVector curPos = GetActorLocation();
		FVector nextPos = myPath[curPathIndex];
		curPos.Z = nextPos.Z = 0;

		FVector dirVec = nextPos - curPos;
		float checkDis = GetMoveSpeed();
		checkDis *= checkDis;

		if (dirVec.SizeSquared() <= checkDis)
		{
			//SetActorLocation(myPath[curPathIndex]);
			++curPathIndex;
			notNextTickInputRot = true; // 목적지로 바로 이동하기에 회전 적용하지 않음.
			return nextPos;
		}

		//SetActorLocation(curPos + (dirVec.GetSafeNormal() * speed));
		return dirVec;
	}
	else
	{
		curPathIndex = 0;
		return FVector::ZeroVector;
	}
}

// Called every frame
void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (curPathIndex > 0)
	{
		lastNextMovePathPos = GetNextMovePathPos();
		inputVector = FVector2D(lastNextMovePathPos.X, lastNextMovePathPos.Y);
	}

	if (inputVector != FVector2D::ZeroVector)
	{
		lastInputVector = inputVector;
		float getInputSize = inputVector.Size();

		if (getInputSize > 1.f)
			inputVector /= getInputSize;

		if (notNextTickInputRot == false)
		{
			float dot = FVector2D::DotProduct(FVector2D(1, 0), inputVector);
			float arcRad = FMath::Acos(dot);
			FVector cross = FVector::CrossProduct(FVector::ForwardVector, FVector(inputVector.X, inputVector.Y, 0));

			if (cross.Z < 0.f)
				arcRad *= -1;

			FQuat quat = FQuat(FVector::UpVector, arcRad);

			SetActorRotation(quat);
		}

		notNextTickInputRot = false;

		inputVector *= GetMoveSpeed();

		FVector pos = GetActorLocation();
		pos.X += inputVector.X;
		pos.Y += inputVector.Y;
		SetLastPos(pos.X, pos.Y);
		SetActorLocation(pos);

		inputVector = FVector2D::ZeroVector;
	}
}

bool AUnit::SetLastPos(float& x, float& y)
{
	bool changed = lastX != x || lastY != y;
	lastX = x;
	lastY = y;
	return changed;
}

