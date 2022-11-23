// Fill out your copyright notice in the Description page of Project Settings.


#include "myInstanceMeshActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AmyInstanceMeshActor::AmyInstanceMeshActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Root staticmeshComponent act as the bound of the UInstancedStaticMeshComponent 
	rootStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("rootStaticMesh"));
	RootComponent = rootStaticMesh;

	InstancedComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("UInstancedStaticMeshComponent"));
	InstancedComponent->SetupAttachment(rootStaticMesh);

	//set the number of custom data needed, close default collision for using our compute result
	InstancedComponent->NumCustomDataFloats = 1;
	InstancedComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InitInstances(instance_num);
}

// Called when the game starts or when spawned
void AmyInstanceMeshActor::BeginPlay()
{
	Super::BeginPlay();
	
	// 使用Pixel Shader纹理结果绘制
	//if (DrawMat != nullptr)
	//{
	//	DrawMatInstance = InstancedComponent->CreateDynamicMaterialInstance(0, DrawMat);
	//	InitInstances(instance_num);
	//	UE_LOG(LogTemp, Warning, TEXT("InitInstances!"));
	//}
	Flash(instance_num);
}

// Called every frame
void AmyInstanceMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Flash(instance_num);

}

void AmyInstanceMeshActor::InitInstances(int instanceNum)
{
	InstancedComponent->ClearInstances();
	//create instances
	// 使用Pixel Shader纹理结果绘制
	//for (int i = 0; i < instatnceNum; i++)
	//{
	//	FTransform t = FTransform();
	//	//for set initial position: t.SetLocation(FVector(FMath::RandRange(-100, 100) * i, FMath::RandRange(-1024, 1024), FMath::RandRange(-1024, 1024)));
	//	InstancedComponent->AddInstance(t);
	//	InstancedComponent->SetCustomDataValue(i, 0, i);
	//	UE_LOG(LogTemp, Warning, TEXT("AddInstance %d"), i);
	//}
	for (int i = 0; i < instanceNum; i++)
	{
		UStaticMeshComponent* Mesh;
		FString tmp = "Mesh" + FString::FromInt(i);
		FName name = FName(*tmp);
		Mesh = CreateDefaultSubobject<UStaticMeshComponent>(name);

		static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeVisualAsset(TEXT("/Game/MyAssets/Shape_Sphere.Shape_Sphere"));
		if (CubeVisualAsset.Succeeded())
		{
			Mesh->SetStaticMesh(CubeVisualAsset.Object);
			Mesh->SetWorldScale3D(FVector(0.1f));
			//Mesh->SetWorldLocation( RootComponent->GetRelativeLocation());
		}
		myMesh.Add(Mesh);
		myPos.Add(FVector(0.f,0.f,0.f));
	}

}

void AmyInstanceMeshActor::Flash(int instanceNum)
{
	for (int i = 0; i < instanceNum; i++)
	{
		myMesh[i]->SetRelativeLocation(myPos[i]);
	}
}

