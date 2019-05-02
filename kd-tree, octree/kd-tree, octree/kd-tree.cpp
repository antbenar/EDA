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

	Kdtree<T>* search_cuadrante(Kdtree<T>*, point<T>*);

	struct Compare {
		int dim;
		bool operator() (point<T>* a, point<T>* b) {
			return (a->dim[dim]< b->dim[dim]);
		}
	};
	Compare compare;

public:
	/*****this must be in private***/
	vector < point<T>* > points;
	Kdtree<T>* left;
	Kdtree<T>* right;

	void divide_tree(Kdtree<T>*);
	/********/

	Kdtree(double);
	Kdtree(vector<line<T>*>, int);
	void insert(point<T>*);
	bool search_point(point<T>*pt);
	void draw(Kdtree<T>*, int color = 255);
	void generate_points(int, int);
	~Kdtree<T>();
};

template <typename T>
Kdtree<T>::~Kdtree<T>()
{
	if ( left ) {
		delete left;
		delete right;
	}
}

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
bool Kdtree<T>::search_point(point<T>*pt) {
	bool equal = true;
	Kdtree<T>* cur_cuadrante = search_cuadrante(this, pt);
	if (!cur_cuadrante) return false;
	//draw(cur_cuadrante, 200);
	//cout << "cuadrante: " << cur_cuadrante->lines[0]->p_inicio->dim[0] << " " << cur_cuadrante->lines[0]->p_inicio->dim[1] << " " << cur_cuadrante->lines[0]->p_inicio->dim[2] << endl;


	for (int i = 0; i < cur_cuadrante->points.size(); ++i) {
		equal = true;
		for (int j = 0; j < cur_cuadrante->points[i]->dim.size(); ++j) {
			if (cur_cuadrante->points[i]->dim[j] != pt->dim[j]) {
				equal = false;
				break;
			}
		}
		if (equal) return true;
	}
	
	return false;
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

	//cout << "median in " << dim << " is: " << cur_value_dim  << endl;
	//--------------------------------------------cuadrante left
	vector < line<T>* > lines_aux = copy_lines(cur_cuadrante->lines);
	lines_aux[dim]->p_fin->dim[dim] = cur_value_dim;

	if (!cur_cuadrante->left)
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

	if (!cur_cuadrante->right)
		cur_cuadrante->right = new Kdtree<T>(lines_aux, cur_cuadrante->deep + 1);
	cur_cuadrante->right->points.insert(cur_cuadrante->right->points.begin(), cur_cuadrante->points.begin() + (int)(cur_cuadrante->points.size() / 2), cur_cuadrante->points.end() );

	(cur_cuadrante->points).clear();
	cur_cuadrante->dividido = true;

	divide_tree(cur_cuadrante->right);
	divide_tree(cur_cuadrante->left);
}

template <typename T>
void Kdtree<T>::draw(Kdtree<T>* cur_cuadrante, int color = 255) {
	if (!cur_cuadrante) return;

	glBegin(GL_LINES);
	if (color == 200) glColor3d(255, 0, 0);
	else glColor3d(255, 255, 255);
	
	double x_min = cur_cuadrante->lines[0]->p_inicio->dim[0];
	double x_max = cur_cuadrante->lines[0]->p_fin->dim[0];

	double y_min = cur_cuadrante->lines[1]->p_inicio->dim[1];
	double y_max = cur_cuadrante->lines[1]->p_fin->dim[1];

	double z_min = cur_cuadrante->lines[2]->p_inicio->dim[2];
	double z_max = cur_cuadrante->lines[2]->p_fin->dim[2];

	//----------------cara de arriba
	glVertex3d(x_min, y_min , z_min);
	glVertex3d(x_max, y_min, z_min);

	glVertex3d(x_max, y_min, z_min);
	glVertex3d(x_max, y_min, z_max);

	glVertex3d(x_max, y_min, z_max);
	glVertex3d(x_min, y_min, z_max);

	glVertex3d(x_min, y_min, z_min);
	glVertex3d(x_min, y_min, z_max);

	//----------------cara de abajo
	glVertex3d(x_min, y_max, z_min);
	glVertex3d(x_max, y_max, z_min);

	glVertex3d(x_max, y_max, z_min);
	glVertex3d(x_max, y_max, z_max);

	glVertex3d(x_max, y_max, z_max);
	glVertex3d(x_min, y_max, z_max);

	glVertex3d(x_min, y_max, z_min);
	glVertex3d(x_min, y_max, z_max);

	//----------------cara de frente
	glVertex3d(x_min, y_min, z_min);
	glVertex3d(x_min, y_max, z_min);

	glVertex3d(x_min, y_max, z_min);
	glVertex3d(x_max, y_max, z_min);

	glVertex3d(x_max, y_max, z_min);
	glVertex3d(x_max, y_min, z_min);

	glVertex3d(x_max, y_min, z_min);
	glVertex3d(x_min, y_min, z_min);

	//----------------cara de atras
	glVertex3d(x_min, y_min, z_max);
	glVertex3d(x_min, y_max, z_max);

	glVertex3d(x_min, y_max, z_max);
	glVertex3d(x_max, y_max, z_max);

	glVertex3d(x_max, y_max, z_max);
	glVertex3d(x_max, y_min, z_max);

	glVertex3d(x_max, y_min, z_max);
	glVertex3d(x_min, y_min, z_max);

	//----------------cara lateral izquierda
	glVertex3d(x_min, y_min, z_min);
	glVertex3d(x_min, y_min, z_max);
	
	glVertex3d(x_min, y_min, z_max);
	glVertex3d(x_min, y_max, z_max);

	glVertex3d(x_min, y_max, z_max);
	glVertex3d(x_min, y_max, z_min);
	
	glVertex3d(x_min, y_max, z_min);
	glVertex3d(x_min, y_min, z_min);

	//----------------cara lateral derecha
	glVertex3d(x_max, y_min, z_min);
	glVertex3d(x_max, y_min, z_max);

	glVertex3d(x_max, y_min, z_max);
	glVertex3d(x_max, y_max, z_max);

	glVertex3d(x_max, y_max, z_max);
	glVertex3d(x_max, y_max, z_min);

	glVertex3d(x_max, y_max, z_min);
	glVertex3d(x_max, y_min, z_min);

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