#version 330 core

uniform vec3 color;
uniform float luminance;

out vec4 FragColor;

void main()
{
	vec3 result =color * luminance;
   FragColor = vec4(result,1.0);
}