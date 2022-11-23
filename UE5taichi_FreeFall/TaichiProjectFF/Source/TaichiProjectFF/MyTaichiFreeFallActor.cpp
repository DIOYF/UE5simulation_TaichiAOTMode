// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTaichiFreeFallActor.h"


// Sets default values
AMyTaichiFreeFallActor::AMyTaichiFreeFallActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	positions.resize(arr_size * 3, 0.0f);
	velocities.resize(arr_size * 3, 0.0f);
	for (int i = 0; i < arr_size; i++)
	{
		positions[3 * i] = FMath::RandRange(-200.f,200.f);
		positions[3 * i + 1] = FMath::RandRange(-200.f, 200.f);
		positions[3 * i + 2] = FMath::RandRange(100.f, 400.f);
		velocities[3 * i] = FMath::RandRange(-0.1f, 0.1f);
		velocities[3 * i + 1] = FMath::RandRange(-0.1f, 0.1f);
		velocities[3 * i + 2] = FMath::RandRange(-0.1f, 0.1f);
	}
	InitInstances(arr_size);
	//UE_LOG(LogTemp, Log, TEXT("TAICHI_MODULE_Loading"));
	//myTaichiModule.runtime = ti::Runtime(TI_ARCH_VULKAN);
	//module = runtime.load_aot_module("/Game/MyAssets/Taichi_Free_AOT");
	//g_demo = module.get_compute_graph("MainCompute");
	//Vel = runtime.allocate_ndarray<float>({ arr_size }, { 3 }, true);
	//Pos = runtime.allocate_ndarray<float>({ arr_size }, { 3 }, true);
	//UE_LOG(LogTemp, Log, TEXT("TAICHI_MODULE_Initialized"));
}

// Called when the game starts or when spawned
void AMyTaichiFreeFallActor::BeginPlay()
{
	Super::BeginPlay();
	//myTaichiModule.init(velocities, positions);
}

// Called every frame
void AMyTaichiFreeFallActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	myTaichiModule.compute(velocities, positions);
	Flash(arr_size);
}


void AMyTaichiFreeFallActor::InitInstances(int instanceNum)
{
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
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		myMesh.Add(Mesh);
	}
}
