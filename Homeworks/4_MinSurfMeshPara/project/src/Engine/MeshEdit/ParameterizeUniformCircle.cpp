#include <Engine/MeshEdit/ParameterizeUniformCircle.h>

#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>

using namespace Ubpa;
using namespace std;
#define PI 3.1415926


ParameterizeUniformCircle::ParameterizeUniformCircle(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
}

void ParameterizeUniformCircle::Clear() {
	heMesh->Clear();
	triMesh = nullptr;
}

bool ParameterizeUniformCircle::Init(Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::MinSurf::Init:\n"
			"\t""trimesh is invalid\n");
		return false;
	}

	// init half-edge structure �����ʼ��ʹ�õ���������Ƭ�����ｫ��������Ƭת��Ϊ��߽ṹ
	size_t nV = triMesh->GetPositions().size();
	vector<vector<size_t>> triangles;
	triangles.reserve(triMesh->GetTriangles().size());
	for (auto triangle : triMesh->GetTriangles())
		triangles.push_back({ triangle->idx[0], triangle->idx[1], triangle->idx[2] });
	heMesh->Reserve(nV);
	heMesh->Init(triangles);

	if (!heMesh->IsTriMesh() || !heMesh->HaveBoundary()) {
		printf("ERROR::MinSurf::Init:\n"
			"\t""trimesh is not a triangle mesh or hasn't a boundaries\n");
		heMesh->Clear();
		return false;
	}

	// triangle mesh's positions ->  half-edge structure's positions 
	for (int i = 0; i < nV; i++) {
		auto v = heMesh->Vertices().at(i);
		v->pos = triMesh->GetPositions()[i].cast_to<vecf3>();
	}

	this->triMesh = triMesh;
	return true;
}

bool ParameterizeUniformCircle::Run() {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	Parameterization();

	// half-edge structure -> triangle mesh
	size_t nV = heMesh->NumVertices();
	size_t nF = heMesh->NumPolygons();

	vector<pointf3> positions;
	vector<unsigned> indice;
	vector<pointf2> texcoords;

	positions.reserve(nV);
	indice.reserve(3 * nF);
	texcoords.reserve(nV);

	for (auto v : heMesh->Vertices()) {
		positions.push_back(v->pos.cast_to<pointf3>());
		texcoords.push_back(v->pos.cast_to<pointf2>());
	}

	for (auto f : heMesh->Polygons()) { // f is triangle
		for (auto v : f->BoundaryVertice()) // vertices of the triangle
			indice.push_back(static_cast<unsigned>(heMesh->Index(v)));
	}
	triMesh->Init(indice, positions);
	if (texture_flag)
		triMesh->Update(texcoords);
	//this->triMesh->Update(texCoor);


	return true;
}

void Ubpa::ParameterizeUniformCircle::Parameterization()
{
	// step1�����߽�
	// step2���̶��߽��
	Fix_boundary();
	// step3������ϡ�跽����
	Create_equation();
	// step4�����ϡ�跽����
	Solve_equation();
	// step5�����¶�������
	Update_vetecies();
	// step6����������ͼ�񣬸�����ʾ����ͼ��
	//Add_texture();
}


void Ubpa::ParameterizeUniformCircle::Fix_boundary()
{
	double boundary_vertecies_num = heMesh->Boundaries()[0].size();

	size_t nV = heMesh->NumVertices(); // mesh�ĵ���
	B_x.resize(nV);	B_x.setZero();
	B_y.resize(nV);	B_y.setZero();
	B_z.resize(nV);	B_z.setZero();


	for (size_t i = 0; i < boundary_vertecies_num; i++)
	{
		double degree_unit = 360.0 / boundary_vertecies_num;
		double degree_i = (i * degree_unit) * PI / 180.0;
		double x = sin(degree_i);
		double y = cos(degree_i);

		size_t boundary_i_index = heMesh->Index(heMesh->Boundaries()[0][i]->Origin()); // ��i�������index
		B_x(boundary_i_index) = x;
		B_y(boundary_i_index) = y;
		B_z(boundary_i_index) = 0;
	}
}


void Ubpa::ParameterizeUniformCircle::Create_equation()
{
	size_t nV = heMesh->NumVertices(); // mesh�ĵ���
	A_sparse.resize(nV, nV);
	A_sparse.setZero();
	for (size_t i = 0; i < nV; i++) {
		A_sparse.insert(i, i) = 1; // �Խ��߶�Ϊ1
		V* v_i = heMesh->Vertices().at(i);
		if (!v_i->IsBoundary()) {
			// ����ϵ������A���������ֱ�����ڵ��ƽ��
			double v_i_degree = v_i->AdjVertices().size(); // ����v_i�Ķ�
			for (size_t j = 0; j < v_i_degree; j++) {
				size_t v_j_index = heMesh->Index(v_i->AdjVertices().at(j)); // ����v_i�ĵ�j���ڵ��index
				A_sparse.insert(i, v_j_index) = -1 / v_i_degree;
				//A_sparse.insert(i, heMesh->Index(v_i->AdjVertices()[j]) ) = -1 / v_i_degree;
			}
		}
	}
}

void Ubpa::ParameterizeUniformCircle::Solve_equation()
{
	solver.compute(A_sparse);
	if (solver.info() != Success) {
		qDebug() << "decomposition failed" << endl;
	}

	X_x = solver.solve(B_x);
	X_y = solver.solve(B_y);
	X_z = solver.solve(B_z);

}

void Ubpa::ParameterizeUniformCircle::Update_vetecies()
{
	size_t nV = heMesh->NumVertices(); // mesh�ĵ���

	X_x_backup.resize(nV);
	X_y_backup.resize(nV);
	X_z_backup.resize(nV);

	for (size_t i = 0; i < nV; i++) {
		V* v_i = heMesh->Vertices().at(i);

		X_x_backup(i) = heMesh->Vertices()[i]->pos.at(0);
		X_y_backup(i) = heMesh->Vertices()[i]->pos.at(1);
		X_z_backup(i) = heMesh->Vertices()[i]->pos.at(2);

		heMesh->Vertices()[i]->pos.at(0) = X_x(i);
		heMesh->Vertices()[i]->pos.at(1) = X_y(i);
		heMesh->Vertices()[i]->pos.at(2) = X_z(i);

	}
}

void Ubpa::ParameterizeUniformCircle::Add_texture()
{
	texture_flag = true;
	size_t nV = heMesh->NumVertices(); // mesh�ĵ���
	for (size_t i = 0; i < nV; i++) {
		heMesh->Vertices()[i]->pos.at(0) = X_x_backup(i); // ������
		heMesh->Vertices()[i]->pos.at(1) = X_y_backup(i);
		heMesh->Vertices()[i]->pos.at(2) = X_z_backup(i);

	}
}