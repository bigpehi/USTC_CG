#include <Engine/MeshEdit/ARAP.h>
#include <Engine/MeshEdit/CParameterize.h>
#include <Engine/Primitive/TriMesh.h>
#include <assert.h>
#include <Eigen/Sparse>

#include <Engine/MeshEdit/Paramaterize.h>

using namespace Ubpa;
using namespace std;
using namespace Eigen;

Ubpa::ARAP::ARAP(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
	texture_flag = false;
}


void Ubpa::ARAP::Clear()
{
	heMesh->Clear();

	triMesh = nullptr;
}

bool Ubpa::ARAP::Init(Ptr<TriMesh> triMesh)
{
	Clear();
	if (triMesh == nullptr)
		return true;
	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::ARAP::Init:\n"
			"\t""trimesh is invalid\n");
		return false;
	}

	this->nT = triMesh->GetTriangles().size();
	this->nV = triMesh->GetPositions().size();

	// step1: init half-edge structure
	vector<vector<size_t>> triangles;
	triangles.reserve(triMesh->GetTriangles().size());
	for (auto triangle : triMesh->GetTriangles())
		triangles.push_back({ triangle->idx[0], triangle->idx[1], triangle->idx[2] });
	heMesh->Reserve(this->nV);
	heMesh->Init(triangles);
	if (!heMesh->IsTriMesh() || !heMesh->HaveBoundary()) {
		printf("ERROR::MinSurf::Init:\n"
			"\t""trimesh is not a triangle mesh or hasn't a boundaries\n");
		heMesh->Clear();
		return false;
	}
	for (int i = 0; i < this->nV; i++) {
		auto v = heMesh->Vertices().at(i);
		v->pos = triMesh->GetPositions()[i].cast_to<vecf3>();
	}

	this->triMesh = triMesh;

	// step2: initializing the points2d and cotan 
	// (locally coordinates and cotangent of every angle for every triangle)
	CongruentMapping2D();

	// step3: locally parameterization with boundry
	auto parameterizeUniformCircle = ParameterizeUniformCircle::New(triMesh);
	if (parameterizeUniformCircle->Run()) {
		printf("paramaterizeUniformSquare done\n");
	}

	// step4: pick two anchor vertexes, 
	// which can (a) remove ratation and displacement of the result (b) avoid  degradation solution.
	auto triangle = heMesh->Polygons().back();
	auto v1 = triangle->BoundaryVertice()[0];
	anchor_v1_idx = heMesh->Index(v1);
	auto v2 = triangle->BoundaryVertice()[1];
	anchor_v2_idx = heMesh->Index(v2);
	anchor_pos1 = pointf2(0, 0);
	anchor_pos2 = pointf2(2, 2);

	// step5: generate and compute the coefficient matrix A, 
	// which is constant in all iterations, so it can be generated and computed only once.
	Generate_and_compute_A();

	return true;
}

bool Ubpa::ARAP::Run()
{
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	error = -1;
	ARAP_Pipeline(); // kernel iteration of the ARAP algorithm

	// Finally, half-edge structure -> triangle mesh, end. 
	vector<pointf3> positions;
	vector<unsigned> indice;
	vector<pointf2> texcoords;

	this->nV = heMesh->NumVertices();
	const std::vector<Ptr<Ubpa::Triangle>>& triangles = triMesh->GetTriangles();
	this->nT = triangles.size();

	positions.reserve(this->nV);
	indice.reserve(3 * this->nT);
	texcoords.reserve(this->nV);

	for (auto v : heMesh->Vertices()) {
		positions.push_back(v->pos.cast_to<pointf3>());
		texcoords.push_back(v->pos.cast_to<pointf2>());
	}
	for (auto f : heMesh->Polygons()) { // f is triangle
		for (auto v : f->BoundaryVertice()) // vertices of the triangle
			indice.push_back(static_cast<unsigned>(heMesh->Index(v)));
	}

	if (this->texture_flag)
		this->triMesh->Update(texcoords);
	else
		this->triMesh->Update(positions);
	return true;
}

void Ubpa::ARAP::Add_texture()
{
	texture_flag = true;

}

void Ubpa::ARAP::ARAP_Pipeline()
{

	// stop condition
	size_t iter_n = 2;

	for (int i = 0; i < iter_n; i++)
	{
		Local_Phase();
		Global_Phase();
		cout << "iter:" << i << " error: " << error << endl;
	}
}

void Ubpa::ARAP::Global_Phase()
{
	// step1: set b
	setb(anchor_v1_idx, anchor_pos1, anchor_v2_idx, anchor_pos2);


	// Then solve the equation
	ARAP_solution = solver.solve(ARAP_mat_b);

	this->texCoor.clear();
	for (size_t i = 0; i < this->nV; i++)
	{
		double dist = pointf3::distance(pointf3(ARAP_solution(i, 0), ARAP_solution(i, 1), 0.0), heMesh->Vertices()[i]->pos.cast_to<pointf3>());
		if (error < 0)
		{
			error = dist;
		}
		else if (error < dist)
		{
			error = dist;
		}

		heMesh->Vertices()[i]->pos[0] = ARAP_solution(i, 0);
		heMesh->Vertices()[i]->pos[1] = ARAP_solution(i, 1);
		heMesh->Vertices()[i]->pos[2] = 0.0;
		// update tex coordinates
		texCoor.push_back(pointf2(ARAP_solution(i, 0), ARAP_solution(i, 1)));
	}
}


void Ubpa::ARAP::Local_Phase() // 使用SVD分解求解最优的Lt
{
	// Get newest u per iteration ,
	triangle_points.clear();	// remember to clear
	for (auto triangle : heMesh->Polygons())
	{
		if (triangle != nullptr)
		{
			this->triangle_points.push_back(triangle->BoundaryVertice());
		}
	}

	// 构造St(u)
	Lt_array.clear(); // 后面更新Lt
	for (size_t t = 0; t < this->nT; t++)
	{
		auto vec_u = triangle_points[t];
		auto mapped_u = points2d[t];
		Matrix2d St;
		St.setZero();

		// get St
		for (int i = 0; i < 3; i++)
		{
			V* u0 = vec_u[i];
			V* u1 = vec_u[(i + 1) % 3];
			MatrixXd delta_u(2, 1);
			delta_u <<
				u0->pos[0] - u1->pos[0], u0->pos[1] - u1->pos[1];

			pointf3 x0 = mapped_u[u0];
			pointf3 x1 = mapped_u[u1];
			MatrixXd delta_x(2, 1);
			delta_x <<
				x0[0] - x1[0], x0[1] - x1[1];

			double cot = getCotan(t, vec_u[(i + 2) % 3]);

			St += cot * delta_u * delta_x.transpose();
		}

		// Do SVD Composition on St
		JacobiSVD<MatrixXd> svd(St, ComputeThinU | ComputeThinV);
		Matrix2d Lt = svd.matrixU() * svd.matrixV().transpose(); // Lt = U * V^T

		Lt_array.push_back(Lt);

	}
}

void Ubpa::ARAP::Generate_and_compute_A()
{
	// Two Anchor points 
	A_sparse.resize(this->nV, this->nV);
	ARAP_coeff.push_back(Eigen::Triplet<double>(anchor_v1_idx, anchor_v1_idx, 1));
	//ARAP_coeff.push_back(Eigen::Triplet<double>(anchor_v2_idx, anchor_v2_idx, 1));

	// Other non-anchor points
	for (size_t i = 0; i < nV; i++)
	{
		if (i != anchor_v1_idx /*&& i != anchor_v2_idx*/) {
			auto v = heMesh->Vertices()[i];
			double cotan_sum = 0.0;

			// traverse adjacent vertrices
			for (auto adj_v : v->AdjVertices())
			{
				size_t adj_idx = heMesh->Index(adj_v);

				// To get cotan 
				auto e = v->EdgeWith(adj_v);
				auto he1 = e->HalfEdge();

				double cot1 = 0.0;
				if (he1->Polygon() != nullptr)
				{
					auto tri1_idx = heMesh->Index(he1->Polygon()); // get index of adjacent triangle
					try
					{
						auto tri_v1 = he1->Next()->End(); // get vertix of adjacent triangle 
						assert(heMesh->Index(tri_v1) != heMesh->Index(v)
							&& heMesh->Index(tri_v1) != heMesh->Index(adj_v));

						cot1 = getCotan(tri1_idx, tri_v1);
					}
					catch (const std::exception& e)
					{
						cout << "cannot find cot 1 in map. " << endl;
					}
				}
				auto he2 = e->HalfEdge()->Pair();

				double cot2 = 0.0;
				if (he2->Polygon() != nullptr)
				{
					auto tri2_idx = heMesh->Index(he2->Polygon());
					try
					{
						auto tri_v2 = he2->Next()->End();
						assert(heMesh->Index(tri_v2) != heMesh->Index(v)
							&& heMesh->Index(tri_v2) != heMesh->Index(adj_v));
						cot2 = getCotan(tri2_idx, tri_v2);
					}
					catch (const std::exception& e)
					{
						cout << "cannot find cot 2 in map. " << endl;
					}
				}

				ARAP_coeff.push_back(Eigen::Triplet<double>(i, adj_idx, -(cot1 + cot2)));

				cotan_sum += (cot1 + cot2);
			}

			ARAP_coeff.push_back(Eigen::Triplet<double>(i, i, cotan_sum));
		}
	}
	A_sparse.setFromTriplets(ARAP_coeff.begin(), ARAP_coeff.end());
	// pre-computation
	solver.compute(A_sparse);
	if (solver.info() != Success) {
		qDebug() << "decomposition failed" << endl;
	}

}




void ARAP::CongruentMapping2D()
{
	this->points2d.clear();
	this->cot2d.clear();
	for (auto triangle : this->heMesh->Polygons())
	{
		if (triangle != nullptr) {
			map<V*, pointf3> map_v_2d;

			auto v0 = triangle->BoundaryVertice()[0];
			auto v1 = triangle->BoundaryVertice()[1];
			auto v2 = triangle->BoundaryVertice()[2];

			map_v_2d[v0] = pointf3(0, 0, 0); // set new v0 = (0,0,0)

			// get 2d coordinates
			double dist01 = pointf3::distance(v0->pos.cast_to<pointf3>(), v1->pos.cast_to<pointf3>());
			double dist02 = pointf3::distance(v0->pos.cast_to<pointf3>(), v2->pos.cast_to<pointf3>());
			double cos01_02 = vecf3::cos_theta((v1->pos - v0->pos), (v2->pos - v0->pos)); // angle of edge 01 and 02
			double sin01_02 = sqrt(1 - cos01_02 * cos01_02);

			map_v_2d[v1] = pointf3(dist01, 0, 0);   // set new v1 = (dist01,0,0)
			map_v_2d[v2] = pointf3(dist02 * cos01_02, dist02 * sin01_02, 0);  // set new v2 = (d02 * cos, d02 * sin, 0)

			this->points2d.push_back(map_v_2d); // triangle in points2d has the same sequence with heMesh->triangles()

			// get cot
			map<V*, double> cotan_2d;
			for (size_t i = 0; i < 3; i++)
			{
				double cos = vecf3::cos_theta((triangle->BoundaryVertice()[i]->pos - triangle->BoundaryVertice()[(i + 2) % 3]->pos),
					(triangle->BoundaryVertice()[(i + 1) % 3]->pos - triangle->BoundaryVertice()[(i + 2) % 3]->pos));
				double sin = sqrt(1 - cos * cos);
				cotan_2d[triangle->BoundaryVertice()[(i + 2) % 3]] = (cos / sin); // store the angle of edge i, i+1. idx is V_{(i+2) %3}
			}
			assert(cotan_2d.size() == 3);
			this->cot2d.push_back(cotan_2d);
		}
	}

	assert(this->points2d.size() == this->nT);
	assert(this->cot2d.size() == this->nT);

}

double ARAP::getCotan(int tri_idx, V* v)
{
	assert(this->cot2d.size() == this->nT);
	try
	{
		auto cotan = cot2d[tri_idx].at(v);
		return cotan;
	}
	catch (const std::exception& e)
	{
		cout << "[Error] Cannot find V in map. ";
	}
}
double ARAP::getCotan(V* v0, V* v1, V* v2)
{
	double cos = vecf3::cos_theta((v0->pos - v2->pos), (v1->pos - v2->pos));
	double sin = sqrt(1 - cos * cos);
	return cos / sin;
}


void ARAP::setb(size_t idx1, pointf2 pos1, size_t idx2, pointf2 pos2)
{
	ARAP_mat_b = MatrixXd(this->nV, 2);
	ARAP_mat_b.setZero();

	// two anchor points
	ARAP_mat_b(idx1, 0) = pos1[0];  // x
	ARAP_mat_b(idx1, 1) = pos1[1]; // y

	//ARAP_mat_b(idx2, 0) = pos2[0];  // x
	//ARAP_mat_b(idx2, 1) = pos2[1]; // y

	// Other non-anchor points
	for (size_t i = 0; i < this->nV; i++)
	{
		if (i != idx1/* && i != idx2*/) {
			auto v = heMesh->Vertices()[i];
			MatrixXd b(2, 1);
			b.setZero();
			//cout << "v idx: " << heMesh->Index(v) << endl<<endl;
			// traverse adjecent vertrices
			for (auto adj_v : v->AdjVertices())
			{
				size_t adj_idx = heMesh->Index(adj_v);  // get index of adj_v
				// get adjacent triangles
				auto e = v->EdgeWith(adj_v);
				auto he1 = e->HalfEdge();
				auto he2 = e->HalfEdge()->Pair();

				// get Lt and cot
				auto triangle1 = he1->Polygon();
				if (triangle1 != nullptr)
				{
					auto tri_v1 = he1->Next()->End();
					assert(heMesh->Index(tri_v1) != heMesh->Index(v)
						&& heMesh->Index(tri_v1) != heMesh->Index(adj_v));

					size_t tri_idx = heMesh->Index(triangle1);
					double cot1 = getCotan(tri_idx, tri_v1);
					map<V*, pointf3> mapped_v = this->points2d[tri_idx]; // congruent mapping of triangle1 
					MatrixXd Lt = Lt_array[tri_idx];
					MatrixXd delta_x(2, 1);
					delta_x <<
						mapped_v[v][0] - mapped_v[adj_v][0],
						mapped_v[v][1] - mapped_v[adj_v][1];

					b += cot1 * Lt * delta_x;
				}

				auto triangle2 = he2->Polygon();
				if (triangle2 != nullptr)
				{
					auto tri_v2 = he2->Next()->End();
					assert(heMesh->Index(tri_v2) != heMesh->Index(v)
						&& heMesh->Index(tri_v2) != heMesh->Index(adj_v));

					size_t tri_idx = heMesh->Index(triangle2);
					double cot2 = getCotan(tri_idx, tri_v2);
					map<V*, pointf3> mapped_v = this->points2d[tri_idx]; // congruent mapping of triangle1 
					MatrixXd Lt = Lt_array[tri_idx];
					MatrixXd delta_x(2, 1);
					delta_x <<
						mapped_v[v][0] - mapped_v[adj_v][0],
						mapped_v[v][1] - mapped_v[adj_v][1];
					b += cot2 * Lt * delta_x;
				}
			}
			ARAP_mat_b(i, 0) = b(0); // x
			ARAP_mat_b(i, 1) = b(1); // y 
		}
	}
}