// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "Vec2i.h"
#include "GameFramework/Actor.h"
#include "ToolMeshActor.generated.h"

UCLASS()
class FIRSTGAME_API AToolMeshActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AToolMeshActor();

	class UProceduralMeshComponent* pm;
	// 입력된 x,y 지점을 시작점으로 사각 면은 그린다.
	TArray<pf::Vec2i> xys;
	// xys 에 입력된 x,y 를 1차원 배열의 해당 인덱스 위치에 true 를 입력한다.
	TArray<bool> xysByIndex;
	int32 spacing;

	void CreatePlane(int32 _width, int32 _height, float _spacing);
	// xys 배열을 변경하고 불러주면 그려준다.
	void CreateCusXYPlane(int32 _spacing);

	// mapSize 는 실제 맵 width, compression 만큼 펼쳐서 좌표를 변환해준다.
	void ConvertIndexToXys(int mapSize, int compression);
	
protected:
	
	TArray<FVector> vertices;
	TArray<FVector> normals;
	TArray<int32> triangles;
	TArray<FVector2D> uvs;
	TArray<FLinearColor> vertexColors;
	TArray<FProcMeshTangent> tangents;
	
	int32 height;
	int32 width;
	bool generateMesh;
	
	void GenerateVertices();
	void GenerateTriangles();
	void ClearMeshData();

};
