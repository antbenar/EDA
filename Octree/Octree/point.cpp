#pragma once
#include <iostream>
using namespace std;

template <typename T>
struct point {
	double x, y, z;
	T data;
	point(double x_, double y_, double z_, T data_) : x(x_), y(y_), z(z_), data(data_) {}
};