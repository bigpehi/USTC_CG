#pragma once

#include <Basic/HeapObj.h>
#include <UHEMesh/HEMesh.h>
#include <UGM/UGM>
#include <Eigen/Sparse>
#include <qdebug.h>
using namespace Eigen;

namespace Ubpa {
	class TriMesh;
	class MinSurf;

	// mesh boundary == 1
	class ParameterizeCotangentCircle : public HeapObj {
	public:
		ParameterizeCotangentCircle(Ptr<TriMesh> triMesh);
	public:
		static const Ptr<ParameterizeCotangentCircle> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<ParameterizeCotangentCircle>(triMesh);
		}
	public:
		void Clear();
		bool Init(Ptr<TriMesh> triMesh);
		bool Run();

		void Parameterization();
		void Fix_boundary();
		void Create_equation();
		void Solve_equation();
		void Update_vetecies();
		void Add_texture();


	protected:
		class V;
		class E;
		class P;
		class V : public TVertex<V, E, P> {
		public:
			vecf3 pos;
		};
		class E : public TEdge<V, E, P> { };
		class P :public TPolygon<V, E, P> { };


	protected:
		Ptr<TriMesh> triMesh;
		const Ptr<HEMesh<V>> heMesh; // vertice order is same with triMesh

		SparseMatrix<double> A_sparse;
		SparseLU<SparseMatrix<double>> solver;
		VectorXd B_x;
		VectorXd B_y;
		VectorXd B_z;
		VectorXd X_x;
		VectorXd X_y;
		VectorXd X_z;

		VectorXd X_x_backup;
		VectorXd X_y_backup;
		VectorXd X_z_backup;

		int* boundary_indexes;

		bool texture_flag;

	};
}