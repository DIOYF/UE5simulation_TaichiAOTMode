// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCSClsiActor.h"
#include "CSclsi.h"

// Sets default values
AMyCSClsiActor::AMyCSClsiActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	arr_size_x = FCSclsiModule::Get().arr_size_x;
	arr_size_y = FCSclsiModule::Get().arr_size_y;
	positions = FCSclsiModule::Get().Positions;
	
	FCSclsiModule::Get().UpdateParameters(spring_K, kinetic_damping, drag_damping, delta_t, wind_strength);
	
	vertices.Init(FVector(0.,0.,0.), arr_size_x * arr_size_y);
	indices.Init(0, (arr_size_x - 1) * (arr_size_y - 1) * 6);
	indices_1.Init(0, (arr_size_x - 1) * (arr_size_y - 1) * 6);
	uv.Init(FVector2d(0, 0), arr_size_x * arr_size_y);
	for (int i = 0; i < arr_size_x-1; i++)
	{
		for (int j = 0; j < arr_size_y-1; j++)
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

	for (int i = 0; i < arr_size_x; i++)
	{
		for (int j = 0; j < arr_size_y; j++)
		{
			if ((i/4 + j/4)%2 == 0) uv[i * arr_size_y + j] = FVector2d(0, 0);
			else uv[i * arr_size_y + j] = FVector2d(1, 1);
			
		}
	}




	InitInstances();
}


// Called when the game starts or when spawned
void AMyCSClsiActor::BeginPlay()
{
	Super::BeginPlay();
	FCSclsiModule::Get().UpdateParameters(spring_K, kinetic_damping, drag_damping, delta_t, wind_strength);
	FCSclsiModule::Get().BeginRendering();

}


// Called when the game end or when spawned
void AMyCSClsiActor::BeginDestroy()
{
	FCSclsiModule::Get().EndRendering();
	Super::BeginDestroy();

}

// Called every frame
void AMyCSClsiActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	positions = FCSclsiModule::Get().Positions;
	for (int i = 0; i < arr_size_x; i++)
	{
		for (int j = 0; j < arr_size_y; j++)
		{
			//UE_LOG(LogTemp, Warning, TEXT("positions[%d][%d]:{%4f,%4f,%4f}"), i, j, positions[i * arr_size_y + j].X, positions[i * arr_size_y + j].Y, positions[i * arr_size_y + j].Z);
			vertices[i * arr_size_y + j] = FVector(positions[i * arr_size_y + j].X * 300, positions[i * arr_size_y + j].Y * 300, positions[i * arr_size_y + j].Z * 300);
		}
	}
	
	UpdateMesh();
}


void AMyCSClsiActor::InitInstances()
{
	rootStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("rootStaticMesh"));
	RootComponent = rootStaticMesh;


	_mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
	_mesh_1 = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh_1"));
	//SetRootComponent(_mesh);
	static ConstructorHelpers::FObjectFinder<UMaterial> mtlAsset(TEXT("Material'/Game/myAssets/materials/M_ColorGrid_LowSpec.M_ColorGrid_LowSpec'"));

	if (mtlAsset.Succeeded())
	{
		UMaterialInstanceDynamic* mtl = UMaterialInstanceDynamic::Create(mtlAsset.Object, NULL);
		_mesh->SetMaterial(0, mtl);
		_mesh_1->SetMaterial(0, mtl);
	}


	_mesh->CreateMeshSection_LinearColor(0, vertices, indices, normals, uv, emptyColor, emptyTangent, false);
	_mesh_1->CreateMeshSection_LinearColor(0, vertices, indices_1, normals, uv, emptyColor, emptyTangent, false);
}



void AMyCSClsiActor::UpdateMesh()
{
	_mesh->CreateMeshSection_LinearColor(0, vertices, indices, normals, uv, emptyColor, emptyTangent, false);
	_mesh_1->CreateMeshSection_LinearColor(0, vertices, indices_1, normals, uv, emptyColor, emptyTangent, false);
}


