#version 330 core             // GLSL 3.30

layout(location = 0) in vec4  a_position; // per-vertex position (per-vertex input)
layout(location = 1) in vec4  a_color;    // per-vertex color (per-vertex input)

out vec4 v_color;                         // per-vertex color (per-vertex output)

void main()
{
	v_color = a_color;
	gl_Position = a_position;
}