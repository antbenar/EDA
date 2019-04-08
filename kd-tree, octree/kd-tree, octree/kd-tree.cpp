#pragma once

#include <iostream>
#include <vector>
#include<GL/glut.h>
#include <algorithm>
#include "point.cpp"
using namespace std;

template <typename T>
class Kdtree {
private:
	vector<line<T>*> lines;
	
	bool dividido;
	int deep;

	void draw_rectangle(point<T>*, double, double, double);
	Kdtree<T>* search_cuadrante(Kdtree<T>*, point<T>*);

	struct Compare {
		int dim;
		bool operator() (point<T>* a, point<T>* b) {
			return (a->dim[dim]< b->dim[dim]);
		}
	};
	Compare compare;

public:
	Kdtree(double);
	Kdtree(vector<line<T>*> , int);
	/********/
	vector < point<T>* > points;
	Kdtree<T>* left;
	Kdtree<T>* right;

	void divide_tree(Kdtree<T>*);
	/********/
	void insert(point<T>*);

	void draw(Kdtree<T>*, int color = 255);
	void generate_points(int, int);
};



template <typename T>
Kdtree<T>::Kdtree(vector<line<T>*> lines_ , int deep_):	lines(lines_),
														deep(deep_),
														dividido(false), 
														left(nullptr), 
														right(nullptr){}


template <typename T>
Kdtree<T>::Kdtree(double size) : lines(num_dimensions, nullptr), dividido(false), left(nullptr), right(nullptr), deep(0)
{
	vector < vector< double > >p = {{ -size, size, size },
									{ size, size, size },
									{-size, size, size},
									{-size, -size, size},
									{-size, size, size},
									{-size, size, -size} };

	lines[0] = new line<T>(new point<T>(p[0]), new point<T>(p[1]));
	lines[1] = new line<T>(new point<T>(p[2]), new point<T>(p[3]));
	lines[2] = new line<T>(new point<T>(p[4]), new point<T>(p[5]));
}

template <typename T>
void Kdtree<T>::insert(point<T>* pt) {
	Kdtree<T>* cur_cuadrante = search_cuadrante(this, pt);
	cur_cuadrante->points.push_back(pt);
	divide_tree(cur_cuadrante);
}

template <typename T>
Kdtree<T>* Kdtree<T>::search_cuadrante(Kdtree<T>* current, point<T>*pt) {
	if (!current->dividido) {
		return current;
	}

	int dim = current->deep % num_dimensions;

	if ( pt->dim[dim] < (current->right->lines[dim]->p_inicio)->dim[dim] )
		return search_cuadrante(current->left, pt);
	else
		return search_cuadrante(current->right, pt);
}

template <typename T>
vector < line<T>* > copy_lines( vector < line<T>* > lines) {
	vector < line<T>* > lines_aux(lines.size(),nullptr);
	for (int i = 0; i < lines.size(); ++i) {
		lines_aux[i] = new line<T>( new point<T> (lines[i]->p_inicio->dim), new point<T>(lines[i]->p_fin->dim));
	}
	return lines_aux;
}


template <typename T>
void Kdtree<T>::divide_tree(Kdtree<T>* cur_cuadrante) {
	if (cur_cuadrante->points.size() <= max_points) return;

	int dim = cur_cuadrante->deep % num_dimensions;

	compare.dim = dim;
	sort(cur_cuadrante->points.begin(), cur_cuadrante->points.end(), compare);

	point<T>* median_point = cur_cuadrante->points[(int)(cur_cuadrante->points.size() / 2)];
	double cur_value_dim = median_point->dim[dim];

	//--------------------------------------------cuadrante left
	vector < line<T>* > lines_aux = copy_lines(cur_cuadrante->lines);
	lines_aux[dim]->p_fin->dim[dim] = cur_value_dim;

	cur_cuadrante->left = new Kdtree<T>(lines_aux, cur_cuadrante->deep + 1);
	cur_cuadrante->left->points.insert(cur_cuadrante->left->points.begin(), cur_cuadrante->points.begin(), cur_cuadrante->points.begin() + (int)(cur_cuadrante->points.size() / 2));

	//--------------------------------------------cuadrante right
	lines_aux = copy_lines(cur_cuadrante->lines);

	for (int i = 0; i < num_dimensions; ++i)
		lines_aux[i]->p_inicio->dim[dim] = cur_value_dim;

	for (int i = 0; i < num_dimensions; ++i) {
		if (i != dim)
			lines_aux[i]->p_fin->dim[dim] = cur_value_dim;
	}

	cur_cuadrante->right = new Kdtree<T>(lines_aux, cur_cuadrante->deep + 1);
	cur_cuadrante->right->points.insert(cur_cuadrante->right->points.begin(), cur_cuadrante->points.begin() + (int)(cur_cuadrante->points.size() / 2), cur_cuadrante->points.end() );

	(cur_cuadrante->points).clear();
	cur_cuadrante->dividido = true;

	divide_tree(cur_cuadrante->right);
	divide_tree(cur_cuadrante->left);
}

template <typename T>
void Kdtree<T>::draw_rectangle(point<T>* origen, double largo, double alto, double ancho) {
	double x = origen->dim[0];
	double y = origen->dim[1];
	double z = origen->dim[2];

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3d(255, 255, 255);

	glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
									  // Define vertices in counter-clockwise (CCW) order with normal pointing out
		
		//glLoadIdentity();

		glPushMatrix();

			//glTranslatef(x, y, z);

			// Top face(y = 1.0f)
			glVertex3d(0, 0, 0);
			glVertex3d(largo, 0, 0);
			glVertex3d(largo, 0, -ancho);
			glVertex3d(0, 0, -ancho);

			// Bottom face (y = -1.0f)
			glVertex3d(0, -alto, 0);
			glVertex3d(largo, -alto, 0);
			glVertex3d(largo, -alto, -ancho);
			glVertex3d(0, -alto, -ancho);

			// Front face  (z = 1.0f)
			glVertex3d(0, 0, 0);
			glVertex3d(0, -alto, 0);
			glVertex3d(largo, -alto, 0);
			glVertex3d(largo, 0, 0);

		glPopMatrix();

		
		/*
		
		// Back face (z = -1.0f)
		glVertex3f(1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);

		// Left face (x = -1.0f)
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);

		// Right face (x = 1.0f)
		glVertex3f(1.0f, 1.0f, -1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
		*/
	glEnd();
}

template <typename T>
void Kdtree<T>::draw(Kdtree<T>* cur_cuadrante, int color = 255) {
	if (!cur_cuadrante) return;
	//graphic lines
	double largo = cur_cuadrante->lines[0]->p_fin->dim[0] - cur_cuadrante->lines[0]->p_inicio->dim[0];
	double alto = cur_cuadrante->lines[1]->p_inicio->dim[1] - cur_cuadrante->lines[1]->p_fin->dim[1];
	double ancho = cur_cuadrante->lines[2]->p_inicio->dim[2] - cur_cuadrante->lines[2]->p_fin->dim[2];

	//draw_rectangle(cur_cuadrante->lines[0]->p_inicio, largo, alto, ancho);

	
	glBegin(GL_LINES);
	glColor3d(255, 255, 255);
	//x
	glVertex3d(cur_cuadrante->lines[0]->p_inicio->dim[0], cur_cuadrante->lines[0]->p_inicio->dim[1], cur_cuadrante->lines[0]->p_inicio->dim[2]);
	glVertex3d(cur_cuadrante->lines[0]->p_fin->dim[0], cur_cuadrante->lines[0]->p_fin->dim[1], cur_cuadrante->lines[0]->p_fin->dim[2]);

	//y
	glVertex3d(cur_cuadrante->lines[1]->p_inicio->dim[0], cur_cuadrante->lines[1]->p_inicio->dim[1], cur_cuadrante->lines[1]->p_inicio->dim[2]);
	glVertex3d(cur_cuadrante->lines[1]->p_fin->dim[0], cur_cuadrante->lines[1]->p_fin->dim[1], cur_cuadrante->lines[1]->p_fin->dim[2]);

	//z
	glVertex3d(cur_cuadrante->lines[2]->p_inicio->dim[0], cur_cuadrante->lines[2]->p_inicio->dim[1], cur_cuadrante->lines[2]->p_inicio->dim[2]);
	glVertex3d(cur_cuadrante->lines[2]->p_fin->dim[0], cur_cuadrante->lines[2]->p_fin->dim[1], cur_cuadrante->lines[2]->p_fin->dim[2]);


	//----------------cara de arriba
	//x
	/*
	glVertex3d(cur_cuadrante->lines[0]->p_inicio->dim[0], cur_cuadrante->lines[0]->p_inicio->dim[1], cur_cuadrante->lines[0]->p_inicio->dim[2]);
	glVertex3d(cur_cuadrante->lines[0]->p_fin->dim[0], cur_cuadrante->lines[0]->p_fin->dim[1], cur_cuadrante->lines[0]->p_fin->dim[2]);

	glVertex3d(cur_cuadrante->lines[0]->p_inicio->dim[0], cur_cuadrante->lines[0]->p_inicio->dim[1], cur_cuadrante->lines[0]->p_inicio->dim[2]);
	glVertex3d(cur_cuadrante->lines[0]->p_fin->dim[0], cur_cuadrante->lines[0]->p_fin->dim[1], cur_cuadrante->lines[0]->p_fin->dim[2]);
	*/


	glEnd();
	

	//graphic points
	glPointSize(5);
	glBegin(GL_POINTS);
		glColor3d(0, 255, 0);
		for (int i = 0; i < (cur_cuadrante->points).size(); ++i) {
			glVertex3f(cur_cuadrante->points[i]->dim[0], cur_cuadrante->points[i]->dim[1], cur_cuadrante->points[i]->dim[2]);
		}
	glEnd();


	draw(cur_cuadrante->left);
	draw(cur_cuadrante->right);
}


template <typename T>
void Kdtree<T>::generate_points(int max_size_point, int num_points) {
	for (int i = 0; i < num_points; ++i) {
		vector<double> pt = { (double)(rand() % max_size_point * 2 - max_size_point), (double)(rand() % max_size_point * 2 - max_size_point), (double)(rand() % max_size_point * 2 - max_size_point)};
		this->insert(new point<int>(pt, i));
	}
}
