#pragma once
#include <iostream>
#include <vector>
using namespace std;

struct point {
	vector<float> dim;
	point(vector<float>& dim_) : dim(dim_) {}
};
