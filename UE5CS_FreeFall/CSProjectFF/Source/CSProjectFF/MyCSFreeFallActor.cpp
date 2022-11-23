// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCSFreeFallActor.h"
#include "CSFree.h"


// Sets default values
AMyCSFreeFallActor::AMyCSFreeFallActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	arr_size = FCSFreeModule::Get().arr_size;
	for (int i = 0; i < arr_size; i++)
	{
		meshPos.Add(FVector3f(0.f, 0.f, 0.f));
	}
	InitInstances(arr_size);
}

// Called when the game starts or when spawned
void AMyCSFreeFallActor::BeginPlay()
{
	Super::BeginPlay();
	FCSFreeModule::Get().UpdateParameters(Range, ParticleRadius, Stiffness, RestitutionCoef, delta_t);
	FCSFreeModule::Get().BeginRendering();
}

void AMyCSFreeFallActor::BeginDestroy()
{
	FCSFreeModule::Get().EndRendering();
	Super::BeginDestroy();
}


// Called every frame
void AMyCSFreeFallActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	meshPos = FCSFreeModule::Get().Positions;

	Flash(arr_size);
}


void AMyCSFreeFallActor::InitInstances(int instanceNum)
{
	//Root staticmeshComponent act as the bound of the UInstancedStaticMeshComponent 
	rootStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("rootStaticMesh"));
	RootComponent = rootStaticMesh;

	for (int i = 0; i < instanceNum; i++)
	{
		UStaticMeshComponent* Mesh;
		FString tmp = "Mesh" + FString::FromInt(i);
		FName name = FName(*tmp);
		Mesh = CreateDefaultSubobject<UStaticMeshComponent>(name);

		static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeVisualAsset(TEXT("/Game/MyAssets/Shape/Shape_Sphere.Shape_Sphere"));
		if (CubeVisualAsset.Succeeded())
		{
			Mesh->SetStaticMesh(CubeVisualAsset.Object);
			Mesh->SetWorldScale3D(FVector(0.2f));
			Mesh->SetWorldLocation(RootComponent->GetRelativeLocation());
		}
		myMesh.Add(Mesh);
	}
}

