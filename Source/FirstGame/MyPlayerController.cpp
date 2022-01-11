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
	 * �ؾ��Ұ�.
	 * 1. ���콺 �ٿ��� �������� ������ �ְ� ����.
	 * 2. ����� �ε����� �����ص��� �׸����� �Ѵ�.
	 * 3. ����°� ���?? ����� ã�ƾ��ҵ�...
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
			toolMapSize = (int)(gameMode->mapSize / (float)toolMapGap); // ��ĭ�� 10*10 ����� ���´�.
			mapToolData.Init(false, toolMapSize * toolMapSize);
		}

		FHitResult outHit;
		// �ι�° ������ bTraceComplex �� "������ �浹�� ���� �����ؾ� �ϴ��� ����" �̷��� �ּ� ������ ����.
		if (this->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, outHit))
		{
			// outHit ������ Location, Normal, Actor... ����.
			if (outHit.bBlockingHit)
			{
				FVector pos = outHit.Location;
				int x = (int)(pos.X / (float)toolMapGap); // ��ĭ�� 10.
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
		float planeSize = toolMapGap / 2; // ��ĭ�� ������.
		int x = 0;
		int y = 0;

		for (int i = 0, k = mapToolData.Num(); i < k; ++i)
		{
			if (mapToolData[i] == true)
			{
				x = (int)(i / (float)toolMapSize);
				y = i - (x * toolMapSize);
				

				//UE_LOG(LogTemp, Warning, TEXT("- draw x : %d, y : %d, index : %d"), x, y, i);

				// FPlane �ʱ�ȭ ���� �����̶�� ����ǰ�, w ���� �� ������ ����ġ�ε�.
				DrawDebugSolidPlane(world, FPlane(0,0,1,3000),
					FVector(x * toolMapGap + planeSize,
						y * toolMapGap + planeSize, 1),
						planeSize, FColor::Blue, true);
			}
		}
	}
}


