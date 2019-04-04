#pragma once

#include <iostream>
#include <vector>
#include "point.cpp"
using namespace std;

template <typename T>
class Octree {
private:
	vector <Octree*> cuadrantes;
	double mid_x, mid_y, mid_z;
	double half_size_cube;
	bool dividido;

	vector < point<T>* > points;
	void divide_tree(Octree<T>*, int);
public:

	Octree(double , double , double , double );

	bool insert(point<T>*);
	void print(Octree<T>*, int);
	void draw(Octree<T>*);
};