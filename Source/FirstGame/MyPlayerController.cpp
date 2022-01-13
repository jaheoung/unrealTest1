// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "FirstGameGameModeBase.h"
#include "ToolMeshActor.h"

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("leftClick", IE_Released, this, &AMyPlayerController::OnMouseUp);
	InputComponent->BindAxis("leftClickAxis", this, &AMyPlayerController::OnMouseDownAxis);
}

void AMyPlayerController::OnMouseDownAxis(float amount)
{
	if (amount <= 0.0)
		return;

	if (isMapToolMode)
	{
		UWorld* world = GetWorld();

		if (world == nullptr)
			return;

		if (toolMeshActor == nullptr) // 초기화.
		{
			AFirstGameGameModeBase* gameMode = world->GetAuthGameMode<AFirstGameGameModeBase>();
			toolMapSize = (int)(gameMode->mapSize / (float)toolMapGap); // 한칸이 10*10 사이즈를 갖는다.
			
			toolMeshActor = world->SpawnActor<AToolMeshActor>();
			toolMeshActor->SetActorLocation(FVector(0, 0, 2000));
			toolMeshActor->xysByIndex.Init(false, toolMapSize * toolMapSize);
		}

		FHitResult outHit;
		// 두번째 인자인 bTraceComplex 는 "복잡한 충돌에 대해 추적해야 하는지 여부" 이렇게 주석 내용이 있음.
		if (this->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, outHit))
		{
			// outHit 정보에 Location, Normal, Actor... 있음.
			if (outHit.bBlockingHit)
			{
				FVector pos = outHit.Location;
				int x = (int)(pos.X / (float)toolMapGap); // 한칸이 10.
				int y = (int)(pos.Y / (float)toolMapGap);
				int index = x * toolMapSize + y;

				isChangedData = !toolMeshActor->xysByIndex[index];

				if (isChangedData)
				{
					isChangedData = false;
					
					float planeSize = toolMapGap / 2; // 한칸의 반지름.
					
					x = (int)(index / (float)toolMapSize);
					y = index - (x * toolMapSize);
					x = x * toolMapGap + planeSize;
					y = y * toolMapGap + planeSize;
					
					toolMeshActor->xys.Emplace(pf::Vec2i(x, y));
					toolMeshActor->xysByIndex[index] = true;
					toolMeshActor->CreateCusXYPlane(toolMapGap);
				}
			}
		}
	}
}

void AMyPlayerController::OnMouseUp()
{
	UE_LOG(LogTemp, Warning, TEXT("up~"));
}


