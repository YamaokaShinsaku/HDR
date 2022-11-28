#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;

uniform float exposure;  //露出

void main()
{
	const float gamma = 2.2;
	vec3 hdrColor = texture(hdrBuffer,TexCoords).rgb;

	// トーンマッピング
	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

	// ガンマコレクション
	mapped = pow(mapped,vec3(1.0 / gamma));

	FragColor = vec4(mapped,1.0);
}