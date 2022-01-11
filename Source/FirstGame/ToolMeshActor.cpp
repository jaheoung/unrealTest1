// Fill out your copyright notice in the Description page of Project Settings.


#include "ToolMeshActor.h"
#include "ProceduralMeshComponent.h"

// Sets default values
AToolMeshActor::AToolMeshActor()
{
	PrimaryActorTick.bCanEverTick = true;

	pm = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	pm->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
}

void AToolMeshActor::Create(int32 _width, int32 _height, float _spacing)
{
	width = _width;
	height = _height;
	spacing = _spacing;
	
	if (generateMesh == false)
	{
		generateMesh = true;

		ClearMeshData();

		GenerateVertices();
		GenerateTriangles();
	
		//Function that creates mesh section
		pm->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uvs, vertexColors, tangents, false);
	}
}

void AToolMeshActor::GenerateVertices()
{
	float uvSpacing = 1.0f / FMath::Max(height, width);

	for (int32 y = 0; y < height; y++)
	{
		for (int32 x = 0; x < width; x++)
		{
			vertices.Emplace(FVector(x * spacing, y * spacing, 0.0f));
			normals.Emplace(FVector(0.0f, 0.0f, 1.0f));
			uvs.Emplace(FVector2D(x * uvSpacing, y * uvSpacing));
			vertexColors.Emplace(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
			tangents.Emplace(FProcMeshTangent(1.0f, 0.0f, 0.0f));
		}
	}
}

void AToolMeshActor::GenerateTriangles()
{
	for (int32 y = 0; y < height - 1; y++)
	{
		for (int32 x = 0; x < width - 1; x++)
		{
			triangles.Emplace(x + (y * width));					//current vertex
			triangles.Emplace(x + (y * width) + width);			//current vertex + row
			triangles.Emplace(x + (y * width) + width + 1);		//current vertex + row + one right

			triangles.Emplace(x + (y * width));					//current vertex
			triangles.Emplace(x + (y * width) + width + 1);		//current vertex + row + one right
			triangles.Emplace(x + (y * width) + 1);				//current vertex + one right
		}
	}
}

void AToolMeshActor::ClearMeshData()
{
	vertices.Empty();
	triangles.Empty();
	uvs.Empty();
	normals.Empty();
	vertexColors.Empty();
	tangents.Empty();
}


