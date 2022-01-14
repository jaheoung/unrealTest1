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
	// �Էµ� x,y ������ ���������� �簢 ���� �׸���.
	TArray<pf::Vec2i> xys;
	// xys �� �Էµ� x,y �� 1���� �迭�� �ش� �ε��� ��ġ�� true �� �Է��Ѵ�.
	TArray<bool> xysByIndex;
	int32 spacing;

	void CreatePlane(int32 _width, int32 _height, float _spacing);
	// xys �迭�� �����ϰ� �ҷ��ָ� �׷��ش�.
	void CreateCusXYPlane(int32 _spacing);

	// mapSize �� ���� �� width, compression ��ŭ ���ļ� ��ǥ�� ��ȯ���ش�.
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
