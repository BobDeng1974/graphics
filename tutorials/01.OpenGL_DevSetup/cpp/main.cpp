///// main.cpp
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

void init();
void mydisplay();

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitWindowSize(500, 500);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Hello OpenGL");

	init();

	glutDisplayFunc(mydisplay);

	glutMainLoop();

	return	0;
}

void init()
{
	if (glewInit() != GLEW_OK)
		std::cout << "GLEW Init Error!" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

void mydisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glutSwapBuffers();
}