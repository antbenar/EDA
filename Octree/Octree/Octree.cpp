#pragma once

#include <iostream>
#include <vector>
#include<GL/glut.h>
#include "point.cpp"
using namespace std;

static int max_points = 1;
static vector< vector<double> > mult_size = {
	{ -1,  1,  1 },
	{ 1,  1,  1 },
	{ -1, -1,  1 },
	{ 1, -1,  1 },

	{ -1,  1, -1 },
	{ 1,  1, -1 },
	{ -1, -1, -1 },
	{ 1, -1, -1 }
};

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

	Octree(double, double, double, double);

	void insert(point<T>*);
	void print(Octree<T>*, int);
	void draw(Octree<T>*);
	void generate_points(int);
};


template <typename T>
Octree<T>::Octree(double mid_x_, double mid_y_, double mid_z_, double half_size_cube_):mid_x(mid_x_), mid_y(mid_y_), mid_z(mid_z_), half_size_cube(half_size_cube_), cuadrantes(8, nullptr), dividido(false) {}


template <typename T>
void Octree<T>::insert(point<T>* pt) {
	points.push_back(pt);
	divide_tree(this, 0);
}

template <typename T>
void Octree<T>::divide_tree(Octree<T>* cur_cuadrante, int max) {
	if (max > 60) return;
	if (!cur_cuadrante) return;
	if ((cur_cuadrante->points).size() == 0) return;
	if (!(cur_cuadrante->dividido) && (cur_cuadrante->points).size() <= max_points) return;

	for (int i = 0; i<(cur_cuadrante->points).size(); ++i) {
		//calculo pos_cuadrante
		int pos_cuadrante = 0;
		if ((cur_cuadrante->points)[i]->x > cur_cuadrante->mid_x)  pos_cuadrante += 1;
		if ((cur_cuadrante->points)[i]->y < cur_cuadrante->mid_y)  pos_cuadrante += 2;
		if ((cur_cuadrante->points)[i]->z < cur_cuadrante->mid_z)  pos_cuadrante += 4;
		//fin calculo pos_cuadrante

		//cout << "__________________" << cur_cuadrante->mid_x << " " << cur_cuadrante->half_size_cube << endl;
		//cout << "__________________" << pos_cuadrante << endl;

		if (!(cur_cuadrante->cuadrantes)[pos_cuadrante]) {
			double size = (cur_cuadrante->half_size_cube) / 2;
			(cur_cuadrante->cuadrantes)[pos_cuadrante] = new Octree((cur_cuadrante->mid_x) + size*mult_size[pos_cuadrante][0], (cur_cuadrante->mid_y) + size*mult_size[pos_cuadrante][1], (cur_cuadrante->mid_z) + size*mult_size[pos_cuadrante][2], size);
			//cout << pos_cuadrante << " " <<(cur_cuadrante->mid_x) + size*mult_size[pos_cuadrante][0] <<" " << (cur_cuadrante->mid_y) + size*mult_size[pos_cuadrante][1]<<" " << (cur_cuadrante->mid_z) + size*mult_size[pos_cuadrante][2]<< " " << size << endl;
		}

		((cur_cuadrante->cuadrantes)[pos_cuadrante]->points).push_back((cur_cuadrante->points)[i]);

	}

	(cur_cuadrante->dividido) = true;
	(cur_cuadrante->points).clear();
	for (int i = 0; i< 8; ++i) divide_tree(cur_cuadrante->cuadrantes[i], max + 1);

}


template <typename T>
void Octree<T>::print(Octree<T>* cur_cuadrante, int nivel) {
	cout << endl << " nivel " << nivel << ": ";
	for (int i = 0; i< (cur_cuadrante->points).size(); ++i)
		cout << (cur_cuadrante->points[i])->data << ", ";

	for (int i = 0; i< (cur_cuadrante->cuadrantes).size(); ++i)
		if (cur_cuadrante->cuadrantes[i])
			print(cur_cuadrante->cuadrantes[i], nivel + 1);
}

template <typename T>
void Octree<T>::draw(Octree<T>* cur_cuadrante) {
	//graphic lines
	double mid_x_ = cur_cuadrante->mid_x;
	double mid_y_ = cur_cuadrante->mid_y;
	double mid_z_ = cur_cuadrante->mid_z;
	
	glPushMatrix();
 	    glColor3d(255, 255, 255);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glTranslatef(mid_x_, mid_y_, mid_z_);
		glutSolidCube(cur_cuadrante->half_size_cube*2);
	glPopMatrix();
	
	/*
	glBegin(GL_LINES);
		glColor3d(255, 0, 0); 
		glVertex3d(mid_x_ - cur_cuadrante->half_size_cube, mid_y_, mid_z_);
		glVertex3d(mid_x_ + cur_cuadrante->half_size_cube, mid_y_, mid_z_);

		glColor3d(0, 255, 0);
		glVertex3d(mid_x_, mid_y_ - cur_cuadrante->half_size_cube, mid_z_);
		glVertex3d(mid_x_, mid_y_ + cur_cuadrante->half_size_cube, mid_z_);

		glColor3d(0, 0, 255);
		glVertex3d(mid_x_, mid_y_, mid_z_ - cur_cuadrante->half_size_cube);
		glVertex3d(mid_x_, mid_y_, mid_z_ + cur_cuadrante->half_size_cube);
	glEnd();
	*/

	//graphic points
	glPointSize(5);
	glBegin(GL_POINTS);
		glColor3d(0, 255, 0);
		for (int i = 0; i < (cur_cuadrante->points).size(); ++i) {
			glVertex3f(cur_cuadrante->points[i]->x, cur_cuadrante->points[i]->y, cur_cuadrante->points[i]->z);
		}		
	glEnd();

	for (int i = 0; i< (cur_cuadrante->cuadrantes).size(); ++i)
		if (cur_cuadrante->cuadrantes[i])
			draw(cur_cuadrante->cuadrantes[i]);
}


template <typename T>
void Octree<T>::generate_points( int num_points ) {
	for (int i = 0; i < num_points; ++i) {
		octree->insert(new point<int>(rand() % (int)half_size_cube*2 - (int)half_size_cube, rand() % (int)half_size_cube*2 - (int)half_size_cube, rand() % (int)half_size_cube*2 - (int)half_size_cube, i));
	}
}