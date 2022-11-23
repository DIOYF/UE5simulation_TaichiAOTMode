// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CUDAClsi.h"
#include "cuda_runtime.h"
#include <string>
#include "ProceduralMeshComponent.h"
#include "myCUDAclsiActor.generated.h"

UCLASS()
class CUDAPROJECTCLSI_API AmyCUDAclsiActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AmyCUDAclsiActor();

	metadata mymetadata;
	float3 Pos[THREAD_NUM_X * THREAD_NUM_Y];
	float3 Vel[THREAD_NUM_X * THREAD_NUM_Y];

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "mymetadata")
		float spring_K = mymetadata.spring_K;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "mymetadata")
		float kinetic_damping = mymetadata.kinetic_damping;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "mymetadata")
		float drag_damping = mymetadata.drag_damping;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "mymetadata")
		float delta_t = mymetadata.delta_t;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "mymetadata")
		float wind_strength = mymetadata.wind_strength;

	UPROPERTY(VisibleAnywhere)
		UProceduralMeshComponent* cloth_mesh;
	UPROPERTY(VisibleAnywhere)
		UProceduralMeshComponent* cloth_mesh_1;

	TArray<FVector> vertices;
	TArray<FVector2d> uv;
	TArray<int32> indices;
	TArray<int32> indices_1;
	TArray<FVector> normals;
	TArray<FLinearColor> emptyColor;
	TArray<FProcMeshTangent> emptyTangent;
	const int arr_size_x = 128;
	const int arr_size_y = 128;

	//cudaError_t t_cudaStatus;
	std::string ErrMessage;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitInstances();
	void UpdateMesh();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
