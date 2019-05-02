#define GLUT_DISABLE_ATEXIT_HACK
#include <windows.h>
#include <iostream>
#include <math.h>
#include<GL/glut.h>

#include "Octree.cpp"
#include "point.cpp"

#define KEY_ESC 27

using namespace std;

int mx = 0;
int my = 0;

float ax = 0.0;
float ay = 0.0;
float speed = 0.1;

Octree <int>* octree;
int esferaX = 0, esferaY = 0, esferaZ = 0, esferaR = 10;
float esferaspeed = 2;

bool r = false, drawSphere= false;

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

	if (drawSphere)	//pinta puntos dentro del radio(color verde)
		octree->drawSphere(esferaX, esferaY, esferaZ, esferaR);

	octree->draw(octree);	//pinta puntos y linea divisoras del cuadrante(color verde)
	
	
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

void window_key_glut(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		if (esferaY <= 30) esferaY += esferaspeed;
		break;

	case GLUT_KEY_DOWN:
		if (esferaY >= -30) esferaY -= esferaspeed;
		break;

	case GLUT_KEY_RIGHT:
		if (esferaX <= 30) esferaX += esferaspeed;
		break;

	case GLUT_KEY_LEFT:
		if (esferaX >= -30) esferaX -= esferaspeed;
		break;
	case GLUT_KEY_PAGE_UP:
		if (esferaZ <= 30) esferaZ += esferaspeed;
		break;
	case GLUT_KEY_PAGE_DOWN:
		if (esferaZ >= -30) esferaZ -= esferaspeed;
		break;
	case GLUT_KEY_HOME:
		drawSphere = !drawSphere;
		break;
	default:
		break;
	}
	//glutPostRedisplay();
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
	glutSpecialFunc(&window_key_glut);
	glutMouseFunc(&OnMouseClick);
	glutMotionFunc(&OnMouseMotion);
	glutIdleFunc(&idle);
}

//
//el programa principal
//
int main(int argc, char** argv) {
	int num_points = 10;
	octree = new Octree<int>(0,0,0,30);
	octree->generate_points(num_points);

	glutInit(&argc, argv);
	create_window_Octree();

	glutMainLoop(); //bucle de rendering
					//no escribir nada abajo de mainloop
	return 0;
} 