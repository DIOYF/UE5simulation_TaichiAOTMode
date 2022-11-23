// Fill out your copyright notice in the Description page of Project Settings.

#include "RHI.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "myCUDAFreeFallActor.h"


// Sets default values
AmyCUDAFreeFallActor::AmyCUDAFreeFallActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	for (int i = 0; i < arr_size; i++)
	{
		MeshPos.Add(FVector(0.f,0.f,0.f));
	}
	InitInstances(arr_size);
}

// Called when the game starts or when spawned
void AmyCUDAFreeFallActor::BeginPlay()
{
	Super::BeginPlay();
	mymetadata.Range = Range;
	mymetadata.ParticleRadius = ParticleRadius;
	mymetadata.Stiffness = Stiffness;
	mymetadata.RestitutionCoef = RestitutionCoef;
	mymetadata.delta_t = delta_t;

	// Not New API for UE5 if (myPosTexture != NULL) textureResource = myPosTexture->GetResource();

	for (int i = 0; i < arr_size ; i++)
	{
		float z = rand()% 100 + Range/2 + 200;
		float y = (float)i/32 * 20 - 400;
		float x = i % 32 * 20 - 400;
		Pos[i] = {x, y, z};
		Vel[i] = { rand()%5 * 0.1f,rand() % 5 * 0.1f , rand() % 5 * 0.1f};
		MeshPos[i] = FVector(Pos[i].x, Pos[i].y, Pos[i].z);
	}
	// we need delete this function if using Pixel Shader
	Flash(arr_size);
}

// Called every frame
void AmyCUDAFreeFallActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FreeFallCompute(mymetadata, Pos, Vel, &ErrMessage);
	for (int i = 0; i < arr_size; i++)
	{
		MeshPos[i] = FVector(Pos[i].x,Pos[i].y,Pos[i].z);
	}
	//if (myPosTexture != NULL)
	//{

	//	if (textureResource->ReadPixels(myColorBuffer))
	//	{
	//	}
	//}

	// we need delete this function if using Pixel Shader
	Flash(arr_size);
}



// we need deltate this function
void AmyCUDAFreeFallActor::InitInstances(int instanceNum)
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
			Mesh->SetWorldScale3D(FVector(0.3f));
			//Mesh->SetWorldLocation( RootComponent->GetRelativeLocation());
		}
		myMesh.Add(Mesh);
	}
}

