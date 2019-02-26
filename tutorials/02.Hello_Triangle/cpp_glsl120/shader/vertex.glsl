#version 120                  // GLSL 1.20

attribute vec4 a_position;    // per-vertex position (per-vertex input)
attribute vec4 a_color;       // per-vertex color (per-vertex input)
varying vec4 v_color;         // per-vertex color (per-vertex output)

void main()
{
	v_color = a_color;
	gl_Position = a_position;
}