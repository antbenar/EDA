#pragma once
#include <iostream>
#include <vector>
using namespace std;

template <typename T>
struct point_ {
	double x, y, z;
	T data;
	point_(double x_, double y_, double z_, T data_) : x(x_), y(y_), z(z_), data(data_) {}
};

template <typename T>
struct point {
	vector<double> dim;
	T data;
	point(vector<double>& dim_, T data_) : dim(dim_), data(data_) {}
	point(vector<double>& dim_) : dim(dim_) {}
};

template <typename T>
struct line {
	point<T>* p_inicio;
	point<T>* p_fin;
	line(point<T>* p_inicio_, point<T>* p_fin_) : p_inicio(p_inicio_), p_fin(p_fin_) {}
};