#version 330 core             // GLSL 3.30

in vec4 v_color;              // per-fragment color (per-fragment input)

out vec4 FragColor;           

void main()
{
	FragColor = v_color;
}