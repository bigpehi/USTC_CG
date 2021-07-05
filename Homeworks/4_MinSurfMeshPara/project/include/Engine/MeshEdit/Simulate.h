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
			this->positions_backup.resize(plist.size());
			for (int i = 0; i < plist.size(); i++)
			{
				for (int j = 0; j < 3; j++)
				{
					this->positions[i][j] = plist[i][j];
					this->positions_backup[i][j] = plist[i][j];
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


	private:
		// kernel part of the algorithm
		void SimulateOnce();
		//void Generate_equation();
		//void Compute_equation();
		void ImplicitEuler();

	private:
		double h = 0.01f;  //步长
		double stiff = 1;
		std::vector<unsigned> fixed_id;  //fixed point id
		double mass = 1;


		//mesh data
		std::vector<unsigned> edgelist; // 存储网格中的边，[i]为一条边的起点，[i+1]为一条边的终点
		std::vector<double> edge_rest; // 每条边自然长度

		//simulation data
		std::vector<pointf3> positions; // 所有质点的位置
		std::vector<pointf3> positions_backup; // 所有质点的位置
		std::vector<pointf3> velocity;  // 所有质点的速度
		std::vector<pointf3> velocity_backup;  // 所有质点的速度


		/*SparseMatrix<double> coeffsM_nabla_gxk;*/
		//SimplicialLDLT<SparseMatrix<double>> solver;
		//VectorXd b_gx; // 方程组Ax = b 中的b，n*3
		//Matrix<double, -1, 3> Y;
		//VectorXd X_k;
		//VectorXd X_kplus1;
		/*Matrix<double, -1, 3> f_int;*/
	};
}
