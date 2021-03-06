#include <iostream>
#include <algorithm>
#include <string>
#include<GL/glut.h>
#include <math.h>

#include "Rectangle.cpp"

#define PI 3.14159
using namespace std;

typedef vector<point*>::iterator iter;

class Rtree {
public:
	Rectangle* root;	
	int room;

	Rtree(int room_):room(room_) {
		root = new Rectangle;
	}

	void create( vector<point*> points ) {
		this->root->points = points;
		make_Rtree(root);
	}

	void insert( point* p ) {
		Rectangle* r = root;
		chooseLeaf(p, r);
		r->addPoint(p);
		if (r->numPoints() > room) {
			splitTree(r);
		}
		adjustTree(r->parent);
	}

	bool search( point* p ) {
		Rectangle* r = root;
		chooseLeaf(p,r);
		for (int i = 0; i < r->numPoints(); ++i) {
			if (p == r->points[i])
				return true;
		}
		return false;
	}

	void draw() {
		draw(root);
	}

	void drawSphere(float x, float y, float radius) {
		drawSphere_( x, y, radius);
	}


private:
	void make_Rtree(Rectangle*& r) {
		if (r->numPoints() > room) {
			splitTree(r);
		}
		adjustTree(r->parent);
		for (int i = 0; i < r->numContainers(); ++i) {
			make_Rtree(r->containers[i]);
		}
	}

	void chooseLeaf(point* p, Rectangle*& r) {	//search the point in the rectangles
		if (r->leaf) {
			return;
		}
		r = r->chose_container(p);
		chooseLeaf(p, r);
	}

	Rectangle* splitTree( Rectangle*& r) {	//Quadratic Split, returns the parrent of the splitted nodes
		point* leftp = nullptr;
		point* rightp = nullptr;

		peekSeedsLeaf(r, leftp, rightp);

		Rectangle* N = new Rectangle(leftp);
		Rectangle* NN = new Rectangle(rightp);

		while ( r->numPoints() > 0) {
			if (room / 2 - N->numPoints() == r->numPoints()) {
				N->addPoints(r->points);
				break;
			}
			else if (room / 2 - NN->numPoints() == r->numPoints()) {
				NN->addPoints(r->points);
				break;
			}
			peekNextLeaf(r->points[0], N, NN);
			r->points.erase(r->points.begin());
		}
		r->points.clear();

		if (r->parent == nullptr) {	//cuando r es la raiz
			root = new Rectangle();
			root->leaf = false;
			root->addContainer(N);
			root->addContainer(NN);
			N->parent = NN->parent = root;//
		}
		else {
			N->parent = NN->parent = r->parent;//
			r = N;
			r->parent->addContainer(NN);
		}
		return N;
	}

	void adjustTree(Rectangle*& r) {
		if (r == nullptr)
			return;
		
		if (r->numContainers() > room) {
			splitRectangles(r);
		}
		else {
			r->resize();	//ajustar para que cubra todos los otros rectangulos que contiene
		}
		adjustTree(r->parent);
	}

	void peekSeedsLeaf(Rectangle*& r, point*& leftp, point*& rightp) {
		float max_area = -INF;
		float area = 0;
		vector<point*>::iterator lp, rp;

		for (vector<point*>::iterator it = r->points.begin(); it != r->points.end(); ++it){
			for (vector<point*>::iterator it2 = it+1; it2 != r->points.end(); ++it2){
				area = areaPoints(*it,*it2);
				if (area > max_area) {
					max_area = area;
					lp = it;
					rp = it2;
				}
			}
		}

		leftp = *lp;
		rightp = *rp;
		r->points.erase(lp);
		r->points.erase(remove(r->points.begin(), r->points.end(), rightp));
	}

	void peekNextLeaf(point* p, Rectangle*& N, Rectangle*& NN) {
		if (N->area(p) < NN->area(p)) {
			N->addPoint(p);
		}
		else {
			NN->addPoint(p);
		}
	}

	float areaPoints(point*p1, point*p2) {	//calcular el area entre dos puntos
		float res = 1;
		for (int i = 0; i < p1->dim.size(); ++i) {
			res *= p1->dim[i] - p2->dim[i];
		}
		return abs(res);
	}

	//start adjust Tree
	Rectangle* splitRectangles(Rectangle* r) {
		Rectangle* N;
		Rectangle* NN;

		peekSeedsRectangle(r, N, NN);//Return rectangles with the leef rectangle added

		while (r->numContainers() > 0) {
			if (room / 2 - N->numContainers() == r->numContainers()) {
				N->addContainers(r->containers);
				break;
			}
			else if (room / 2 - NN->numContainers() == r->numContainers()) {
				NN->addContainers(r->containers);
				break;
			}
			peekNextRectangle(r->containers[0], N, NN);
			r->containers.erase(r->containers.begin());
		}
		r->containers.clear();
		
		if (r->parent == nullptr) {	//cuando r es la raiz
			root = new Rectangle();
			root->leaf = false;
			root->addContainer(N);
			root->addContainer(NN);
			N->parent = NN->parent = root;//
		}
		else {
			N->parent = NN->parent = r->parent;//
			r = N;
			r->parent->addContainer(NN);
		}
		return N;
	}


	void peekSeedsRectangle(Rectangle*& r, Rectangle*& leftp, Rectangle*& rightp) {
		float max_area = -INF;
		float area = 0;
		vector<Rectangle*>::iterator lp, rp;

		for (vector<Rectangle*>::iterator it = r->containers.begin(); it != r->containers.end(); ++it) {
			for (vector<Rectangle*>::iterator it2 = it + 1; it2 != r->containers.end(); ++it2) {
				area = (*it)->area((*it));
				if (area > max_area) {
					max_area = area;
					lp = it;
					rp = it2;
				}
			}
		}

		leftp = *lp;
		rightp = *rp;
		r->containers.erase(lp);
		r->containers.erase(remove(r->containers.begin(), r->containers.end(), rightp));
	}

	void peekNextRectangle(Rectangle* p, Rectangle*& N, Rectangle*& NN) {
		if (N->area(p) < NN->area(p)) {
			N->addContainer(p);
		}
		else {
			NN->addContainer(p);
		}
	}
	//end adjust tree

	void draw(Rectangle* cur) {
		//graphic Rectangle
		glBegin(GL_LINES);
		glColor3d(255,0,0);
		glVertex2d(cur->vertexes[0].first, cur->vertexes[1].first);
		glVertex2d(cur->vertexes[0].second, cur->vertexes[1].first);

		glVertex2d(cur->vertexes[0].second, cur->vertexes[1].first);
		glVertex2d(cur->vertexes[0].second, cur->vertexes[1].second);

		glVertex2d(cur->vertexes[0].second, cur->vertexes[1].second);
		glVertex2d(cur->vertexes[0].first, cur->vertexes[1].second);

		glVertex2d(cur->vertexes[0].first, cur->vertexes[1].second);
		glVertex2d(cur->vertexes[0].first, cur->vertexes[1].first);
		glEnd();

		//graphic points	
		glPointSize(2);
		glBegin(GL_POINTS);
		glColor3d(255, 255, 255);
		for (int i = 0; i < (cur->points).size(); ++i) {
			glVertex2f(cur->points[i]->dim[0], cur->points[i]->dim[1]);
		}
		glEnd();

		for (int i = 0; i< (cur->containers).size(); ++i)
			draw(cur->containers[i]);
	}

	
	float euclideanDistance(float cx, float cy, float x, float y)
	{
		float x1 = powf((x - cx), 2);
		float y1 = powf((y - cy), 2);
		return (x1 + y1);
	}
	
	bool pointInRadius(point* pt, float x, float y, float radius) {
		if (euclideanDistance(pt->dim[0], pt->dim[1], x, y) <= (radius*radius))
		{
			return true;
		}
		return false;
	}

	vector<point*> rangeSearch( Rectangle* cur_cuadrante, float x, float y, float radius) {
		vector<point*> result;

		if (!cur_cuadrante->intersect(x, y, radius)) {
			return result;
		}

		if (cur_cuadrante->leaf) {	// it is a leaf node
			for (int i = 0; i < cur_cuadrante->numPoints(); ++i) {
				if (pointInRadius(cur_cuadrante->points[i], x, y,radius))
					result.push_back(cur_cuadrante->points[i]);
			}
			return result;
		}

		for (int i = 0; i < cur_cuadrante->numContainers(); ++i) {
			vector<point*> aux = rangeSearch(cur_cuadrante->containers[i], x, y, radius);
			result.insert(result.end(), aux.begin(), aux.end());
		}

		return result;
	}

	void DrawCircle(float x, float y, float radio, int num_segments) {
		glBegin(GL_LINES);
		for (int i = 0; i < 100 + 1; i++) {  // +1 para cerrar
			glVertex2f(x + radio * cos(2.0 * PI * i / 100),
				y + radio * sin(2.0 * PI * i / 100));
		}
		glEnd();
	}

	void drawSphere_(float x, float y, float radius) {
		//-----------graphic sphere
		glColor3d(255, 255, 0);
		DrawCircle(x,y,radius, 100);

		//--------------------graphic points
		vector<point*> points_to_color = rangeSearch(root, x, y, radius);

		//cout << points_to_color.size() << endl;
		glPointSize(5);
		glBegin(GL_POINTS);
			glColor3d(0, 0, 255);
			for (int i = 0; i < points_to_color.size(); ++i) {
				//cout << points_to_color[i]->dim[0] << " - " << points_to_color[i]->dim[1] << endl;
				glVertex2f(points_to_color[i]->dim[0], points_to_color[i]->dim[1]);
			}
		glEnd();
	}

};