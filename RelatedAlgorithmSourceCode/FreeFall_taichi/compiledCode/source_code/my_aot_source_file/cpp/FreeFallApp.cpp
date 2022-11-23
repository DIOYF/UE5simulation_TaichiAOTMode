#include <cmath>
#include <iostream>
#include <numeric>
#include <taichi/cpp/taichi.hpp>

namespace {
	void check_taichi_error(const std::string& msg) {
		TiError error = ti_get_last_error(0, nullptr);
		if (error < TI_ERROR_SUCCESS) {
			throw std::runtime_error(msg);
		}
	}
}


struct FreeFall {
	static const uint32_t Thread_Num = 1024 ;
	//float Range = 800;
	//float ParticleRadius = 20;
	//float Stiffness = 6e3;
	//float RestitutionCoef = 1e-4;
	//float delta_t = 5e-4;

	ti::Runtime runtime_;
	ti::AotModule module_;
	ti::ComputeGraph g_demo_;
	ti::NdArray<float> Vel;
	ti::NdArray<float> Pos;

	FreeFall() {
		runtime_ = ti::Runtime(TI_ARCH_VULKAN);
		module_ = runtime_.load_aot_module("./Taichi_Free_AOT");
		check_taichi_error("runtime failed");
		g_demo_ = module_.get_compute_graph("MainCompute");
		check_taichi_error("module failed");
		Vel = runtime_.allocate_ndarray<float>({Thread_Num},{3}, true);
		Pos = runtime_.allocate_ndarray<float>({Thread_Num},{3}, true);
		check_taichi_error("allocate failed");
		std::cout << "Initialized" << std::endl;
	}

	void run() {
		std::vector<float> vel(Thread_Num * 3, 0.1f);
		std::vector<float> pos(Thread_Num * 3, 10.1f);
		
		for (int i = 0; i < Thread_Num * 3; i++)
		{
			pos[i] = i * 1.0f;
		}

		Vel.write(vel);
		Pos.write(pos);
		
		g_demo_["Vel"] = Vel;
		g_demo_["Pos"] = Pos;

		for (int i = 0; i < 20; i++)
		{
			g_demo_.launch();
			runtime_.wait();
			check_taichi_error("cgraph launch failed");
			Vel.read(vel);
			Pos.read(pos);
			for (int j = 0; j < 2; j++)
			{

				std::cout <<i<<"Some Velocities:"<<j << " " << vel[j] << vel[j * 3 + 1] << vel[j * 3 + 2] << std::endl;
				std::cout <<i<< "Some Positions:"<<j << " " << pos[j] << pos[j * 3 + 1] << pos[j * 3 + 2] << std::endl;

			}

		}
	}

};



int main(int argc, const char** argv) {
	FreeFall myFreeFallKernel;
	int count = 0;
	while ( count < 2)
	{
		count++;
		std::cout<<"MYCOUNT"<<count << std::endl;
		myFreeFallKernel.run();
	}
	return 0;


}