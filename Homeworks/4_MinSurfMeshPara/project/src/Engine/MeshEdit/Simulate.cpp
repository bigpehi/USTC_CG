#include <Engine/MeshEdit/Simulate.h>


#include <Eigen/Sparse>

void Simulate::Clear() {
	this->positions.clear();
	this->velocity.clear();
}

bool Simulate::Init() {
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
	this->velocity_backup.resize(positions.size());
	for (int i = 0; i < positions.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			this->velocity[i][j] = 0;
			this->velocity_backup[i][j] = 0;
		}
	}



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
	for (int i = 0; i < positions.size(); i++)
	{
		if (positions[i][0] < x)
		{
			x = positions[i][0];
		}
	}

	for (int i = 0; i < positions.size(); i++)
	{
		if (abs(positions[i][0] - x) < 1e-5)
		{
			fixed_id.push_back(i);
		}
	}
	Init();
}

void Simulate::SimulateOnce() {
	ImplicitEuler();

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
	G(2) = 10;

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

	//MatrixXd M(F_ext.rows(), F_ext.rows());
	//M.setZero();
	//for (size_t i = 0; i < M.rows(); i++)
	//		M(i, i) = mass;
	Y = X + h * V + h * h / mass * F_ext;
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
			//std::cout << std::endl << "p1 - p2: " << p1_x - p2_x << "	" << p1_y - p2_y << "	" << p1_z - p2_z << std::endl; ////////

			// ���㲢�����ʵ��ܵ��ĵ�����ע�ⵯ������ͼӼ��Ĺ�ϵ
			Vector3d f_int_temp = stiff * (dist_p1p2 - rest_p1p2) * vector_p1p2 / dist_p1p2;
			//std::cout << std::endl << "f_int_temp: " << f_int_temp[0] << "	" << f_int_temp[1] << "	" << f_int_temp[2] << std::endl; ////////

			for (size_t k = 0; k < 3; k++)
			{
				std::vector<unsigned>::iterator it_p1 = find(fixed_id.begin(), fixed_id.end(), p1_index_in_positions);
				if (it_p1 == fixed_id.end())  // ˵��p1�㲻�ǹ̶���
				{
					f_int(p1_index_in_positions, k) -= f_int_temp(k);
					//std::cout << std::endl << "f_int(p1): " << f_int(p1_index_in_positions, k) << std::endl; ////////
				}
				std::vector<unsigned>::iterator it_p2 = find(fixed_id.begin(), fixed_id.end(), p2_index_in_positions);
				if (it_p2 == fixed_id.end())  // ˵��p2�㲻�ǹ̶���
				{
					f_int(p2_index_in_positions, k) += f_int_temp(k);
					//std::cout << std::endl << "f_int(p2): " << f_int(p2_index_in_positions, k) << std::endl; ////////
				}

			}

			// ���㵯����p1λ�õĵ���
			Matrix3d partial_temp = stiff * (rest_p1p2 / dist_p1p2 - 1) * Matrix3d::Identity() \
				- stiff * rest_p1p2 * pow(dist_p1p2, -3) * vector_p1p2 * vector_p1p2.transpose();

			//std::cout << std::endl << "partial_temp" << std::endl; //////
			//for (size_t i = 0; i < 3; i++)
				//std::cout << partial_temp(i, 0) << "	" << partial_temp(i, 1) << "	" << partial_temp(i, 2) << std::endl; //////

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

		g_x = mass * (X - Y) - h * h * f_int; // ����
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
		SparseLU<SparseMatrix<double>> solver;
		VectorXd X_temp(positions.size() * 3); X_temp.setZero();
		solver.compute(coeffsM_nabla_gxk);
		if (solver.info() != Success) {
			std::cout << "decomposition failed" << std::endl;
		}
		X_temp = solver.solve(b_gx);

		X_kplus1 = X_k - X_temp;
		X_k = X_kplus1;

		//std::cout << "error	" << X_temp.norm() / positions.size() << std::endl;

	}
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
