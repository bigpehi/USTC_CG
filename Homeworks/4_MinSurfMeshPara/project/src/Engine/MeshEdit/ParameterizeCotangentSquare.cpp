#include <Engine/MeshEdit/ParameterizeCotangentSquare.h>

#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>

using namespace Ubpa;
using namespace std;
#define PI 3.1415926


ParameterizeCotangentSquare::ParameterizeCotangentSquare(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
}

void ParameterizeCotangentSquare::Clear() {
	heMesh->Clear();
	triMesh = nullptr;
}

bool ParameterizeCotangentSquare::Init(Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::MinSurf::Init:\n"
			"\t""trimesh is invalid\n");
		return false;
	}

	// init half-edge structure 对象初始化使用的是三角面片，这里将该三角面片转换为半边结构
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

bool ParameterizeCotangentSquare::Run() {
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

void Ubpa::ParameterizeCotangentSquare::Parameterization()
{
	// step1：检测边界
	// step2：固定边界点
	Fix_boundary();
	// step3：构建稀疏方程组
	Create_equation();
	// step4：求解稀疏方程组
	Solve_equation();
	// step5：更新顶点坐标
	Update_vetecies();
	// step6：链接纹理图像，更新显示（贴图）
	//Add_texture();
}


void Ubpa::ParameterizeCotangentSquare::Fix_boundary()
{
	double boundary_vertecies_num = heMesh->Boundaries()[0].size();

	// 将boundary_vertecies_num四等分
	double quarter_point_1 = 0;
	double quarter_point_2 = boundary_vertecies_num / 4;
	double quarter_point_3 = boundary_vertecies_num / 2;
	double quarter_point_4 = (boundary_vertecies_num / 4) * 3;
	double quarter_point_5 = boundary_vertecies_num;
	size_t nV = heMesh->NumVertices(); // mesh的点数
	B_x.resize(nV);	B_x.setZero();
	B_y.resize(nV);	B_y.setZero();
	B_z.resize(nV);	B_z.setZero();

	for (size_t i = quarter_point_1; i < quarter_point_2; i++)
	{
		size_t boundary_i_index = heMesh->Index(heMesh->Boundaries()[0][i]->Origin()); // 第i个顶点的index
		if (i <= quarter_point_1)
			B_x(boundary_i_index) = 0;
		else if (i + 1 >= quarter_point_2 || i * (1 / (quarter_point_2 - quarter_point_1)) > 1)
			B_x(boundary_i_index) = 1;
		else
			B_x(boundary_i_index) = i * (1 / (quarter_point_2 - quarter_point_1));
		B_y(boundary_i_index) = 0;
		B_z(boundary_i_index) = 0;
	}
	for (size_t i = quarter_point_2; i < quarter_point_3; i++)
	{
		size_t boundary_i_index = heMesh->Index(heMesh->Boundaries()[0][i]->Origin()); // 第i个顶点的index
		B_x(boundary_i_index) = 1;
		if (i <= quarter_point_2)
			B_y(boundary_i_index) = 0;
		else if (i + 1 >= quarter_point_3 || (i - quarter_point_2) * (1 / (quarter_point_3 - quarter_point_2)) > 1)
			B_y(boundary_i_index) = 1;
		else
			B_y(boundary_i_index) = (i - quarter_point_2) * (1 / (quarter_point_3 - quarter_point_2));
		B_z(boundary_i_index) = 0;
	}
	for (size_t i = quarter_point_3; i < quarter_point_4; i++)
	{
		size_t boundary_i_index = heMesh->Index(heMesh->Boundaries()[0][i]->Origin()); // 第i个顶点的index
		if (i <= quarter_point_3)
			B_x(boundary_i_index) = 1;
		else if (i + 1 >= quarter_point_4 || (i - quarter_point_3) * (1 / (quarter_point_4 - quarter_point_3)) > 1)
			B_y(boundary_i_index) = 0;
		else
			B_x(boundary_i_index) = 1 - (i - quarter_point_3) * (1 / (quarter_point_4 - quarter_point_3));
		B_y(boundary_i_index) = 1;
		B_z(boundary_i_index) = 0;
	}
	for (size_t i = quarter_point_4; i < quarter_point_5; i++)
	{
		size_t boundary_i_index = heMesh->Index(heMesh->Boundaries()[0][i]->Origin()); // 第i个顶点的index
		B_x(boundary_i_index) = 0;
		if (i <= quarter_point_4)
			B_y(boundary_i_index) = 1;
		else if (i + 1 >= quarter_point_5 || (i - quarter_point_4) * (1 / (quarter_point_5 - quarter_point_4)) > 1)
			B_y(boundary_i_index) = 0;
		else
			B_y(boundary_i_index) = 1 - (i - quarter_point_4) * (1 / (quarter_point_5 - quarter_point_4));
		B_z(boundary_i_index) = 0;
	}
}


void Ubpa::ParameterizeCotangentSquare::Create_equation()
{
	size_t nV = heMesh->NumVertices(); // mesh的点数
	A_sparse.resize(nV, nV);
	A_sparse.setZero();
	for (size_t i = 0; i < nV; i++) {
		A_sparse.insert(i, i) = 1; // 对角线都为1
		V* v_i = heMesh->Vertices().at(i);
	

		if (!v_i->IsBoundary()) {
			// 设置系数矩阵A，重心坐标使用cotangent
			int v_i_degree = v_i->AdjVertices().size(); // 顶点v_i的度
			double* omiga = new double[v_i_degree];
			double omiga_sum = 0;
			for (size_t j = 0; j < v_i_degree; j++) {
				V* p_i = v_i;
				V* p_j; V* p_j_fore; V* p_j_rear;
				if (j == 0) {
					p_j = v_i->AdjVertices().at(0);
					p_j_fore = v_i->AdjVertices().at(v_i_degree-1);
					p_j_rear = v_i->AdjVertices().at(1);
				}
				else if(j == v_i_degree - 1) {
					p_j = v_i->AdjVertices().at(v_i_degree-1);
					p_j_fore = v_i->AdjVertices().at(v_i_degree - 2);
					p_j_rear = v_i->AdjVertices().at(0);
				}
				else {
					p_j = v_i->AdjVertices().at(j);
					p_j_fore = v_i->AdjVertices().at(j - 1);
					p_j_rear = v_i->AdjVertices().at(j + 1);
				}
				double p0x = v_i->pos.at(0);	  double p0y = v_i->pos.at(1);      double p0z = v_i->pos.at(2);
				double p1x = p_j_fore->pos.at(0); double p1y = p_j->pos.at(1);      double p1z = p_j->pos.at(2);
				double p2x = p_j->pos.at(0);      double p2y = p_j->pos.at(1);      double p2z = p_j->pos.at(2);
				double p3x = p_j_rear->pos.at(0); double p3y = p_j_rear->pos.at(1); double p3z = p_j_rear->pos.at(2);
				double p0_p1_pow_2 = pow(p1x - p0x, 2.0) + pow(p1y - p0y, 2.0) + pow(p1z - p0z, 2.0);
				double p0_p2_pow_2 = pow(p2x - p0x, 2.0) + pow(p2y - p0y, 2.0) + pow(p2z - p0z, 2.0);
				double p0_p3_pow_2 = pow(p3x - p0x, 2.0) + pow(p3y - p0y, 2.0) + pow(p3z - p0z, 2.0);
				double p1_p2_pow_2 = pow(p2x - p1x, 2.0) + pow(p2y - p1y, 2.0) + pow(p2z - p1z, 2.0);
				double p2_p3_pow_2 = pow(p3x - p2x, 2.0) + pow(p3y - p2y, 2.0) + pow(p3z - p2z, 2.0);
				double cos_alpha = (p0_p1_pow_2 + p0_p2_pow_2 - p1_p2_pow_2) / (2 * sqrt(p0_p1_pow_2) * sqrt(p0_p2_pow_2)); // 弧度
				double cos_delta = (p0_p2_pow_2 + p0_p3_pow_2 - p2_p3_pow_2) / (2 * sqrt(p0_p2_pow_2) * sqrt(p0_p3_pow_2));
				double alpha = acos(cos_alpha);
				double delta = acos(cos_delta);
				omiga[j] = (tan(alpha / 2.0) + tan(delta / 2.0)) / sqrt(p0_p2_pow_2);
				omiga_sum += omiga[j];

				// 用cotalpha + cotbeta 会有很多点飞走
				//double cos_alpha = (p0_p1_pow_2 + p1_p2_pow_2 - p0_p2_pow_2) / (2 * sqrt(p0_p1_pow_2) * sqrt(p1_p2_pow_2)); // 弧度
				//double cos_beta = (p0_p3_pow_2 + p2_p3_pow_2 - p0_p2_pow_2) / (2 * sqrt(p0_p3_pow_2) * sqrt(p2_p3_pow_2));
				//double sin_alpha = sqrt(1 - pow(cos_alpha,2.0));
				//double sin_beta = sqrt(1 - pow(cos_beta,2.0));
				//double cot_alpha = cos_alpha/sin_alpha;
				//double cot_beta = cos_beta/sin_beta;
				//omiga[j] = cot_alpha + cot_beta;
				//omiga_sum += omiga[j];
			}
			for (size_t j = 0; j < v_i_degree; j++) {
				size_t v_j_index = heMesh->Index(v_i->AdjVertices().at(j)); // 顶点v_i的第j个邻点的index
				A_sparse.insert(i, v_j_index) = -omiga[j]/omiga_sum;
			}
			delete[] omiga;
		}
	}
}

void Ubpa::ParameterizeCotangentSquare::Solve_equation()
{
	solver.compute(A_sparse);
	if (solver.info() != Success) {
		qDebug() << "decomposition failed" << endl;
	}

	X_x = solver.solve(B_x);
	X_y = solver.solve(B_y);
	X_z = solver.solve(B_z);

}

void Ubpa::ParameterizeCotangentSquare::Update_vetecies()
{
	size_t nV = heMesh->NumVertices(); // mesh的点数

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

void Ubpa::ParameterizeCotangentSquare::Add_texture()
{
	texture_flag = true;
	size_t nV = heMesh->NumVertices(); // mesh的点数
	for (size_t i = 0; i < nV; i++) {
		heMesh->Vertices()[i]->pos.at(0) = X_x_backup(i); // 索引错
		heMesh->Vertices()[i]->pos.at(1) = X_y_backup(i);
		heMesh->Vertices()[i]->pos.at(2) = X_z_backup(i);

	}


}
