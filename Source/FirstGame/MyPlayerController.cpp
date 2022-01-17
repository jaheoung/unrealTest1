// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "FirstGameGameModeBase.h"
#include "ToolMeshActor.h"

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("leftClick", IE_Released, this, &AMyPlayerController::OnMouseUp);
	InputComponent->BindAxis("leftClickAxis", this, &AMyPlayerController::LeftClickAxis);
	InputComponent->BindAxis("rightClickAxis", this, &AMyPlayerController::RightClickAxis);
}

void AMyPlayerController::RightClickAxis(float amount)
{
	if (amount <= 0.0)
		return;

	if (isMapToolMode == false)
		return;

	UWorld* world = GetWorld();

	if (world == nullptr)
		return;

	InitToolMeshActor(false);

	FHitResult outHit;
	// 두번째 인자인 bTraceComplex 는 "복잡한 충돌에 대해 추적해야 하는지 여부" 이렇게 주석 내용이 있음.
	if (this->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, outHit))
	{
		// outHit 정보에 Location, Normal, Actor... 있음.
		if (outHit.bBlockingHit)
		{
			FVector pos = outHit.Location;
			SetToolMesh(pos.X, pos.Y, false, toolSelectRange);
		}
	}
}


void AMyPlayerController::LeftClickAxis(float amount)
{
	if (amount <= 0.0)
		return;

	if (isMapToolMode == false)
		return;

	UWorld* world = GetWorld();

	if (world == nullptr)
		return;

	InitToolMeshActor(false);

	FHitResult outHit;
	// 두번째 인자인 bTraceComplex 는 "복잡한 충돌에 대해 추적해야 하는지 여부" 이렇게 주석 내용이 있음.
	if (this->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, outHit))
	{
		// outHit 정보에 Location, Normal, Actor... 있음.
		if (outHit.bBlockingHit)
		{
			FVector pos = outHit.Location;
			SetToolMesh(pos.X, pos.Y, true, toolSelectRange);
		}
	}
}

void AMyPlayerController::SetToolMesh(const float& _x, const float& _y, bool isSelect, const int& range)
{
	if (range > 0)
	{
		float tempX = _x;
		float tempY = _y;
		float rangeGap = range * toolMapGap;

		for (int i = tempX + rangeGap, k = tempX - rangeGap; i >= k; i -= toolMapGap)
		{
			for (int q = tempY - rangeGap, w = tempY + rangeGap; q <= w; q += toolMapGap)
			{
				SetToolMesh(i, q, isSelect, 0);
			}
		}
	}

	int x = _x / toolMapGap; // 기본은 한칸이 300.
	int y = _y / toolMapGap;

	if (x < 0 || x >= compressionMapWidth || y < 0 || y >= compressionMapWidth)
		return;
	
	int index = x * compressionMapWidth + y;

	// 변경된 경우에만.
	if ((isSelect && !toolMeshActor->xysByIndex[index]) ||
		(!isSelect && toolMeshActor->xysByIndex[index])) // xysByIndex 는 toolMapGap 만큼 압축됨. 
	{
		// 그리기 위한 좌표 변환.
		x = index / compressionMapWidth;
		y = index % compressionMapWidth;
		x = x * toolMapGap;
		y = y * toolMapGap;

		if (isSelect)
		{
			toolMeshActor->xys.Emplace(index, pf::Vec2i(x, y));
			toolMeshActor->xysByIndex[index] = true;
		}
		else
		{
			toolMeshActor->xys.Remove(index);
			toolMeshActor->xysByIndex[index] = false;
		}
		toolMeshActor->CreateCusXYPlane(toolMapGap);
	}
}


void AMyPlayerController::InitToolMeshActor(bool needMeshEmpty)
{
	UWorld* world = GetWorld();

	if (world == nullptr)
		return;

	if (toolMeshActor == nullptr) // 초기화.
	{
		AFirstGameGameModeBase* gameMode = world->GetAuthGameMode<AFirstGameGameModeBase>();
		compressionMapWidth = (int)(gameMode->mapWidth / (float)toolMapGap); // 한칸이 300*300 사이즈를 갖는다.
			
		toolMeshActor = world->SpawnActor<AToolMeshActor>();
		toolMeshActor->SetActorLocation(FVector(0, 0, 2000));

		if (needMeshEmpty)
			toolMeshActor->xysByIndex.Empty();
		else
			toolMeshActor->xysByIndex.Init(false, compressionMapWidth * compressionMapWidth);
	}
}


void AMyPlayerController::OnMouseUp()
{
	UE_LOG(LogTemp, Warning, TEXT("up~"));
}


