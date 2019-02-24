/// main.cpp

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

int main(void)
{
	GLFWwindow* window;

	// Initialize GLFW library
	if (!glfwInit())
		return -1;

	// Create a GLFW window containing a OpenGL context
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Make the current OpenGL context as one in the window
	glfwMakeContextCurrent(window);

	// Initialize GLEW library
	if (glewInit() != GLEW_OK)
		std::cout << "GLEW Init Error!" << std::endl;

	// Print out the OpenGL version supported by the graphics card in my PC
	std::cout << glGetString(GL_VERSION) << std::endl;

	glClearColor(0.5, 0.5, 0.5, 1.0);

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		// Render here
		glClear(GL_COLOR_BUFFER_BIT);

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}