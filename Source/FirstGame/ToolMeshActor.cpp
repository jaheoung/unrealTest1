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

void AToolMeshActor::CreateCusXYPlane(int _spacing)
{
	spacing = _spacing;

	ClearMeshData();
	
	int tris[6] = {0,2,1,3,4,5};

	int halfSpacing = (int)(_spacing / 2.0);

	for (int i = 0, k = xys.Num(); i < k; ++i)
	{
		pf::Vec2i v = xys[i];

		vertices.Emplace(v.x + halfSpacing, v.y+ halfSpacing, 0);
		vertices.Emplace(v.x + spacing+ halfSpacing, v.y+ halfSpacing, 0);
		vertices.Emplace(v.x+ halfSpacing, v.y + spacing+ halfSpacing, 0);

		vertices.Emplace(v.x + spacing+ halfSpacing, v.y+ halfSpacing, 0);
		vertices.Emplace(v.x+ halfSpacing, v.y + spacing+ halfSpacing, 0);
		vertices.Emplace(v.x + spacing+ halfSpacing, v.y + spacing+ halfSpacing, 0);

		for (int q = 0; q < 6; ++q)
		{
			triangles.Emplace(tris[q] + i * 6);

			normals.Emplace(FVector(0.0f, 0.0f, 1.0f));
			uvs.Emplace(FVector2D(0, 0));
			tangents.Emplace(FProcMeshTangent(1.0f, 0.0f, 0.0f));
			vertexColors.Emplace(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
		}
	}

	pm->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uvs, vertexColors, tangents, false);
}


void AToolMeshActor::CreatePlane(int32 _width, int32 _height, float _spacing)
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


