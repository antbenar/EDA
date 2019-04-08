#define GLUT_DISABLE_ATEXIT_HACK
#include <windows.h>
#include <iostream>
#include <math.h>
#include<GL/glut.h>
#include <time.h>       

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
	glutInitWindowPosition(720, 100); //posicion de la ventana
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
	for (int i = 0; i < num_points; ++i) {
		vector<double> pt = { (double)(rand() % max_size_point * 2 - max_size_point), (double)(rand() % max_size_point * 2 - max_size_point), (double)(rand() % max_size_point * 2 - max_size_point) };
		result.push_back(new point<T>(pt));
	}
	return result;
}

void octree_against_kdtree(int max_size, int number_points) {
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

int main(int argc, char** argv) {
	int number_points = 5;
	int max_size = 30; // de -30 a 30 en cada dimension
	
	octree = new Octree<int>(0,0,0, max_size);
	kdtree = new Kdtree<int>(max_size);
	
	/*
	for (long long int i = 10000; i <= 100000; i = i + 10000) {
		octree_against_kdtree(max_size, i);
	}
	*/
	

	glutInit(&argc, argv);
	
	//octree->generate_points(number_points);
	//create_window_Octree();

	kdtree->generate_points(max_size, number_points);
	create_window_kdtree();

	glutMainLoop(); //bucle de rendering //no escribir nada abajo de mainloop
	
	system("pause");
	return 0;
} 