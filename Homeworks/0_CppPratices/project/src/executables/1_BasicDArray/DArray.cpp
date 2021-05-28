// implementation of class DArray
#include<iostream>
#include <cassert>
#include "DArray.h"
using namespace std;
// default constructor
DArray::DArray() {
	//默认构造函数
	Init();
}

// set an array with default values
DArray::DArray(int nSize, double dValue):m_pData(new double[nSize]),m_nSize(nSize){
	//有参构造函数
	for (int i = 0; i < nSize; i++) {
		m_pData[i] = dValue;
	}
	
}

DArray::DArray(const DArray& arr):m_pData(new double[arr.m_nSize]),m_nSize(arr.m_nSize){
	//拷贝构造函数
	for (int i = 0; i < m_nSize; i++) {
		m_pData[i] = arr.m_pData[i];
	}
}

// deconstructor
DArray::~DArray() {
	//析构函数
	Free();
}

// display the elements of the array
void DArray::Print() const {
	cout << "size = " << m_nSize << ":";
	for (int i = 0; i < m_nSize; i++) {
		cout << m_pData[i] << "	";
	}
	cout << endl;
}

// initilize the array
void DArray::Init() {
	m_nSize = 0;
	m_pData = nullptr;
}

// free the array
void DArray::Free() {
	delete[] m_pData;
	m_pData = nullptr;
	m_nSize = 0;
}

// get the size of the array
int DArray::GetSize() const {
	return m_nSize;
}

// set the size of the array
void DArray::SetSize(int nSize){
	double* m_pData_temp = new double[nSize];
	if (nSize >= m_nSize) {
		for (int i = 0; i < m_nSize; i++)
			m_pData_temp[i] = m_pData[i];
		for (int i = m_nSize; i < nSize; i++)
			m_pData_temp[i] = 0.;
	}
	else {
		for (int i = 0; i < nSize; i++)
			m_pData_temp[i] = m_pData[i];
	}
	delete[] m_pData; // star 忘记释放
	m_pData = m_pData_temp;
	m_nSize = nSize;
}

// get an element at an index
const double& DArray::GetAt(int nIndex) const {
	assert(nIndex >= 0 && nIndex < m_nSize); //star 断言
	return m_pData[nIndex];
}

// set the value of an element 
void DArray::SetAt(int nIndex, double dValue) {
	assert(nIndex >= 0 && nIndex < m_nSize);
	m_pData[m_nSize] = dValue;
}

// overload operator '[]'
const double& DArray::operator[](int nIndex) const {
	assert(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex];
}

// add a new element at the end of the array
void DArray::PushBack(double dValue) {
	double* m_pData_temp = new double[m_nSize + 1];
	for (int i = 0; i < m_nSize; i++)
		m_pData_temp[i] = m_pData[i];
	m_pData_temp[m_nSize] = dValue;
	m_nSize++;
	delete[] m_pData;
	m_pData = m_pData_temp;
}

// delete an element at some index
void DArray::DeleteAt(int nIndex) {
	assert(nIndex >= 0 && nIndex < m_nSize);
	double* m_pData_temp = new double[m_nSize - 1];
	for (int i = 0; i < nIndex; i++)
		m_pData_temp[i] = m_pData[i];
	for (int i = nIndex; i+1 < m_nSize; i++)
		m_pData_temp[i] = m_pData[i+1];
	m_nSize--;
	delete[] m_pData;
	m_pData = m_pData_temp;
}

// insert a new element at some index
void DArray::InsertAt(int nIndex, double dValue) {
	assert(nIndex >= 0 && nIndex <= m_nSize);
	double* m_pData_temp = new double[m_nSize + 1];
	for (int i = 0; i < nIndex; i++)
		m_pData_temp[i] = m_pData[i];
	m_pData_temp[nIndex] = dValue;
	for (int i = nIndex; i < m_nSize; i++)
		m_pData_temp[i + 1] = m_pData[i];
	m_nSize++;
	delete[] m_pData;
	m_pData = m_pData_temp;
}

// overload operator '=' , arr is the right value.
DArray& DArray::operator = (const DArray& arr) {
	delete[] m_pData;
	m_pData = new double[arr.m_nSize];
	for (int i = 0; i < arr.m_nSize; i++) {
		m_pData[i] = arr[i];
	}
	m_nSize = arr.m_nSize;
	return *this;
}
