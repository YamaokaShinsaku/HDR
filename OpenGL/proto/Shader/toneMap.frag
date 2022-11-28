#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;

uniform float exposure;  //�I�o

void main()
{
	const float gamma = 2.2;
	vec3 hdrColor = texture(hdrBuffer,TexCoords).rgb;

	// �g�[���}�b�s���O
	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

	// �K���}�R���N�V����
	mapped = pow(mapped,vec3(1.0 / gamma));

	FragColor = vec4(mapped,1.0);
}