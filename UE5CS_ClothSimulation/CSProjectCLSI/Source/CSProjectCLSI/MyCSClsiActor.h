// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "MyCSClsiActor.generated.h"

UCLASS()
class CSPROJECTCLSI_API AMyCSClsiActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyCSClsiActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "myCSmetadata")
		float spring_K = 3e4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "myCSmetadata")
		float kinetic_damping = 1e4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "myCSmetadata")
		float drag_damping = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "myCSmetadata")
		float delta_t = 6e-3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "myCSmetadata")
		float wind_strength = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CSFree")
		class UStaticMeshComponent* rootStaticMesh;

	UPROPERTY(VisibleAnywhere)
		UProceduralMeshComponent* _mesh;

	UPROPERTY(VisibleAnywhere)
		UProceduralMeshComponent* _mesh_1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;


	void InitInstances();
	void UpdateMesh();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	TArray<FVector3f> positions;
	TArray<FVector> vertices;
	TArray<FVector2d> uv;
	TArray<int32> indices;
	TArray<int32> indices_1;

	TArray<FVector> normals;
	TArray<FLinearColor> emptyColor;
	TArray<FProcMeshTangent> emptyTangent;
	int arr_size_x;
	int arr_size_y;

};
