#include <Engine/MeshEdit/Simulate.h>


#include <Eigen/Sparse>

void Simulate::Clear() {
	this->positions.clear();
	this->velocity.clear();
}

bool Simulate::Init() {
	is_accelrating = false;
	//Clear(); // why not
	std::cout << "Init" << std::endl;

	// ��ʼ��ʱ������ı߳�������Ϊ��Ȼ����
	edge_rest.resize(edgelist.size() / 2);
	for (size_t i = 0; i < edge_rest.size(); i++)
	{
		int p1_index = edgelist[i * 2];
		int p2_index = edgelist[i * 2 + 1];
		double p1_x = positions[p1_index][0], p1_y = positions[p1_index][1], p1_z = positions[p1_index][2];
		double p2_x = positions[p2_index][0], p2_y = positions[p2_index][1], p2_z = positions[p2_index][2];
		edge_rest[i] = sqrt(pow(p1_x - p2_x, 2) + pow(p1_y - p2_y, 2) + pow(p1_z - p2_z, 2)) + 0.0001f;
	}

	// ��ʼ�������ʵ��velocity
	this->velocity.resize(positions.size());
	for (int i = 0; i < positions.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			this->velocity[i][j] = 0;
		}
	}


	// �����㷨�ĳ�ʼ����һЩ����Ҫִ��һ�εĸ��ӹ�����
	Generate_global_M2hL();
	Generate_J();
	Generate_global_coeffsMat();

	return true;
}


// simulatestart �����󣬻�ѭ�������������
bool Simulate::Run() {

	SimulateOnce();

	// half-edge structure -> triangle mesh
	return true;
}

void Ubpa::Simulate::SetLeftFix()
{
	//�̶�����x������С��
	fixed_id.clear();
	double x = 100000;
	double y_min = 100000;
	double y_max = 0;
	for (int i = 0; i < positions.size(); i++)
	{
		if (positions[i][0] < x)
		{
			x = positions[i][0];
		}
		if (positions[i][1] < y_min)
		{
			y_min = positions[i][1];
		}
		if (positions[i][1] > y_max)
		{
			y_max = positions[i][1];
		}
	}

	for (int i = 0; i < positions.size(); i++)
	{
		if (abs(positions[i][0] - x) < 1e-5)
		{
			if (abs(positions[i][1] - y_max) < 1e-5 || abs(positions[i][1] - y_min) < 1e-5)
			{
				fixed_id.push_back(i);
			}
		}
	}
	Init();
}

void Ubpa::Simulate::Accelerate_on()
{
	is_accelrating = true;
}

void Simulate::SimulateOnce() {
	if (is_accelrating) {
		Accelerate_liu();
	}
	else
	{
		ImplicitEuler();
	}

}

void Ubpa::Simulate::ImplicitEuler()
{
	std::vector<Triplet<double>> tripletlist;
	std::vector<Matrix3d> coeffs_diag; // ��Խ����ϵľ����
	SparseMatrix<double> coeffsM_nabla_gxk;
	Matrix<double, -1, 3> f_int;
	Matrix<double, -1, 3> Y;
	VectorXd X_k;
	VectorXd X_kplus1;

	VectorXd b_gx; 

	coeffsM_nabla_gxk.resize(positions.size() * 3, positions.size() * 3);
	f_int.resize(positions.size(), 3); // ���� n*3
	Y.resize(positions.size(), 3); // y n*3
	X_k.resize(positions.size() * 3); X_k.setZero();
	X_kplus1.resize(positions.size() * 3); X_kplus1.setZero();
	b_gx.resize(positions.size() * 3); // �����Ҷ��� 3n*1
	coeffs_diag.resize(positions.size());

	MatrixXd Y_t(Y.cols(), Y.rows()); // y��ת��
	MatrixXd f_int_t(f_int.cols(), f_int.rows()); // f_int��ת��

	Vector3d G(Vector3d::Zero()); // ������ΪΨһ����
	G(2) = 1;

	// ���ڼ���Y
	MatrixXd X(positions.size(), 3); X.setZero();
	MatrixXd V(positions.size(), 3); V.setZero();
	MatrixXd F_ext(positions.size(), 3); F_ext.setZero();

	// ����Y
	for (size_t i = 0; i < positions.size(); i++)
		for (size_t j = 0; j < 3; j++)
			X(i, j) = positions[i][j];
	for (size_t i = 0; i < positions.size(); i++)
		for (size_t j = 0; j < 3; j++)
			V(i, j) = velocity[i][j];
	for (size_t i = 0; i < positions.size(); i++) {
		std::vector<unsigned>::iterator it = find(fixed_id.begin(), fixed_id.end(), i);
		if (it == fixed_id.end())  // ˵���˵㲻�ǹ̶���
			for (size_t j = 0; j < 3; j++)
				F_ext(i, j) = G(j);
	}

	MatrixXd M(F_ext.rows(), F_ext.rows());
	M.setZero();
	for (size_t i = 0; i < M.rows(); i++)
			M(i, i) = mass;
	Y = X + h * V + h * h * M.inverse() * F_ext;
	for (size_t i = 0; i < positions.size(); i++)
	{
		for (size_t j = 0; j < 3; j++) {
			X_k(i * 3 + j) = Y(i, j);
		}
	}


	int iteration = 4; // ţ�ٷ���������
	for (size_t it = 0; it < iteration; it++)
	{
		// ����
		coeffsM_nabla_gxk.setZero(); // Remove all non-zero coefficients
		f_int.setZero();
		tripletlist.clear();
		for (size_t i = 0; i < positions.size(); i++)
			coeffs_diag[i] = Matrix3d::Identity() * mass;
		for (size_t i = 0; i < edgelist.size() / 2; i++)
		{
			size_t p1_index_in_positions = edgelist[i * 2];
			size_t p2_index_in_positions = edgelist[i * 2 + 1];

			double p1_x = X_k(p1_index_in_positions * 3 + 0);
			double p1_y = X_k(p1_index_in_positions * 3 + 1);
			double p1_z = X_k(p1_index_in_positions * 3 + 2);
			double p2_x = X_k(p2_index_in_positions * 3 + 0);
			double p2_y = X_k(p2_index_in_positions * 3 + 1);
			double p2_z = X_k(p2_index_in_positions * 3 + 2);

			double rest_p1p2 = edge_rest[i]; // ��i���ߵ���Ȼ����

			double dist_p1p2 = sqrt(pow(p1_x - p2_x, 2) + pow(p1_y - p2_y, 2) + pow(p1_z - p2_z, 2)) + 0.0001f; // p1��p2֮��ľ���
			Vector3d vector_p1p2;
			vector_p1p2[0] = p1_x - p2_x; vector_p1p2[1] = p1_y - p2_y; vector_p1p2[2] = p1_z - p2_z; // ����p1-p2

			// ���㲢�����ʵ��ܵ��ĵ�����ע�ⵯ������ͼӼ��Ĺ�ϵ
			Vector3d f_int_temp = stiff * (dist_p1p2 - rest_p1p2) * vector_p1p2 / dist_p1p2;
			//std::cout << std::endl << "f_int_temp: " << f_int_temp[0] << "	" << f_int_temp[1] << "	" << f_int_temp[2] << std::endl; ////////

			for (size_t k = 0; k < 3; k++)
			{
				std::vector<unsigned>::iterator it_p1 = find(fixed_id.begin(), fixed_id.end(), p1_index_in_positions);
				if (it_p1 == fixed_id.end())  // ˵��p1�㲻�ǹ̶���
				{
					f_int(p1_index_in_positions, k) -= f_int_temp(k);
				}
				std::vector<unsigned>::iterator it_p2 = find(fixed_id.begin(), fixed_id.end(), p2_index_in_positions);
				if (it_p2 == fixed_id.end())  // ˵��p2�㲻�ǹ̶���
				{
					f_int(p2_index_in_positions, k) += f_int_temp(k);
				}

			}

			// ���㵯����p1λ�õĵ���
			Matrix3d partial_temp = stiff * (rest_p1p2 / dist_p1p2 - 1) * Matrix3d::Identity() \
				- stiff * rest_p1p2 * pow(dist_p1p2, -3) * vector_p1p2 * vector_p1p2.transpose();

			// ���ϡ�����
			for (size_t k = 0; k < 3; k++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					std::vector<unsigned>::iterator it_p1 = find(fixed_id.begin(), fixed_id.end(), p1_index_in_positions);
					if (it_p1 == fixed_id.end())  // ˵��p1�㲻�ǹ̶���
					{
						tripletlist.push_back(Triplet<double>(p1_index_in_positions * 3 + k, p2_index_in_positions * 3 + j, h * h * partial_temp(k, j)));
						coeffs_diag[p1_index_in_positions](k, j) += -h * h * partial_temp(k, j);
					}
					std::vector<unsigned>::iterator it_p2 = find(fixed_id.begin(), fixed_id.end(), p2_index_in_positions);
					if (it_p2 == fixed_id.end())  // ˵��p2�㲻�ǹ̶���
					{
						tripletlist.push_back(Triplet<double>(p2_index_in_positions * 3 + k, p1_index_in_positions * 3 + j, h * h * partial_temp(k, j)));
						coeffs_diag[p2_index_in_positions](k, j) += -h * h * partial_temp(k, j);
					}
				}
			}
		}
		for (size_t i = 0; i < positions.size(); i++)
		{
			for (size_t k = 0; k < 3; k++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					tripletlist.push_back(Triplet<double>(i * 3 + k, i * 3 + j, coeffs_diag[i](k, j)));
				}
			}
		}
		coeffsM_nabla_gxk.setFromTriplets(tripletlist.begin(), tripletlist.end());

		// ����g(x)
		MatrixXd g_x(positions.size(), 3);
		g_x.setZero();

		for (size_t i = 0; i < positions.size(); i++)
			for (size_t j = 0; j < 3; j++)
				X(i, j) = X_k(i * 3 + j);

		g_x = M * (X - Y) - h * h * f_int; // ����
		for (size_t i = 0; i < g_x.rows(); i++)
		{
			std::vector<unsigned>::iterator it = find(fixed_id.begin(), fixed_id.end(), i);
			if (it != fixed_id.end())  // ˵���˵��ǹ̶���
			{
				for (size_t j = 0; j < g_x.cols(); j++)
				{
					g_x(i, j) = 0;
				}
			}
			
		}

		b_gx.setZero(); // g_xչ����������

		for (size_t i = 0; i < positions.size(); i++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				b_gx(i * 3 + j) = g_x(i, j);
			}
		}
		// ��ϡ�跽����
		SparseLU<SparseMatrix<double>> solver;
		VectorXd X_temp(positions.size() * 3); X_temp.setZero();
		solver.compute(coeffsM_nabla_gxk);
		if (solver.info() != Success) {
			std::cout << "decomposition failed" << std::endl;
		}
		X_temp = solver.solve(b_gx);

		X_kplus1 = X_k - X_temp;
		X_k = X_kplus1;

	}
	// ���������������ٶȺ�λ��
	for (size_t i = 0; i < positions.size(); i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			velocity[i][j] = (X_k(i * 3 + j) - positions[i][j]) / h;
		}
	}
	for (size_t i = 0; i < positions.size(); i++)
	{
		std::vector<unsigned>::iterator it = find(fixed_id.begin(), fixed_id.end(), i);
		if (it == fixed_id.end())  // ˵���˵㲻�ǹ̶���
			for (size_t j = 0; j < 3; j++)
				positions[i][j] = X_k(i * 3 + j);
		else
			for (size_t j = 0; j < 3; j++)
				velocity[i][j] = 0;
	}
}

void Ubpa::Simulate::Accelerate_liu()
{
	// ��������F_ext����
	Vector3d G(Vector3d::Zero()); // ������ΪΨһ����
	G(2) = 1;
	VectorXd F_ext(positions.size()*3); F_ext.setZero();
	for (size_t i = 0; i < positions.size(); i++) {
		std::vector<unsigned>::iterator it = find(fixed_id.begin(), fixed_id.end(), i);
		if (it == fixed_id.end())  // ˵���˵㲻�ǹ̶���
			for (size_t j = 0; j < 3; j++)
				F_ext(i * 3 + j) = G(j);
	}

	// ����x_n����, 3n��������x_n = q_{n+1}
	VectorXd X(3 * positions.size());
	X.setZero();
	for (size_t i = 0; i < positions.size(); i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			X(i * 3 + j) = positions[i][j];
		}
	}

	// ����y������3n��������y = 2*q_n - q_{n-1}
	VectorXd y(3 * positions.size());
	y.setZero();
	for (size_t i = 0; i < positions.size(); i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			y(i * 3 + j) = positions[i][j] + h * velocity[i][j] + h * h / mass * F_ext[3 * i + j];
		}
	}

	// ����d������3n������
	VectorXd d(3 * edgelist.size()/2);
	d.setZero();
	for (size_t i = 0; i < edgelist.size() / 2; i++)
	{
		auto x1 = edgelist[2 * i];
		auto x2 = edgelist[2 * i + 1];
		auto p1 = positions[x1];
		auto p2 = positions[x2];
		double dist = (p1 - p2).norm();
		for (int j = 0; j < 3; j++)
		{
			d(3 * i + j) = edge_rest[i] * (p1[j] - p2[j]) / dist;
		}
	}

	VectorXd hJdMy(3 * edgelist.size() / 2);
	hJdMy.setZero();
	VectorXd x_nplus1(3 * positions.size());
	x_nplus1.setZero();
	int iteration = 5;
	for (size_t it = 0; it < iteration; it++)
	{
		// global phase, fix d and compute the optimal x
		hJdMy = K * (h * h * J * d + mass * y - global_M2hL * b);
		VectorXd x_result = solver_coeffsMat.solve(hJdMy);
		if (solver_coeffsMat.info() != Success) {
			std::cout << "decomposition failed" << std::endl;
		}
		size_t j = 0;
		for (size_t i = 0; i < positions.size(); i++)
		{
			std::vector<unsigned>::iterator it = find(fixed_id.begin(), fixed_id.end(), i);
			if (it == fixed_id.end())  // ˵���˵㲻�ǹ̶���
			{
				for (int k = 0; k < 3; k++)
				{
					x_nplus1(3 * i + k) = x_result(3 * j + k);
				}
				j++;
			}
		}

		// local phase, we fix x and compute the optimal d
		for (size_t i = 0; i < edgelist.size()/2; i++)
		{
			auto x1 = edgelist[2 * i];
			auto x2 = edgelist[2 * i + 1];
			pointf3 p1, p2;
			for (int k = 0; k < 3; k++)
			{
				p1[k] = x_nplus1(3 * x1 + k);
				p2[k] = x_nplus1(3 * x2 + k);
			}
			double dist = (p1 - p2).norm();
			for (int j = 0; j < 3; j++)
			{
				d(3 * i + j) = edge_rest[i] / dist * (p1[j] - p2[j]);
			}
		}
	}

	// �����ٶȺ�λ��
	for (int i = 0; i < positions.size(); i++)
	{
		std::vector<unsigned>::iterator it = find(fixed_id.begin(), fixed_id.end(), i);
		if (it == fixed_id.end())  // ˵���˵㲻�ǹ̶���
		{
			for (int j = 0; j < 3; j++)
			{
				velocity[i][j] = (x_nplus1(3 * i + j) - positions[i][j]) / h;
				positions[i][j] = x_nplus1(3 * i + j);
			}
		}
	}

}

void Ubpa::Simulate::Generate_global_M2hL()
{
	std::vector<Triplet<double>> tripletlist;
	for (size_t i = 0; i < edgelist.size() / 2; i++)
	{
		auto x1 = edgelist[2 * i];
		auto x2 = edgelist[2 * i + 1];
		for (int j = 0; j < 3; j++)
		{
			tripletlist.push_back(Triplet<double>(3 * x1 + j, 3 * x1 + j, h * h * stiff));
			tripletlist.push_back(Triplet<double>(3 * x1 + j, 3 * x2 + j, -h * h * stiff));
			tripletlist.push_back(Triplet<double>(3 * x2 + j, 3 * x1 + j, -h * h * stiff));
			tripletlist.push_back(Triplet<double>(3 * x2 + j, 3 * x2 + j, h * h * stiff));
		}
	}
	for (size_t i = 0; i < 3 * positions.size(); i++)
	{
		tripletlist.push_back(Triplet<double>(i, i, mass));
	}
	global_M2hL.resize(3 * positions.size(), 3 * positions.size());
	global_M2hL.setZero();
	global_M2hL.setFromTriplets(tripletlist.begin(), tripletlist.end());
}

void Ubpa::Simulate::Generate_global_coeffsMat()
{
	b.resize(3 * positions.size());
	b.setZero();

	std::vector<Triplet<double>> tripletlist;

	int j = 0;
	for (size_t i = 0; i < positions.size(); i++)
	{
		std::vector<unsigned>::iterator it = find(fixed_id.begin(), fixed_id.end(), i);
		if (it == fixed_id.end())  // ˵���˵㲻�ǹ̶���
		{
			for (int k = 0; k < 3; k++)
			{
				tripletlist.push_back(Triplet<double>(3 * j + k, 3 * i + k, 1.0));
			}
			j++;
		}
		else
		{
			for (int k = 0; k < 3; k++)
			{
				b(3 * i + k, 0) = positions[i][k];
			}
		}
	}
	K.resize(3 * (positions.size() - fixed_id.size()), 3 * positions.size());
	K.setZero();
	K.setFromTriplets(tripletlist.begin(), tripletlist.end());
	SparseMatrix<double> Kt = K.transpose();
	global_coeffsMat.setZero();
	if (fixed_id.size() > 0)
	{
		global_coeffsMat = K * global_M2hL * Kt;
	}
	else
	{
		global_coeffsMat = global_M2hL;
	}
	global_coeffsMat.makeCompressed();
	solver_coeffsMat.compute(global_coeffsMat);
}

void Ubpa::Simulate::Generate_J()
{
	J.resize(3 * positions.size(), 3 * edgelist.size() / 2);
	J.setZero();
	std::vector<Triplet<double>> tripletlist;
	for (size_t i = 0; i < edgelist.size()/2; i++)
	{
		auto x1 = edgelist[2 * i];
		auto x2 = edgelist[2 * i + 1];
		for (int j = 0; j < 3; j++)
		{
			tripletlist.push_back(Triplet<double>(3 * x1 + j, 3 * i + j, stiff));
			tripletlist.push_back(Triplet<double>(3 * x2 + j, 3 * i + j, -stiff));
		}
	}
	J.setFromTriplets(tripletlist.begin(), tripletlist.end());
}
