///// main.cpp
///// GLSL 3.30

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <cassert>

////////////////////////////////////////////////////////////////////////////////
/// ���̴� ���� ���� �� �Լ�
////////////////////////////////////////////////////////////////////////////////
GLuint  program;         // ���̴� ���α׷� ��ü�� ���۷��� ��
//GLint loc_a_position;   // attribute ���� a_position ��ġ
//GLint loc_a_color;      // attribute ���� a_color ��ġ

GLuint  position_buffer;  // GPU �޸𸮿��� position_buffer�� ��ġ
GLuint  color_buffer;     // GPU �޸𸮿��� color_buffer�� ��ġ

GLuint create_shader_from_file(const std::string& filename, GLuint shader_type);
void init_shader_program();
void init_buffer_objects();
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/// ������ ���� ���� �� �Լ�
////////////////////////////////////////////////////////////////////////////////
  // per-vertex 3D homogeneous positions (x, y, z, w=1)
  GLfloat position[] = {
     0.5f,  0.5f,  0.0f,  1.0f,   // 0th vertex position
    -0.5f, -0.5f,  0.0f,  1.0f,   // 1st vertex position
     0.5f, -0.5f,  0.0f,  1.0f,   // 2nd vertex position
  };

  // per-vertex RGBA color (r, g, b, a)
  GLfloat color[] = {     
    1.0f, 0.0f, 0.0f, 1.0f,       // 0th vertex color (red)
    1.0f, 0.0f, 0.0f, 1.0f,       // 1st vertex color (red)
    1.0f, 0.0f, 0.0f, 1.0f,       // 2nd vertex color (red)
  };

void render_scene();    // rendering �Լ�: ���� scene�� �ﰢ�� �ϳ��� �����Ǿ� ����.
////////////////////////////////////////////////////////////////////////////////


// GLSL ������ �о �������� �� ���̴� ��ü�� �����ϴ� �Լ�
GLuint create_shader_from_file(const std::string& filename, GLuint shader_type)
{
  GLuint shader = 0;

  shader = glCreateShader(shader_type);

  std::ifstream shader_file(filename.c_str());
  std::string shader_string;

  shader_string.assign(
    (std::istreambuf_iterator<char>(shader_file)),
    std::istreambuf_iterator<char>());

  const GLchar * shader_src = shader_string.c_str();
  glShaderSource(shader, 1, (const GLchar **)&shader_src, NULL);
  glCompileShader(shader);

  return shader;
}

// vertex shader�� fragment shader�� ��ũ���� program�� �����ϴ� �Լ�
void init_shader_program()
{
  GLuint vertex_shader
    = create_shader_from_file("./shader/vertex.glsl", GL_VERTEX_SHADER);

  std::cout << "vertex_shader id: " << vertex_shader << std::endl;
  assert(vertex_shader != 0);

  GLuint fragment_shader
    = create_shader_from_file("./shader/fragment.glsl", GL_FRAGMENT_SHADER);

  std::cout << "fragment_shader id: " << fragment_shader << std::endl;
  assert(fragment_shader != 0);

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  std::cout << "program id: " << program << std::endl;
  assert(program != 0);

  //loc_a_position = glGetAttribLocation(program, "a_position");
  //loc_a_color = glGetAttribLocation(program, "a_color");
}

void init_buffer_objects()
{
  glGenBuffers(1, &position_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);

  glGenBuffers(1, &color_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
}


// scene rendering: ���� scene�� �ﰢ�� �ϳ��� �����Ǿ� ����.
void render_scene()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Ư�� ���̴� ���α׷� ���
  glUseProgram(program);

  // ������ attribute ���� a_position, a_color ���� ä���� Ŭ���̾�Ʈ �� ������ ����
  
  // ���ؽ� ���̴� layout(location = 0) ��ġ�� attribute Ȱ��ȭ
  glEnableVertexAttribArray(0);     
  // ������ ����ϴ� �迭 ����(GL_ARRAY_BUFFER) �� position_buffer�� ����
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
  // ���� �迭 ���ۿ� �ִ� �����͸� ���ؽ� ���̴� layout(location = 0) ��ġ�� attribute�� ����
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // ���ؽ� ���̴� layout(location = 1) ��ġ�� attribute Ȱ��ȭ
  glEnableVertexAttribArray(1);
  // ������ ����ϴ� �迭 ����(GL_ARRAY_BUFFER) �� color_buffer�� ����
  glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
  // ���� �迭 ���ۿ� �ִ� �����͸� ���ؽ� ���̴� layout(location = 1) ��ġ�� attribute�� ����
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
  
  // �ﰢ�� �׸���
  glDrawArrays(GL_TRIANGLES, 0, 3);

  // ���� attribute �迭 ��Ȱ��ȭ
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  // ���̴� ���α׷� �������
  glUseProgram(0);
}


int main(void)
{
  GLFWwindow* window;

  // Initialize GLFW library
  if (!glfwInit())
    return -1;

  // Create a GLFW window containing a OpenGL context
  window = glfwCreateWindow(500, 500, "Hello Triangle", NULL, NULL);
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

  init_shader_program();
  init_buffer_objects();

  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

  // Loop until the user closes the window
  while (!glfwWindowShouldClose(window))
  {
    // Poll for and process events
    glfwPollEvents();

    render_scene();

    // Swap front and back buffers
    glfwSwapBuffers(window);

  }

  glfwTerminate();

  return 0;
}