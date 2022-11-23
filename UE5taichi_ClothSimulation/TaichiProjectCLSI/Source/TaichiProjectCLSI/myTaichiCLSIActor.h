// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "taichi/cpp/taichi.hpp"
#include "myTaichiCLSIActor.generated.h"


namespace {
	void check_taichi_error(const std::string& msg) {
		TiError error = ti_get_last_error(0, nullptr);
		if (error < TI_ERROR_SUCCESS) {
			throw std::runtime_error(msg);
		}
	}
}

struct TaichiCLSIModule {
	static const unsigned int Thread_Num = 128;
	ti::Runtime my_runtime;
	ti::AotModule my_module;
	ti::ComputeGraph my_g_demo;
	ti::NdArray<float> Vel;
	ti::NdArray<float> Pos;

public:
	TaichiCLSIModule() {
		my_runtime = ti::Runtime(TI_ARCH_VULKAN);
		FString aot_file_dir = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("MyAssets/Taichi_clsi_AOT"));
		std::string temp_dir(TCHAR_TO_UTF8(*aot_file_dir));
		my_module = my_runtime.load_aot_module(temp_dir);
		check_taichi_error("runtime failed");
		my_g_demo = my_module.get_compute_graph("MainCompute");
		check_taichi_error("module failed");
		Vel = my_runtime.allocate_ndarray<float>({ Thread_Num * Thread_Num }, {3}, true);
		Pos = my_runtime.allocate_ndarray<float>({ Thread_Num * Thread_Num }, {3}, true);
		check_taichi_error("allocate failed");
		UE_LOG(LogTemp, Log, TEXT("TAICHI_MODULE_Initialized"));
	}
	void compute(std::vector<float>& vel, std::vector<float>& pos) {
		//void compute(std::vector<float>& pos) {
		Vel.write(vel);
		Pos.write(pos);
		my_g_demo["Vel"] = Vel;
		my_g_demo["Pos"] = Pos;
		for (int i = 0; i < 20; i++)
		{
			my_g_demo.launch();
			my_runtime.wait();
		}


		Vel.read(vel);
		Pos.read(pos);

	}

};




UCLASS()
class TAICHIPROJECTCLSI_API AmyTaichiCLSIActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AmyTaichiCLSIActor();

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



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitInstances();
	void UpdateMesh();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	TaichiCLSIModule myclsimodule;
	std::vector<float> Pos;
	std::vector<float> Vel;

};
