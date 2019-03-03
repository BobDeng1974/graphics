///// main.cpp
///// GLSL 3.30
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <cassert>
#include "vec.hpp"
#include "transform.hpp"
#include "Camera.h"

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
// per-vertex 3D homogeneous positions (x, y, z)
GLfloat g_position[] = {
  0.5f,  0.5f,  0.0f,     // 0th vertex g_position
  -0.5f, -0.5f,  0.0f,    // 1st vertex g_position
  0.5f, -0.5f,  0.0f,     // 2nd vertex g_position
};

// per-vertex RGB color (r, g, b)
GLfloat g_color[] = {
  1.0f, 0.0f, 0.0f,       // 0th vertex color (red)
  1.0f, 0.0f, 0.0f,       // 1st vertex color (red)
  1.0f, 0.0f, 0.0f,       // 2nd vertex color (red)
};

void render_scene();    // rendering 함수: 현재 scene은 삼각형 하나로 구성되어 있음.

void update_buffer_objects();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
kmuvcl::math::mat4x4f     mat_model, mat_view, mat_proj;
kmuvcl::math::mat4x4f     mat_PVM;
GLint                     loc_u_PVM;      // uniform 변수 a_color 위치

float   x_pos = 0.0, y_pos = 0.0, z_pos = 0.0;
bool    b_animation = false;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
Camera  camera;
float   aspect = 1.0f;

static float clear_color[3] = { 0.5f, 0.5f, 0.5f };
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

  loc_u_PVM = glGetUniformLocation(program, "u_PVM");

  loc_a_position = glGetAttribLocation(program, "a_position");
  loc_a_color = glGetAttribLocation(program, "a_color");
}

void init_buffer_objects()
{
  glGenBuffers(1, &position_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_position), g_position, GL_STATIC_DRAW);

  glGenBuffers(1, &color_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_color), g_color, GL_STATIC_DRAW);
}

void set_transform()
{
  // set object transformation
  mat_model = kmuvcl::math::translate<float>(x_pos, y_pos, z_pos);
  
  // set camera transformation
  kmuvcl::math::vec3f eye = camera.position();
  kmuvcl::math::vec3f up = camera.up_direction();
  kmuvcl::math::vec3f center = eye + camera.front_direction();

  std::cout << "eye:    " << eye << std::endl;
  std::cout << "up:     " << up << std::endl;
  std::cout << "center: " << center << std::endl;

  //mat_proj = kmuvcl::math::translate<float>(0, 0, 0);
  //mat_view = kmuvcl::math::translate<float>(0, 0, 0);
  mat_view = kmuvcl::math::lookAt(eye[0], eye[1], eye[2], 
                                  center[0], center[1], center[2], 
                                  up[0], up[1], up[2]);

  if (camera.mode() == Camera::kOrtho)
  {
    std::cout << "(camera.mode() == Camera::kOrtho)" << std::endl;
  }
  else if (camera.mode() == Camera::kPerspective)
  {
    std::cout << "(camera.mode() == Camera::kPerspective)" << std::endl;

    mat_proj = kmuvcl::math::perspective(camera.fovy(), aspect, 0.001f, 1000.0f);
  }
    



  // TODO ...
}


void init_imgui(GLFWwindow* window)
{
  const char* glsl_version = "#version 120";

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
}

void compose_imgui_frame()
{
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
  {
    static float f = 0.0f;
    static int counter = 0;

    static bool show_demo_window;
    static bool show_another_window;
    

    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    //ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
    //ImGui::Checkbox("Another Window", &show_another_window);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    //ImGui::ColorEdit3("clear g_color", (float*)&clear_color); // Edit 3 floats representing a g_color
    ImGui::ColorEdit3("clear g_color", clear_color); // Edit 3 floats representing a g_color

    ImGui::SliderFloat3("1st vertex pos", &g_position[0], -3.0f, 3.0f);
    ImGui::SliderFloat3("2nd vertex pos", &g_position[3], -3.0f, 3.0f);
    ImGui::SliderFloat3("3rd vertex pos", &g_position[6], -3.0f, 3.0f);
    
    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
      counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
  }

  {
    ImGui::Begin("Another window");

    ImGui::End();
  }


  //// Rendering
  //ImGui::Render();
  //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void glfw_error_callback(int error, const char* description)
{
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  //if (key == GLFW_KEY_A && action == GLFW_PRESS)
  //  camera.move_left(0.1f);
  //if (key == GLFW_KEY_D && action == GLFW_PRESS)
  //  camera.move_right(0.1f);
  //if (key == GLFW_KEY_W && action == GLFW_PRESS)
  //  camera.move_forward(0.1f);
  //if (key == GLFW_KEY_S && action == GLFW_PRESS)
  //  camera.move_backward(0.1f);
  if (key == GLFW_KEY_A)
    camera.move_left(0.1f);
  if (key == GLFW_KEY_D)
    camera.move_right(0.1f);
  if (key == GLFW_KEY_W)
    camera.move_forward(0.1f);
  if (key == GLFW_KEY_S)
    camera.move_backward(0.1f);


  if (key == GLFW_KEY_H && action == GLFW_PRESS)
    x_pos -= 0.1f;
  if (key == GLFW_KEY_L && action == GLFW_PRESS)
    x_pos += 0.1f;
  if (key == GLFW_KEY_J && action == GLFW_PRESS)
    y_pos -= 0.1f;
  if (key == GLFW_KEY_K && action == GLFW_PRESS)
    y_pos += 0.1f;

  if (key == GLFW_KEY_P && action == GLFW_PRESS)
  {
    b_animation = !b_animation;
    std::cout << (b_animation ? "animation" : "no animation") << std::endl;
  }
}

void frambuffer_size_callback(GLFWwindow* window, int width, int height)
{
  aspect = (float)width / (float)height;
}

// scene rendering: 현재 scene은 삼각형 하나로 구성되어 있음.
void render_scene()
{
  glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  update_buffer_objects();

  // 특정 쉐이더 프로그램 사용
  glUseProgram(program);

  // 서버측 attribute 변수 a_position, a_color 값을 채춰줄 클라이언트 측 데이터 지정
  mat_PVM = mat_proj * mat_view * mat_model;
  glUniformMatrix4fv(loc_u_PVM, 1, GL_FALSE, mat_PVM);

  // 버텍스 쉐이더 layout(location = 0) 위치의 attribute 활성화
  glEnableVertexAttribArray(loc_a_position);
  // 앞으로 언급하는 배열 버퍼(GL_ARRAY_BUFFER) 는 position_buffer로 지정
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
  // 현재 배열 버퍼에 있는 데이터를 버텍스 쉐이더 layout(location = 0) 위치의 attribute와 연결
  glVertexAttribPointer(loc_a_position, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // 버텍스 쉐이더 layout(location = 1) 위치의 attribute 활성화
  glEnableVertexAttribArray(loc_a_color);
  // 앞으로 언급하는 배열 버퍼(GL_ARRAY_BUFFER) 는 color_buffer로 지정
  glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
  // 현재 배열 버퍼에 있는 데이터를 버텍스 쉐이더 layout(location = 1) 위치의 attribute와 연결
  glVertexAttribPointer(loc_a_color, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // 삼각형 그리기
  glDrawArrays(GL_TRIANGLES, 0, 3);

  // 정점 attribute 배열 비활성화
  glDisableVertexAttribArray(loc_a_position);
  glDisableVertexAttribArray(loc_a_color);
  // 쉐이더 프로그램 사용해제
  glUseProgram(0);
}


void update_buffer_objects()
{
  // bind then map the VBO
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
  
  float* ptr = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  // if the pointer is valid(mapped), update VBO
  if (ptr)
  {
    // modify buffer data
    memcpy(ptr, g_position, 3 * 3 * sizeof(GLfloat));
    glUnmapBuffer(GL_ARRAY_BUFFER);    // unmap it after use
  }

  // unbind VBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}


int main(void)
{
  GLFWwindow* window;

  // Initialize GLFW library
  glfwSetErrorCallback(glfw_error_callback);
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
  
  init_imgui(window);
  init_shader_program();
  init_buffer_objects();

  camera.set_mode(Camera::kPerspective);

  //glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
  
  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, frambuffer_size_callback);

  // Loop until the user closes the window
  while (!glfwWindowShouldClose(window))
  {
    // Poll for and process events
    glfwPollEvents();

    compose_imgui_frame();

    double currentTime = glfwGetTime();
       

    //glfwMakeContextCurrent(window);

    set_transform();
    render_scene();

    

    double lastTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    if (b_animation)
    {
      //x_pos += 0.1f *deltaTime;
      x_pos += 0.1f;
      if (x_pos > 1.0f)
        x_pos = -1.0f;
    }    

    //glfwMakeContextCurrent(window);
    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());    

    // Swap front and back buffers
    //glfwMakeContextCurrent(window);    
    glfwSwapBuffers(window);

    

  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();

  return 0;
}