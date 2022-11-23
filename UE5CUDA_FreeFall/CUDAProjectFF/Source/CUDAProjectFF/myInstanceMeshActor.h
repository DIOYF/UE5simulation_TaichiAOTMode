// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "myInstanceMeshActor.generated.h"

UCLASS()
class CUDAPROJECTFF_API AmyInstanceMeshActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AmyInstanceMeshActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//class UMaterialInstanceDynamic* DrawMatInstance;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Flash(int instatnceNum);
	class UMaterialInstanceDynamic* DrawMatInstance;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CUDAFree")
		class UInstancedStaticMeshComponent* InstancedComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CUDAFree")
		class UStaticMeshComponent* rootStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CUDAFree")
		class UMaterial* DrawMat;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CUDAFree")
	//	class UMaterial* DrawMat;  // for colorful ball and its position
	TArray<class UStaticMeshComponent*> myMesh;

	UPROPERTY(BlueprintReadWrite, Category = "myPos")
		TArray<FVector> myPos;

	UFUNCTION(BlueprintCallable)
		void InitInstances(int instatnceNum);

private:
	int instance_num = 1024;

};
