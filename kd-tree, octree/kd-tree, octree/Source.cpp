#define GLUT_DISABLE_ATEXIT_HACK
#include <windows.h>
#include <iostream>
#include <math.h>
#include<GL/glut.h>
#include <time.h>       
#include <fstream>
#include <string>
#include <chrono>

#include "Octree.cpp"
#include "kd-tree.cpp"
#include "point.cpp"

#define KEY_ESC 27

using namespace std;

int mx = 0;
int my = 0;

float ax = 0.0;
float ay = 0.0;
float speed = 0.1;

int max_points = 1;
int num_dimensions = 3;

Octree <int>* octree;
Kdtree <int>* kdtree;

bool r = false;

void OnMouseClick(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		mx = x;
		my = y;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		r = true;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
		r = false;
	}
}

void OnMouseMotion(int x, int y)
{
	int dx = mx - x;
	int dy = my - y;

	mx = x;
	my = y;

	ax += dx*speed;
	ay += dy*speed;
}



void idle() { // AGREGAR ESTA FUNCION
	glutPostRedisplay();
}

//funcion llamada a cada imagen
void glPaint_Octree(void) {

	//El fondo de la escena al color initial
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //CAMBIO
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 1.0, 500.0);

	glTranslatef(0, 0, -100.0);
	glRotatef(ax, 0, 1, 0);
	glRotatef(ay, 1, 0, 0);

	octree->draw(octree);

	//doble buffer, mantener esta instruccion al fin de la funcion
	glutSwapBuffers();
}

//funcion llamada a cada imagen
void glPaint_kdtree(void) {

	//El fondo de la escena al color initial
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //CAMBIO
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 1.0, 500.0);

	glTranslatef(0, 0, -100.0);
	glRotatef(ax, 0, 1, 0);
	glRotatef(ay, 1, 0, 0);

	kdtree->draw(kdtree);
	//kdtree->draw(kdtree->right);
	//kdtree->draw(kdtree->left);

	//doble buffer, mantener esta instruccion al fin de la funcion
	glutSwapBuffers();
}

//
//inicializacion de OpenGL
//
void init_GL(void) {
	//Color del fondo de la escena
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //(R, G, B, transparencia) en este caso un fondo negro
	glEnable(GL_DEPTH_TEST);
	//modo projeccion
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

//en el caso que la ventana cambie de tamaño
GLvoid window_redraw(GLsizei width, GLsizei height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

GLvoid window_key(unsigned char key, int x, int y) {
	switch (key) {
	case KEY_ESC:
		exit(0);
		break;

	default:
		break;
	}

}

void create_window_Octree() {
	//Inicializacion de la GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600); //tamaño de la ventana
	glutInitWindowPosition(100, 100); //posicion de la ventana
	glutCreateWindow("OcTree"); //titulo de la ventana

	init_GL(); //funcion de inicializacion de OpenGL

	glutDisplayFunc(glPaint_Octree);
	glutReshapeFunc(&window_redraw);
	// Callback del teclado
	glutKeyboardFunc(&window_key);
	glutMouseFunc(&OnMouseClick);
	glutMotionFunc(&OnMouseMotion);
	glutIdleFunc(&idle);
}

void create_window_kdtree() {
	//Inicializacion de la GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600); //tamaño de la ventana
	glutInitWindowPosition(710, 100); //posicion de la ventana
	glutCreateWindow("kdTree"); //titulo de la ventana

	init_GL(); //funcion de inicializacion de OpenGL

	glutDisplayFunc(glPaint_kdtree);
	glutReshapeFunc(&window_redraw);
	// Callback del teclado
	glutKeyboardFunc(&window_key);
	glutMouseFunc(&OnMouseClick);
	glutMotionFunc(&OnMouseMotion);
	glutIdleFunc(&idle);
}
//
//el programa principal
//

template <typename T>
vector<point<T>*> generate_points(int max_size_point, int num_points) {
	vector<point<T>*> result;
	//srand(time(NULL));
	for (int i = 0; i < num_points; ++i) {
		vector<double> pt = { (double)(rand() % max_size_point * 2 - max_size_point), (double)(rand() % max_size_point * 2 - max_size_point), (double)(rand() % max_size_point * 2 - max_size_point) };
		result.push_back(new point<T>(pt));
		//cout << "x: "<< pt[0] << ", y: " << pt[1] << ", z: " << pt[2] << endl;
	}
	return result;
}

void octree_against_kdtree_insert(int max_size, int number_points) {
	vector<point<int>*> points = generate_points<int>(max_size, number_points);
	octree->points = points;
	kdtree->points = points;
	
	clock_t t;
	
	cout << "Number of points: " << number_points << endl;
	t = clock();
		octree->divide_tree(octree);
	t = clock() - t;
	cout << "Time to divide Octree: " << ((float)t) / CLOCKS_PER_SEC << endl;
	
	t = clock();
		kdtree->divide_tree(kdtree);
	t = clock() - t;
	cout << "Time to divide Kd-tree: " << ((float)t) / CLOCKS_PER_SEC << endl;
}


template <typename T>
vector<point<T>*> read_from_image(string file, double scale) {
	char c;
	string aux;
	double x, y, z;
	vector<point<T>*> result;

	ifstream ifs(file.c_str(), ifstream::in);
	c = ifs.get();
	while (c != '#') {
		if (c == 'v') {
			aux = "";
			c = ifs.get();//read blank space
			while ((c = ifs.get()) != ' ') {
				aux += c;
			}
			x = atof(aux.c_str());

			aux = "";
			while ((c = ifs.get()) != ' ') {
				aux += c;
			}
			y = atof(aux.c_str());

			aux = "";
			while ((c = ifs.get()) != 'v' && c != '#') {
				aux += c;
			}
			z = atof(aux.c_str());
			vector<double> pt = { x*scale, y*scale , z*scale };
			result.push_back(new point<T>(pt));
		}
		else c = ifs.get();
	}
	return result;
}

void insert_from_file(string file, double percent , double scale, vector<point<int>*> image) {
	vector<point<int>*> points(image.begin(), image.begin() + image.size()*percent);
	octree->points = points;
	kdtree->points = points;

	cout << "Number of points: " << points.size() << endl;
	
	auto start = chrono::steady_clock::now();

	octree->divide_tree(octree);

	auto end = chrono::steady_clock::now();
	auto diff = end - start;
	cout << "Time to divide Octree: " << chrono::duration <double, milli>(diff).count() << endl;


	start = chrono::steady_clock::now();

	kdtree->divide_tree(kdtree);

	end = chrono::steady_clock::now();
	diff = end - start;
	cout << "Time to divide Kd-tree: " << chrono::duration <double, milli>(diff).count() << endl;
}



void octree_against_kdtree_search_point(int max_size, int number_iterations, vector<point<int>*> points) {
	//vector<point<int>*> points = generate_points<int>(max_size, number_iterations);

	cout << "Number of searched points: " << number_iterations << endl;
	
	auto start = chrono::steady_clock::now();

	for (int i = 0; i < points.size(); ++i)
		octree->search_point(points[i]);

	auto end = chrono::steady_clock::now();
	auto diff = end - start;
	cout << "Time average to search in Octree: " << (chrono::duration <double, milli>(diff).count() / (double)number_iterations) << endl;

	/*
	start = chrono::steady_clock::now();
	for (int i = 0; i < points.size(); ++i) 
		kdtree->search_point(points[i]);
	end = chrono::steady_clock::now();
	diff = end - start;
	cout << "Time average to search in Kd-tree: " << (chrono::duration <double, milli>(diff).count() / (double)number_iterations)  << endl;
	*/

}

void compare(string file, int max_size, int num_searchs, double scale) {
	for (int i = 1; i <= 4; ++i) {
		vector<point<int>*> image = read_from_image<int>(file, scale);
		max_points = 0.05*image.size();

		octree = new Octree<int>(0, 0, 0, max_size);
		kdtree = new Kdtree<int>(max_size);

		cout << "-- percent of image -> " << 0.25*i << endl;
		insert_from_file(file, 0.25*i, scale, image);

		vector<point<int>*> points_to_compare_search = generate_points<int>(max_size, num_searchs);
		octree_against_kdtree_search_point(max_size, num_searchs, points_to_compare_search);
	}
}



vector<string> name_images = { "airboat", "cessna", "minicooper" , "skyscraper", "trumpet", "bunny" }; //scale factor: 2, 1 , 0.25 , 0.75 , 0.05, 6

int main(int argc, char** argv) {
	int num_searchs = 100000;
	int max_size = 30; // de -30 a 30 en cada dimension
	
	//----------functions to compare
	max_points = 20;
	compare("objects/" + name_images[5] + ".obj", max_size, num_searchs, 6);
	/*
	//code to draw an image
	octree = new Octree<int>(0, 0, 0, max_size);
	kdtree = new Kdtree<int>(max_size);
	vector<point<int>*> image = read_from_image<int>("objects/" + name_images[5] + ".obj", 9);
	max_points = 0.05*image.size();
	octree->points = image;
	kdtree->points = image;
	octree->divide_tree(octree);
	kdtree->divide_tree(kdtree);
	*/

	glutInit(&argc, argv);
	create_window_Octree();
	create_window_kdtree();
		
	glutMainLoop(); //bucle de rendering //no escribir nada abajo de mainloop
	
	system("pause");
	return 0;
} 