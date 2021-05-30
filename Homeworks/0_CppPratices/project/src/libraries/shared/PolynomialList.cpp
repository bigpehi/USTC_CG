#include "PolynomialList.h"
#include <iostream>
#include <fstream>
#include <list>
#include <cmath>
#include <assert.h>
#define EPSILON 1.0e-10	// zero double

using namespace std;

PolynomialList::PolynomialList(const PolynomialList& other) {
    auto it_other = other.m_Polynomial.begin();
    while (it_other != other.m_Polynomial.end()) {
        //cout << it_other->deg << endl;
        m_Polynomial.push_back(*it_other);
        it_other++;
    }
        
}

PolynomialList::PolynomialList(const string& file) {
    ReadFromFile(file);
}

PolynomialList::PolynomialList(const double* cof, const int* deg, int n) {
    for (int i = 0; i < n; i++)
        AddOneTerm(Term(deg[i], cof[i]));
}

PolynomialList::PolynomialList(const vector<int>& deg, const vector<double>& cof) {
    assert(deg.size() == cof.size());
    for (size_t i = 0; i < deg.size(); i++)
        AddOneTerm(Term(deg[i], cof[i]));
}

double PolynomialList::coff(int i) const {
    for (const Term& term : m_Polynomial) {
        if (term.deg > i)
            break;
        if (term.deg == i)
            return term.cof;
    }

    return 0.;
}

double& PolynomialList::coff(int i) {
    return AddOneTerm(Term(i, 0)).cof;
}

void PolynomialList::compress() {
    //Print();
    PolynomialList polynomialList_temp;
    auto it_m = m_Polynomial.begin();
    while (it_m != m_Polynomial.end()) {
        if (it_m->cof==0.)
        {
            it_m++;
            continue;
        }
        int deg_sum = it_m->deg;
        double cof_sum = 0;
        auto it_temp = it_m;
        while (it_temp!=m_Polynomial.end() && it_temp->deg == it_m->deg) {
            cof_sum += it_temp->cof;
            it_temp++;
        }
        Term t(deg_sum, cof_sum);
        polynomialList_temp.m_Polynomial.push_back(t);
        it_m = it_temp;
        //polynomialList_temp.Print();
    }
    m_Polynomial.clear();
    //Print();
    auto it_temp = polynomialList_temp.m_Polynomial.begin();
    while (it_temp != polynomialList_temp.m_Polynomial.end())
    {
        m_Polynomial.push_back(*it_temp);
        it_temp++;
        //Print();
    }
}

PolynomialList PolynomialList::operator+(const PolynomialList& right) const {
    PolynomialList polynomialList_temp(right);
    for (auto it = m_Polynomial.begin(); it != m_Polynomial.end();it++) {
        polynomialList_temp.AddOneTerm(*it);
    }
    polynomialList_temp.compress();
    return polynomialList_temp; // you should return a correct value
}

PolynomialList PolynomialList::operator-(const PolynomialList& right) const {
    PolynomialList polynomialList_temp(*this);
    for (auto it = right.m_Polynomial.begin(); it != right.m_Polynomial.end(); it++) {
        Term t(it->deg, -it->cof);
        polynomialList_temp.AddOneTerm(t);
    }
    polynomialList_temp.compress();
    return polynomialList_temp; // you should return a correct value
}

PolynomialList PolynomialList::operator*(const PolynomialList& right) const {
    PolynomialList product;
    auto it_m =m_Polynomial.begin();
    for (; it_m != m_Polynomial.end();it_m++) {
        auto it_right = right.m_Polynomial.begin();
        for (; it_right != right.m_Polynomial.end(); it_right++) {
            Term temp(it_m->deg + it_right->deg, it_m->cof * it_right->cof);
            product.m_Polynomial.push_back(temp);
        }
    }
    product.compress();
    return product; // you should return a correct value
}

PolynomialList& PolynomialList::operator=(const PolynomialList& right) {
    m_Polynomial.clear();
    auto it_right = right.m_Polynomial.begin();
    while (it_right!= right.m_Polynomial.end())    {
        m_Polynomial.push_back(*it_right);
        it_right++;
    }
    compress();
    return *this;
}

void PolynomialList::Print() const {
    //cout << "f(x) = ";
    auto it = m_Polynomial.begin();  //为什么不能用std::list<Term>::iterator换掉auto
    if (it == m_Polynomial.end()) {
        cout << "0" << endl;
        return;
    }
    if (it->deg != 1)
        cout << it->cof << "x^{" << it->deg << "}";
    else
        cout << it->cof << "x";

    it++;
    for (; it != m_Polynomial.end(); it++) {
        if (it->cof==0)
            continue;
        if (it->cof > 0)
            cout << "+ ";
        if (it->deg != 1)
            cout << it->cof << "x^{" << it->deg << "}";
        else
            cout << it->cof << "x";
    }
    cout << endl;
}

bool PolynomialList::ReadFromFile(const string& file) {
    ifstream f;
    f.open(file, ios::in);
    if (!f.is_open()) {
        cout << "读取文件"<< file <<"失败" << endl;
        return false;
    }
    char p; int cnt;
    f >> p; f >> cnt;
    int deg, cof;
    while (f>>deg && f>>cof)
    {
        Term term(deg, cof);
        AddOneTerm(term);
    }


    return true;
}

PolynomialList::Term& PolynomialList::AddOneTerm(const Term& term) {
    auto it = m_Polynomial.begin();  //为什么不能用std::list<Term>::iterator换掉auto
    if (it == m_Polynomial.end()) {
        m_Polynomial.push_back(term);
        return *m_Polynomial.begin();
    }
    while (it->deg > term.deg)
        it++;
    return *m_Polynomial.insert(it, term); // 返回插入数据的位置
}