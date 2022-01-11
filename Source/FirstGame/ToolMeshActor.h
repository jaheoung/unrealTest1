// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"
#include "ToolMeshActor.generated.h"

UCLASS()
class FIRSTGAME_API AToolMeshActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AToolMeshActor();

	void Create(int32 _width, int32 _height, float _spacing);

	class UProceduralMeshComponent* pm;

protected:
	
	TArray<FVector> vertices;
	TArray<FVector> normals;
	TArray<int32> triangles;
	TArray<FVector2D> uvs;
	TArray<FLinearColor> vertexColors;
	TArray<FProcMeshTangent> tangents;
	
	int32 height;
	int32 width;
	float spacing;
	bool generateMesh;
	
	void GenerateVertices();
	void GenerateTriangles();
	void ClearMeshData();

};
