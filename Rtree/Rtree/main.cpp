#include <iostream>
#include "Rtree.cpp"

using namespace std;

Rtree* rtree;

//funcion llamada a cada imagen
void glPaint(void) {
	//El fondo de la escena al color initial
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //CAMBIO
	glLoadIdentity();
	glOrtho(-0.1, 1.1, -0.1, 1.1, -1, 1);

	rtree->draw();

	//doble buffer, mantener esta instruccion al fin de la funcion
	glutSwapBuffers();
}

void init_GL(void) {
	//Color del fondo de la escena
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //(R, G, B, transparencia) en este caso un fondo negro
}

//en el caso que la ventana cambie de tamaño
GLvoid window_redraw(GLsizei width, GLsizei height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

void idle() { // AGREGAR ESTA FUNCION
	glutPostRedisplay();
}

void create_window() {
	//Inicializacion de la GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600); //tamaño de la ventana
	glutInitWindowPosition(100, 100); //posicion de la ventana
	glutCreateWindow("R-Tree"); //titulo de la ventana

	init_GL(); //funcion de inicializacion de OpenGL

	glutDisplayFunc(glPaint);
	glutReshapeFunc(&window_redraw);
	// Callback del teclado
	glutIdleFunc(&idle);
}


vector<point*> generate_points(int num_points, int num_dimension) {	//generar puntos normalizados entre 0 y 1
	vector<point*> result(num_points, nullptr);
	vector<float> pt(num_dimension, 0);

	for (int i = 0; i < num_points; ++i) {
		for (int j = 0; j < num_dimension; ++j)
			pt[j] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

		result[i] = new point(pt);
	}
	return result;
}

void show_points(vector<point*> points) {	//imprimir puntos normalizados entre 0 y 1
	for (int i = 0; i < points.size(); ++i) {
		for (int j = 0; j < (points[0]->dim).size(); ++j)
			cout << (points[i]->dim)[j] << ", ";
		cout << endl;
	}
}

int main(int argc, char** argv) {
	int num_points = 100000;
	int room = 50;
	int num_dimension = 2;
	

	vector<point*> points = generate_points(num_points, num_dimension);
	//show_points(points);

	rtree = new Rtree(room);
	
	for (int i = 0; i < points.size() ; ++i) {
		rtree->insert(points[i]);
	}
	cout << rtree->root->containers.size() << endl;;
	cout << "num_points: " << rtree->root->numPoints() << endl;

	glutInit(&argc, argv);
	create_window();
	glutMainLoop(); //bucle de rendering //no escribir nada abajo de mainloop
	
	system("pause");
	return 0;
}