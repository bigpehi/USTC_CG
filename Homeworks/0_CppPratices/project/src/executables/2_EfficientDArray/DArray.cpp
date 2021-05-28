// implementation of class DArray
#include <iostream>

#include "DArray.h"

#include <assert.h>

using namespace std;

// default constructor
DArray::DArray() {
	Init();
}

// set an array with default values
DArray::DArray(int nSize, double dValue):m_nSize(nSize){
	while (m_nMax < nSize)
		m_nMax *= 2;
	for (int i = 0; i < nSize; i++)
		m_pData[i] = dValue;
	for (int i = nSize; i < m_nMax; i++)
		m_pData[i] = 0.;
}

DArray::DArray(const DArray& arr):
	m_nSize(arr.m_nSize),m_nMax(arr.m_nMax),m_pData(new double[arr.m_nSize]){
	memcpy(m_pData, arr.m_pData, sizeof(double) * m_nSize);
}

// deconstructor
DArray::~DArray() {
	Free();
}

// display the elements of the array
void DArray::Print() const {
	cout << endl << "Size:" << m_nSize << "	MaxSize:" << m_nMax << endl;
	cout << "Content:	";
	for (int i = 0; i < m_nSize; i++)
		cout << m_pData[i] << "	";
}

// initilize the array
void DArray::Init() {
	m_pData = nullptr;
	m_nMax = 0;
	m_nSize = 0;
}

// free the array
void DArray::Free() {
	delete[] m_pData;
	m_pData = nullptr;
	m_nMax = 0;
	m_nSize = 0;
}

void DArray::Reserve(int nSize) {
	if (nSize == 0)
		return;
	if (nSize >= m_nMax) {
		int m_nMax_temp = 1;
		while (m_nMax_temp <= nSize)
			m_nMax_temp *= 2;
		double* m_pData_temp = new double[m_nMax_temp];
		memcpy(m_pData_temp, m_pData, sizeof(double)*m_nMax);
		for (int i = m_nSize; i < m_nMax_temp; i++)
			m_pData_temp[i] = 0.;
		delete[] m_pData;
		m_pData = m_pData_temp;
		m_nMax = m_nMax_temp;
	}
	if (nSize < m_nMax / 2) {
		int m_nMax_temp = m_nMax;
		while (m_nMax_temp < nSize)
			m_nMax_temp *= 2;
		double* m_pData_temp = new double[m_nMax];
		for (int i = 0; i < m_nMax_temp; i++) {
			m_pData_temp[i] = m_pData[i];
		}
		delete[] m_pData;
		m_pData = m_pData_temp;
		m_nMax = m_nMax_temp;
	}
}


// get the size of the array
int DArray::GetSize() const {
	return m_nSize; // you should return a correct value
}

// set the size of the array
void DArray::SetSize(int nSize) {
	Reserve(nSize);
	if (nSize < m_nSize) {
		for (int i = nSize; i < m_nSize; i++)
			m_pData[i] = 0.;
	}
	m_nSize = nSize;
}

// get an element at an index
const double& DArray::GetAt(int nIndex) const {
	assert(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; // you should return a correct value
}

// set the value of an element 
void DArray::SetAt(int nIndex, double dValue) {
	assert(nIndex >= 0 && nIndex <= m_nSize);
	m_pData[nIndex] = dValue;
}

// overload operator '[]'
double& DArray::operator[](int nIndex) {
	assert(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; // you should return a correct value
}

// overload operator '[]'
const double& DArray::operator[](int nIndex) const {
	assert(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; // you should return a correct value
}

// add a new element at the end of the array
void DArray::PushBack(double dValue) {
	Reserve(m_nSize+1);
	m_nSize++;
	m_pData[m_nSize - 1] = dValue;
}

// delete an element at some index
void DArray::DeleteAt(int nIndex) {
	assert(nIndex >= 0 && nIndex < m_nSize);
	Reserve(m_nSize - 1);
	for (int i = nIndex; i < m_nSize; i++)
		m_pData[i] = m_pData[i + 1];
	m_nSize--;
}

// insert a new element at some index
void DArray::InsertAt(int nIndex, double dValue) {
	assert(nIndex >= 0 && nIndex <= m_nSize);
	Reserve(m_nSize + 1);
	m_nSize++;
	for (int i = m_nSize; i > nIndex; i--)
		m_pData[i] = m_pData[i - 1];
	m_pData[nIndex] = dValue;
}

// overload operator '='
DArray& DArray::operator = (const DArray& arr){
	if (m_pData != nullptr) {
		delete[] m_pData;
		m_pData = nullptr;
	}
	m_nSize = arr.m_nSize;
	m_nMax = arr.m_nMax;
	m_pData = new double[arr.m_nSize];
	memcpy(m_pData, arr.m_pData, sizeof(double) * m_nSize);
	return *this;
}
