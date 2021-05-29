#pragma once
#pragma once
// implementation of class DArray
#include <iostream>

#include <assert.h>

using namespace std;


// interfaces of Dynamic Array class DArray
template <class DataType>
class DArray {
public:
	DArray(); // default constructor
	DArray(int nSize, DataType dValue = 0); // set an array with default values
	DArray(const DArray& arr); // copy constructor
	~DArray(); // deconstructor

	void Print() const; // print the elements of the array

	int GetSize() const; // get the size of the array
	void SetSize(int nSize); // set the size of the array

	const DataType& GetAt(int nIndex) const; // get an element at an index
	void SetAt(int nIndex, DataType dValue); // set the value of an element

	DataType& operator[](int nIndex); // overload operator '[]'
	const DataType& operator[](int nIndex) const; // overload operator '[]'

	void PushBack(DataType dValue); // add a new element at the end of the array
	void DeleteAt(int nIndex); // delete an element at some index
	void InsertAt(int nIndex, DataType dValue); // insert a new element at some index

	DArray& operator = (const DArray& arr); //overload operator '='

private:
	DataType* m_pData; // the pointer to the array memory
	int m_nSize; // the size of the array
	int m_nMax;

private:
	void Init(); // initilize the array
	void Free(); // free the array
	void Reserve(int nSize); // allocate enough memory
};




// default constructor
template <class DataType>
DArray<DataType>::DArray() {
	Init();
}

// set an array with default values
template <class DataType>
DArray<DataType>::DArray(int nSize, DataType dValue) :m_nSize(nSize) {
	while (m_nMax < nSize)
		m_nMax *= 2;
	for (int i = 0; i < nSize; i++)
		m_pData[i] = dValue;
	for (int i = nSize; i < m_nMax; i++)
		m_pData[i] = 0.;
}

template <class DataType>
DArray<DataType>::DArray(const DArray& arr) :
	m_nSize(arr.m_nSize), m_nMax(arr.m_nMax), m_pData(new DataType[arr.m_nSize]) {
	memcpy(m_pData, arr.m_pData, sizeof(DataType) * m_nSize);
}

// deconstructor
template <class DataType>
DArray<DataType>::~DArray() {
	Free();
}

// display the elements of the array
template <class DataType>
void DArray<DataType>::Print() const {
	cout << endl << "Size:" << m_nSize << "	MaxSize:" << m_nMax << endl;
	cout << "Content:	";
	for (int i = 0; i < m_nSize; i++)
		cout << m_pData[i] << "	";
}

// initilize the array
template <class DataType>
void DArray<DataType>::Init() {
	m_pData = nullptr;
	m_nMax = 0;
	m_nSize = 0;
}

// free the array
template <class DataType>
void DArray<DataType>::Free() {
	delete[] m_pData;
	m_pData = nullptr;
	m_nMax = 0;
	m_nSize = 0;
}

template <class DataType>
void DArray<DataType>::Reserve(int nSize) {
	if (nSize == 0)
		return;
	if (nSize >= m_nMax) {
		int m_nMax_temp = 1;
		while (m_nMax_temp <= nSize)
			m_nMax_temp *= 2;
		DataType* m_pData_temp = new DataType[m_nMax_temp];
		memcpy(m_pData_temp, m_pData, sizeof(DataType) * m_nMax);
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
		DataType* m_pData_temp = new DataType[m_nMax];
		for (int i = 0; i < m_nMax_temp; i++) {
			m_pData_temp[i] = m_pData[i];
		}
		delete[] m_pData;
		m_pData = m_pData_temp;
		m_nMax = m_nMax_temp;
	}
}


// get the size of the array
template <class DataType>
int DArray<DataType>::GetSize() const {
	return m_nSize; // you should return a correct value
}

// set the size of the array
template <class DataType>
void DArray<DataType>::SetSize(int nSize) {
	Reserve(nSize);
	if (nSize < m_nSize) {
		for (int i = nSize; i < m_nSize; i++)
			m_pData[i] = 0.;
	}
	m_nSize = nSize;
}

// get an element at an index
template <class DataType>
const DataType& DArray<DataType>::GetAt(int nIndex) const {
	assert(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; // you should return a correct value
}

// set the value of an element 
template <class DataType>
void DArray<DataType>::SetAt(int nIndex, DataType dValue) {
	assert(nIndex >= 0 && nIndex <= m_nSize);
	m_pData[nIndex] = dValue;
}

// overload operator '[]'
template <class DataType>
DataType& DArray<DataType>::operator[](int nIndex) {
	assert(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; // you should return a correct value
}

// overload operator '[]'
template <class DataType>
const DataType& DArray<DataType>::operator[](int nIndex) const {
	assert(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; // you should return a correct value
}

// add a new element at the end of the array
template <class DataType>
void DArray<DataType>::PushBack(DataType dValue) {
	Reserve(m_nSize + 1);
	m_nSize++;
	m_pData[m_nSize - 1] = dValue;
}

// delete an element at some index
template <class DataType>
void DArray<DataType>::DeleteAt(int nIndex) {
	assert(nIndex >= 0 && nIndex < m_nSize);
	Reserve(m_nSize - 1);
	for (int i = nIndex; i < m_nSize; i++)
		m_pData[i] = m_pData[i + 1];
	m_nSize--;
}

// insert a new element at some index
template <class DataType>
void DArray<DataType>::InsertAt(int nIndex, DataType dValue) {
	assert(nIndex >= 0 && nIndex <= m_nSize);
	Reserve(m_nSize + 1);
	m_nSize++;
	for (int i = m_nSize; i > nIndex; i--)
		m_pData[i] = m_pData[i - 1];
	m_pData[nIndex] = dValue;
}

// overload operator '='
template <class DataType>
DArray<DataType>& DArray<DataType>::operator = (const DArray<DataType>& arr) {
	if (m_pData != nullptr) {
		delete[] m_pData;
		m_pData = nullptr;
	}
	m_nSize = arr.m_nSize;
	m_nMax = arr.m_nMax;
	m_pData = new DataType[arr.m_nSize];
	memcpy(m_pData, arr.m_pData, sizeof(DataType) * m_nSize);
	return *this;
}
