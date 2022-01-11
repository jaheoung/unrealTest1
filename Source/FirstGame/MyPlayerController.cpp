// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include "DrawDebugHelpers.h"
#include "FirstGameGameModeBase.h"

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("leftClick", IE_Pressed, this, &AMyPlayerController::OnMouseDown);
	InputComponent->BindAction("leftClick", IE_Released, this, &AMyPlayerController::OnMouseUp);


	/*
	 * 해야할것.
	 * 1. 마우스 다운을 연속으로 받을수 있게 변경.
	 * 2. 변경된 인덱스만 예약해도고 그리도록 한다.
	 * 3. 지우는건 어떻게?? 방법을 찾아야할듯...
	 */
}

void AMyPlayerController::OnMouseDown()
{
	if (isMapToolMode)
	{
		UWorld* world = GetWorld();

		if (world == nullptr)
			return;

		if (mapToolData.Num() == 0)
		{
			AFirstGameGameModeBase* gameMode = world->GetAuthGameMode<AFirstGameGameModeBase>();
			toolMapSize = (int)(gameMode->mapSize / (float)toolMapGap); // 한칸이 10*10 사이즈를 갖는다.
			mapToolData.Init(false, toolMapSize * toolMapSize);
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


				//UE_LOG(LogTemp, Warning, TEXT("input x : %f, y : %f, index : %d"), pos.X, pos.Y, index);

				changedMapToolData = !mapToolData[index];
				mapToolData[index] = true;
			}
		}
	}
}

void AMyPlayerController::OnMouseUp()
{
	UE_LOG(LogTemp, Warning, TEXT("up~"));
}

void AMyPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (changedMapToolData && isMapToolMode && toolMapSize > 0)
	{
		changedMapToolData = false;
		
		UWorld* world = GetWorld();
		float planeSize = toolMapGap / 2; // 한칸의 반지름.
		int x = 0;
		int y = 0;

		for (int i = 0, k = mapToolData.Num(); i < k; ++i)
		{
			if (mapToolData[i] == true)
			{
				x = (int)(i / (float)toolMapSize);
				y = i - (x * toolMapSize);
				

				//UE_LOG(LogTemp, Warning, TEXT("- draw x : %d, y : %d, index : %d"), x, y, i);

				// FPlane 초기화 값은 방향이라고 보면되고, w 값은 그 방향의 가중치인듯.
				DrawDebugSolidPlane(world, FPlane(0,0,1,3000),
					FVector(x * toolMapGap + planeSize,
						y * toolMapGap + planeSize, 1),
						planeSize, FColor::Blue, true);
			}
		}
	}
}


