// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyCSFreeFallActor.generated.h"

UCLASS()
class CSPROJECTFF_API AMyCSFreeFallActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyCSFreeFallActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "myCSmetadata")
		float Range = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "myCSmetadata")
		float ParticleRadius = 20;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "myCSmetadata")
		float Stiffness = 6000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "myCSmetadata")
		float RestitutionCoef = 0.001;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "myCSmetadata")
		float delta_t = 5e-4;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CSFree")
		class UStaticMeshComponent* rootStaticMesh;
	TArray<class UStaticMeshComponent*> myMesh;
	TArray<FVector3f> meshPos;
	int arr_size;


	void InitInstances(int instatnceNum);
	void Flash(int instanceNum) {
		for (int i = 0; i < instanceNum; i++)
		{
			myMesh[i]->SetRelativeLocation(FVector(meshPos[i]));
		}
	}

};
