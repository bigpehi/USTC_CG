#pragma once

#include <Basic/HeapObj.h>
//#include <Engine/Primitive/MassSpring.h>
#include <algorithm>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/src/SparseCholesky/SimplicialCholesky.h>
#include <Eigen/src/SparseLU/SparseLU.h>
#include <vector>
#include <UGM/UGM>
using namespace Ubpa;
using namespace Eigen;

namespace Ubpa {
	class Simulate : public HeapObj {
	public:
		Simulate(const std::vector<pointf3>& plist,
			const std::vector<unsigned>& elist) {
			std::cout << "simulate creater" << std::endl;
			edgelist = elist;
			// 初始化position
			this->positions.resize(plist.size());
			for (int i = 0; i < plist.size(); i++)
			{
				for (int j = 0; j < 3; j++)
				{
					this->positions[i][j] = plist[i][j];
				}
			}
		};
	public:
		static const Ptr<Simulate> New(const std::vector<pointf3>& plist,
			const std::vector<unsigned>& elist) {
			return Ubpa::New<Simulate>(plist, elist);
		}
	public:
		// clear cache data
		void Clear();

		// init cache data (eg. half-edge structure) for Run()
		bool Init();
		//bool Init();

		// call it after Init()
		bool Run();

		const std::vector<pointf3>& GetPositions() const { return positions; };

		const float GetStiff() { return stiff; };
		void SetStiff(float k) { stiff = k; Init(); };
		const float GetTimeStep() { return h; };
		void SetTimeStep(float k) { h = k; Init(); };
		std::vector<unsigned>& GetFix() { return this->fixed_id; };
		void SetFix(const std::vector<unsigned>& f) { this->fixed_id = f; Init(); };
		const std::vector<pointf3>& GetVelocity() { return velocity; };
		//void SetVelocity(const std::vector<pointf3>& v) { velocity = v; };

		void SetLeftFix();
		void Accelerate_on();


	private:
		// kernel part of the algorithm
		void SimulateOnce();
		void ImplicitEuler();
		void Accelerate_liu();
		void Generate_global_M2hL();
		void Generate_global_coeffsMat();
		void Generate_J();

	private:
		double h = 0.03f;  //步长
		double stiff = 1;
		std::vector<unsigned> fixed_id;  //fixed point id
		double mass = 1;


		//mesh data
		std::vector<unsigned> edgelist; // 存储网格中的边，[i]为一条边的起点，[i+1]为一条边的终点
		std::vector<double> edge_rest; // 每条边自然长度

		//simulation data
		std::vector<pointf3> positions; // 所有质点的位置
		std::vector<pointf3> velocity;  // 所有质点的速度

		// 加速算法
		SparseMatrix<double> global_M2hL; // global阶段的矩阵，global_coeffs_Mat = (M +（h^2）* L)
		SparseMatrix<double> global_coeffsMat; // global阶段的矩阵，global_coeffs_Mat = (M +（h^2）* L)
		SparseLU<Eigen::SparseMatrix<double>> solver_coeffsMat;
		SparseMatrix<double> J;
		VectorXd b;
		SparseMatrix<double> K;

		bool is_accelrating;
	};
}
