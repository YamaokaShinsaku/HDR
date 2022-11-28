#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model      ; // モデル行列
uniform mat4 view       ; // ビュー行列
uniform mat4 projection ; // プロジェクション行列

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}