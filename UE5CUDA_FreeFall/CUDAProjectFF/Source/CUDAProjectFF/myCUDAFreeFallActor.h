// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CUDA_Free.h"
#include "cuda_runtime.h"
#include <string>
#include "myCUDAFreeFallActor.generated.h"

UCLASS()
class CUDAPROJECTFF_API AmyCUDAFreeFallActor : public AActor
{
	GENERATED_BODY()	
public:	
	// Sets default values for this actor's properties
	AmyCUDAFreeFallActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = myTextureTarget)
		class UTextureRenderTarget* myPosTexture;
	//UFUNCTION(BlueprintCallable, Category = "CUDAFreeFall")
	static const int arr_size = THREAD_NUM;
	metadata mymetadata = { 800.f,20.f,float(6e3),float(1e-4),float(5e-4) };
	float3 Pos[arr_size];
	float3 Vel[arr_size];
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "mymetadata")
	float Range = mymetadata.Range;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "mymetadata")
	float ParticleRadius = mymetadata.ParticleRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "mymetadata")
	float Stiffness = mymetadata.Stiffness;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "mymetadata")
	float RestitutionCoef = mymetadata.RestitutionCoef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "mymetadata")
	float delta_t = mymetadata.delta_t;

	UPROPERTY(BlueprintReadOnly)
	TArray<FVector> MeshPos;

	//cudaError_t t_cudaStatus;
	std::string ErrMessage;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	
	TArray<FColor> myColorBuffer;
	class FTextureRenderTarget2DResource* textureResource; 
	// 更合适的做法是再使用一个pixel shader读取TArray的MeshPos，然后输出到RenderTargetResource
	// 测试方法是将Mesh数据直接写入StaticMesh




public:
	// for cuda test
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CUDAFree")
		class UStaticMeshComponent* rootStaticMesh;
	TArray<class UStaticMeshComponent*> myMesh;
	void InitInstances(int instatnceNum);
	void Flash(int instanceNum){
		for (int i = 0; i < instanceNum; i++)
		{
			myMesh[i]->SetRelativeLocation(MeshPos[i]);
		}
	}
};
