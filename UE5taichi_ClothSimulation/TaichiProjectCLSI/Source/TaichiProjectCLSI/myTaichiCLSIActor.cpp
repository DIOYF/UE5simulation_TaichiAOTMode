// Fill out your copyright notice in the Description page of Project Settings.


#include "myTaichiCLSIActor.h"

// Sets default values
AmyTaichiCLSIActor::AmyTaichiCLSIActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Pos.resize(arr_size_x * arr_size_y * 3);
	Vel.resize(arr_size_x * arr_size_y * 3);

	for (int i = 0; i < arr_size_x; i++)
	{
		for (int j = 0; j < arr_size_y; j++)
		{
			Pos[3 * (i * arr_size_x + j) + 0] = i * 1.0f / arr_size_x - 0.5f + FMath::RandRange(-0.001f, 0.001f);
			Vel[3 * (i * arr_size_x + j) + 0] = 0.f;
			Pos[3 * (i * arr_size_x + j) + 1] = j * 1.0f / arr_size_y - 0.5f + FMath::RandRange(-0.001f, 0.001f);
			Vel[3 * (i * arr_size_x + j) + 1] = 0.f;
			Pos[3 * (i * arr_size_x + j) + 2] = 0.6f;
			Vel[3 * (i * arr_size_x + j) + 2] = 0.f;
		}
	}

	InitInstances();

}

// Called when the game starts or when spawned
void AmyTaichiCLSIActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AmyTaichiCLSIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	myclsimodule.compute(Vel,Pos);
	for (int i = 0; i < arr_size_x * arr_size_y; i++)
	{
		vertices[i] = FVector(Pos[3 * i ] * 300.0f, Pos[3 * i + 1] * 300.0f, Pos[3 * i + 2] * 300.0f);
	}

	UpdateMesh();

}


void AmyTaichiCLSIActor::InitInstances()
{
	vertices.Init(FVector(0., 0., 0.), arr_size_x * arr_size_y);
	indices.Init(0, (arr_size_x - 1) * (arr_size_y - 1) * 6);
	indices_1.Init(0, (arr_size_x - 1) * (arr_size_y - 1) * 6);
	uv.Init(FVector2d(0, 0), arr_size_x * arr_size_y);

	for (int i = 0; i < arr_size_x - 1; i++)
	{
		for (int j = 0; j < arr_size_y - 1; j++)
		{
			int quad_id = i * (arr_size_y - 1) + j;
			indices[quad_id * 6 + 0] = i * arr_size_y + j;
			indices[quad_id * 6 + 1] = (i + 1) * arr_size_y + j;
			indices[quad_id * 6 + 2] = i * arr_size_y + (j + 1);

			indices[quad_id * 6 + 3] = (i + 1) * arr_size_y + j + 1;
			indices[quad_id * 6 + 4] = i * arr_size_y + j + 1;
			indices[quad_id * 6 + 5] = (i + 1) * arr_size_y + j;

			indices_1[quad_id * 6 + 2] = i * arr_size_y + j;
			indices_1[quad_id * 6 + 1] = (i + 1) * arr_size_y + j;
			indices_1[quad_id * 6 + 0] = i * arr_size_y + (j + 1);

			indices_1[quad_id * 6 + 5] = (i + 1) * arr_size_y + j + 1;
			indices_1[quad_id * 6 + 4] = i * arr_size_y + j + 1;
			indices_1[quad_id * 6 + 3] = (i + 1) * arr_size_y + j;

		}
	}
	//ClothSimulationCompute(mymetadata, Pos, Vel, &ErrMessage);
	for (int i = 0; i < arr_size_x; i++)
	{
		for (int j = 0; j < arr_size_y; j++)
		{
			if ((i / 4 + j / 4) % 2 == 0) uv[i * arr_size_y + j] = FVector2d(0, 0);
			else uv[i * arr_size_y + j] = FVector2d(1, 1);
			vertices[i * arr_size_y + j] = FVector(Pos[3 * (i * arr_size_y + j) ] * 300.0, Pos[3 * (i * arr_size_y + j) + 1] * 300.0, Pos[3 * (i * arr_size_y + j) + 2] * 300.0);
		}
	}

	cloth_mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
	cloth_mesh_1 = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh_1"));
	//SetRootComponent(cloth_mesh);
	static ConstructorHelpers::FObjectFinder<UMaterial> mtlAsset(TEXT("Material'/Game/MyAssets/Meterials/M_ColorGrid_LowSpec.M_ColorGrid_LowSpec'"));

	if (mtlAsset.Succeeded())
	{
		UMaterialInstanceDynamic* mtl = UMaterialInstanceDynamic::Create(mtlAsset.Object, NULL);
		cloth_mesh->SetMaterial(0, mtl);
		cloth_mesh_1->SetMaterial(0, mtl);
	}

	cloth_mesh->CreateMeshSection_LinearColor(0, vertices, indices, normals, uv, emptyColor, emptyTangent, false);
	cloth_mesh_1->CreateMeshSection_LinearColor(0, vertices, indices_1, normals, uv, emptyColor, emptyTangent, false);
}


void AmyTaichiCLSIActor::UpdateMesh()
{
	cloth_mesh->CreateMeshSection_LinearColor(0, vertices, indices, normals, uv, emptyColor, emptyTangent, false);
	cloth_mesh_1->CreateMeshSection_LinearColor(0, vertices, indices_1, normals, uv, emptyColor, emptyTangent, false);
}





