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

		InitToolMeshActor(false);

		FHitResult outHit;
		// �ι�° ������ bTraceComplex �� "������ �浹�� ���� �����ؾ� �ϴ��� ����" �̷��� �ּ� ������ ����.
		if (this->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, outHit))
		{
			// outHit ������ Location, Normal, Actor... ����.
			if (outHit.bBlockingHit)
			{
				FVector pos = outHit.Location;
				int x = pos.X / toolMapGap; // ��ĭ�� 300.
				int y = pos.Y / toolMapGap;
				int index = x * toolCompressionMapSize + y;

				UE_LOG(LogTemp, Warning, TEXT("pos (%f, %f) / conv xy (%d, %d) / index (%d)"), pos.X, pos.Y, x, y, index);

				// �Էµ��� ���� ��쿡��.
				if (!toolMeshActor->xysByIndex[index]) // xysByIndex �� toolMapGap ��ŭ �����. 
				{
					// �׸��� ���� ��ǥ ��ȯ.
					x = index / toolCompressionMapSize;
					y = index % toolCompressionMapSize;
					x = x * toolMapGap;
					y = y * toolMapGap;
					
					toolMeshActor->xys.Emplace(pf::Vec2i(x, y));
					toolMeshActor->xysByIndex[index] = true;
					toolMeshActor->CreateCusXYPlane(toolMapGap);
				}
			}
		}
	}
}

void AMyPlayerController::InitToolMeshActor(bool needMeshEmpty)
{
	UWorld* world = GetWorld();

	if (world == nullptr)
		return;

	if (toolMeshActor == nullptr) // �ʱ�ȭ.
	{
		AFirstGameGameModeBase* gameMode = world->GetAuthGameMode<AFirstGameGameModeBase>();
		toolCompressionMapSize = (int)(gameMode->mapSize / (float)toolMapGap); // ��ĭ�� 300*300 ����� ���´�.
			
		toolMeshActor = world->SpawnActor<AToolMeshActor>();
		toolMeshActor->SetActorLocation(FVector(0, 0, 2000));

		if (needMeshEmpty)
			toolMeshActor->xysByIndex.Empty();
		else
			toolMeshActor->xysByIndex.Init(false, toolCompressionMapSize * toolCompressionMapSize);
	}
}


void AMyPlayerController::OnMouseUp()
{
	UE_LOG(LogTemp, Warning, TEXT("up~"));
}


