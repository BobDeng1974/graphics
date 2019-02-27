///// main.cpp
///// GLSL 3.30

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <cassert>

////////////////////////////////////////////////////////////////////////////////
/// 쉐이더 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLuint  program;          // 쉐이더 프로그램 객체의 레퍼런스 값
GLint   loc_a_position;   // attribute 변수 a_position 위치
GLint   loc_a_color;      // attribute 변수 a_color 위치

GLuint  position_buffer;  // GPU 메모리에서 position_buffer의 위치
GLuint  color_buffer;     // GPU 메모리에서 color_buffer의 위치

GLuint create_shader_from_file(const std::string& filename, GLuint shader_type);
void init_shader_program();
void init_buffer_objects();
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/// 렌더링 관련 변수 및 함수
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

void render_scene();    // rendering 함수: 현재 scene은 삼각형 하나로 구성되어 있음.
////////////////////////////////////////////////////////////////////////////////


                        // GLSL 파일을 읽어서 컴파일한 후 쉐이더 객체를 생성하는 함수
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

// vertex shader와 fragment shader를 링크시켜 program을 생성하는 함수
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

  loc_a_position = glGetAttribLocation(program, "a_position");
  loc_a_color = glGetAttribLocation(program, "a_color");
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


// scene rendering: 현재 scene은 삼각형 하나로 구성되어 있음.
void render_scene()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // 특정 쉐이더 프로그램 사용
  glUseProgram(program);

  // 서버측 attribute 변수 a_position, a_color 값을 채춰줄 클라이언트 측 데이터 지정

  // 버텍스 쉐이더 layout(location = 0) 위치의 attribute 활성화
  glEnableVertexAttribArray(loc_a_position);
  // 앞으로 언급하는 배열 버퍼(GL_ARRAY_BUFFER) 는 position_buffer로 지정
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
  // 현재 배열 버퍼에 있는 데이터를 버텍스 쉐이더 layout(location = 0) 위치의 attribute와 연결
  glVertexAttribPointer(loc_a_position, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // 버텍스 쉐이더 layout(location = 1) 위치의 attribute 활성화
  glEnableVertexAttribArray(loc_a_color);
  // 앞으로 언급하는 배열 버퍼(GL_ARRAY_BUFFER) 는 color_buffer로 지정
  glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
  // 현재 배열 버퍼에 있는 데이터를 버텍스 쉐이더 layout(location = 1) 위치의 attribute와 연결
  glVertexAttribPointer(loc_a_color, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // 삼각형 그리기
  glDrawArrays(GL_TRIANGLES, 0, 3);

  // 정점 attribute 배열 비활성화
  glDisableVertexAttribArray(loc_a_position);
  glDisableVertexAttribArray(loc_a_color);
  // 쉐이더 프로그램 사용해제
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