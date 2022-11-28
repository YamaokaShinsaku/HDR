#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;

uniform float exposure;
uniform float luminance;

void main()
{
	const float gamma = 2.2;
	vec3 hdrColor = texture(hdrBuffer,TexCoords).rgb;

	// トーンマッピング
	vec3 mapped = hdrColor / (-hdrColor * exposure);

	// ガンマコレクション
	mapped = pow(mapped,vec3(1.0 / gamma));

	FragColor = vec4(hdrColor,1.0);
}