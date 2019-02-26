#version 120                  // GLSL 1.20

varying vec4 v_color;         // per-fragment color (per-fragment input)

void main()
{
	gl_FragColor = v_color;
}