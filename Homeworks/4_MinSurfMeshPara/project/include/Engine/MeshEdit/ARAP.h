#pragma once

#include <Engine/MeshEdit/ParameterizeUniformCircle.h>
#include <Engine/MeshEdit/ParamaterizeUniformSquare.h>
#include <Engine/MeshEdit/ParameterizeCotangentSquare.h>
#include <Engine/MeshEdit/CParameterize.h>
#include <Basic/HeapObj.h>
#include <UHEMesh/HEMesh.h>
#include <Eigen/Sparse>
#include <Eigen/SparseCholesky>
#include <UGM/UGM>
#include <qdebug.h>
#include <Eigen/SVD>

namespace Ubpa {
	class TriMesh;
	class Paramaterize;

	class ARAP : public HeapObj {
	public:
		ARAP(Ptr<TriMesh> triMesh);
	public:
		static const Ptr<ARAP> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<ARAP>(triMesh);
		}

	private:
		class V;
		class E;
		class P;
		class V : public TVertex<V, E, P> {
		public:
			vecf3 pos;
		};
		class E : public TEdge<V, E, P> { };
		class P :public TPolygon<V, E, P> { };


	public:
		// clear cache data
		void Clear();
		// init cache data (eg. half-edge structure) for Run()
		bool Init(Ptr<TriMesh> triMesh);
		// call it after Init()
		bool Run();
		void Add_texture();


	private:
		// kernel part of the algorithm
		void ARAP_Pipeline();
		void Global_Phase();
		void Local_Phase();
		void Generate_and_compute_A();

		double getCotan(V* v1, V* v2, V* v3);/* Get cotan of angle of edge v1v2 in triangle v1v2v3 */
		double getCotan(int tri_idx, V* v);/* get the angle of edge i, i+1. idx is V_{(i+2) %3} */
		void CongruentMapping2D(); // Congruently Map triangles on 3D mesh to 2D, return: vector of new vertrixes
		void setb(size_t idx1, pointf2 pos1, size_t idx2, pointf2 pos2);





	private:
		friend class Paramaterize;

		Ptr<TriMesh> triMesh;
		const Ptr<HEMesh<V>> heMesh; // vertice order is same with triMesh

		// point2D 是一个vector，其中包含nV个map，每个map表示第i个三角形的三个点对应的局部坐标，局部坐标的最后一位为0
		std::vector<std::map<V*, pointf3>> points2d; // 2d points mapped from 3D
		std::vector<std::map<V*, double>> cot2d; // cotan value of origin 3D mesh

		size_t anchor_v1_idx = 0;
		size_t anchor_v2_idx = 1;
		pointf2 anchor_pos1;
		pointf2 anchor_pos2;

		size_t nV; // num of vertices
		size_t nT; // num of triangles


		std::vector<Matrix2d> Lt_array;
		std::vector<std::vector<V*>> triangle_points;  // 保存最新的u


		bool texture_flag;
		std::vector<pointf2> texCoor;

		std::vector<Eigen::Triplet<double> > ARAP_coeff;
		SparseMatrix<double> A_sparse;
		SimplicialLDLT<SparseMatrix<double>> solver;

		double error;


		MatrixXd ARAP_mat_b;
		MatrixXd ARAP_solution;

	};
}