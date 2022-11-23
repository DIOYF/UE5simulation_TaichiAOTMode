// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "taichi/cpp/taichi.hpp"
#include "GameFramework/Actor.h"
#include "MyTaichiFreeFallActor.generated.h"

namespace {
	void check_taichi_error(const std::string& msg) {
		TiError error = ti_get_last_error(0, nullptr);
		if (error < TI_ERROR_SUCCESS) {
			throw std::runtime_error(msg);
		}
	}
}

struct TaichiFreeFallModule {
	static const unsigned int Thread_Num = 1024;
	ti::Runtime my_runtime;
	ti::AotModule my_module;
	ti::ComputeGraph my_g_demo;
	ti::NdArray<float> Vel;
	ti::NdArray<float> Pos;

public:
	TaichiFreeFallModule() {
		my_runtime = ti::Runtime(TI_ARCH_VULKAN);
		FString aot_file_dir = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("MyAssets/Taichi_Free_AOT"));
		std::string temp_dir(TCHAR_TO_UTF8(*aot_file_dir));
		my_module = my_runtime.load_aot_module(temp_dir);
		check_taichi_error("runtime failed");
		my_g_demo = my_module.get_compute_graph("MainCompute");
		check_taichi_error("module failed");
		Vel = my_runtime.allocate_ndarray<float>({ Thread_Num }, { 3 }, true);
		Pos = my_runtime.allocate_ndarray<float>({ Thread_Num }, { 3 }, true);
		check_taichi_error("allocate failed");
		UE_LOG(LogTemp, Log, TEXT("TAICHI_MODULE_Initialized"));
	}
	/*
	void init(std::vector<float>& vel, std::vector<float>& pos) {
	
		Vel.write(vel);
		Pos.write(pos);
		my_g_demo["Vel"] = Vel;
		my_g_demo["Pos"] = Pos;
		UE_LOG(LogTemp, Log, TEXT("Vel:{%4f}{%4f}{%4f}"), vel[0], vel[1], vel[2]);
	}
	*/
	void compute(std::vector<float>& vel, std::vector<float>& pos) {
	//void compute(std::vector<float>& pos) {
		Vel.write(vel);
		Pos.write(pos);
		my_g_demo["Vel"] = Vel;
		my_g_demo["Pos"] = Pos;
		my_g_demo.launch();
		my_runtime.wait();
		Vel.read(vel);
		Pos.read(pos);

	}
	
};



UCLASS()
class TAICHIPROJECTFF_API AMyTaichiFreeFallActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyTaichiFreeFallActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CSFree")
		class UStaticMeshComponent* rootStaticMesh;
	
	TArray<class UStaticMeshComponent*> myMesh;

	static const int arr_size = 1024;

	void InitInstances(int instatnceNum);
	void Flash(int instanceNum) {
		for (int i = 0; i < instanceNum; i++)
		{
			myMesh[i]->SetRelativeLocation(FVector(positions[3*i],positions[3*i+1],positions[3*i+2]));
		}
	}

public:
	TaichiFreeFallModule myTaichiModule;
	// We use std::vector pos and vel for transiton with Taichi AOT Module API   
	std::vector<float> positions;
	std::vector<float> velocities;

};


