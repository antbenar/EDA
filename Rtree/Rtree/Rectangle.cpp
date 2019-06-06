#include <iostream>
#include <algorithm>
#include <string>
#include "point.cpp"

#define INF 1000.00

using namespace std;

class Rectangle {
public:
	vector<point*> points;					//if it is of size 0, it is a leaf
	vector<pair<float, float>> vertexes;	//for each dimension, the min and the max
	vector<Rectangle*> containers;
	Rectangle* parent;
	bool leaf;

	Rectangle() : leaf(true), parent(nullptr), vertexes(vector<pair<float, float>>(2,make_pair(INF,-INF))) {}

	Rectangle(vector<point*>& points_) : Rectangle(){
		if (points_.size() == 0) {
			return;
		}
		addPoints(points_);
	}

	Rectangle(point* p) : Rectangle() {
		addPoint(p);
	}

	vector<point*> getPoints() {
		return points;
	}

	int numPoints() {
		return points.size();
	}

	int numContainers() {
		return containers.size();
	}

	Rectangle* chose_container(point* p){	//choose the smallest rectangle that includes p 
		Rectangle* res = containers[0];
		float min_area = INF;
		float area;
		for (int i = 0; i < containers.size(); ++i) {
			area = containers[i]->area(p);
			if ( area < min_area) {
				min_area = area;
				res = containers[i];
			}
		}
		return res;
	}

	float area(point*p) {	//calculate area of the rectangle plus the new point
		float res = 1;
		vector<pair<float, float>> vetxs(vertexes);

		for (int i = 0; i < vetxs.size(); ++i) {
			if (p->dim[i] < vetxs[i].first) vetxs[i].first = p->dim[i];
			else if (p->dim[i] > vetxs[i].second) vetxs[i].second = p->dim[i];
		}
		for (int i = 0; i < vetxs.size(); ++i) {
			res *= vetxs[i].second - vetxs[i].first;
		}
		return abs(res);
	}

	float area(Rectangle *p) {	//calculate area of the rectangle plus the new rectangle
		float res = 1;
		vector<pair<float, float>> vetxs(vertexes);

		for (int i = 0; i < vetxs.size(); ++i) {
			if (p->vertexes[i].first < vetxs[i].first) vetxs[i].first = p->vertexes[i].first;
			else if (p->vertexes[i].second > vetxs[i].second) vetxs[i].second = p->vertexes[i].second;
		}
		for (int i = 0; i < vetxs.size(); ++i) {
			res *= vetxs[i].second - vetxs[i].first;
		}
		return abs(res);
	}

	void addPoint(point*p) {			//resize rectangle plus one point
		points.push_back(p);
		for (int i = 0; i < vertexes.size(); ++i) {
			if (p->dim[i] < vertexes[i].first) vertexes[i].first = p->dim[i];
			if (p->dim[i] > vertexes[i].second) vertexes[i].second = p->dim[i];
		}
	}

	void addPoints(vector<point*>& p) {	//resize rectangle plus new points
		int i = points.size();
		points.insert(points.end(), p.begin(),p.end());

		for (; i < points.size(); ++i) {
			for (int j = 0; j < vertexes.size(); ++j) {
				if (points[i]->dim[j] > vertexes[j].second)
					vertexes[j].second = points[i]->dim[j];
				if (points[i]->dim[j] < vertexes[j].first)
					vertexes[j].first = points[i]->dim[j];
			}
		}
	}

	void addContainer(Rectangle *r) {			//resize rectangle plus one new rectangle
		containers.push_back(r);
		for (int j = 0; j < vertexes.size(); ++j) {
			if (r->vertexes[j].second > vertexes[j].second)
				vertexes[j].second = r->vertexes[j].second;
			if (r->vertexes[j].first < vertexes[j].first)
				vertexes[j].first = r->vertexes[j].first;
		}
	}

	void addContainers(vector<Rectangle*>& r) {			//resize rectangle plus one new rectangle
		int i = containers.size();
		containers.insert(containers.end(), r.begin(), r.end());

		for (; i < points.size(); ++i) {
			for (int j = 0; j < vertexes.size(); ++j) {
				if (containers[i]->vertexes[j].second > vertexes[j].second)
					vertexes[j].second = containers[i]->vertexes[j].second;
				if (containers[i]->vertexes[j].first < vertexes[j].first)
					vertexes[j].first = containers[i]->vertexes[j].first;
			}
		}
	}


	void resize() {
		for (int i = 0; i < containers.size(); ++i) {
			for (int j = 0; j < vertexes.size(); ++j) {
				if (containers[i]->vertexes[j].second > vertexes[j].second)
					vertexes[j].second = containers[i]->vertexes[j].second;
				if (containers[i]->vertexes[j].first < vertexes[j].first)
					vertexes[j].first = containers[i]->vertexes[j].first;
			}
		}
	}

	~Rectangle()
	{
		for(int i=0; i<containers.size();++i)
			delete containers[i];

		for (int i = 0; i<points.size(); ++i)
			delete points[i];
	}
};