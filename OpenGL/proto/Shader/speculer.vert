#version 330 core
layout (location = 0) in vec3 aPos      ; // 頂点位置
layout (location = 1) in vec3 aNormal   ; // 法線
layout (location = 2) in vec2 aTexCoords; // テクスチャ座標

uniform mat4 model      ; // モデル行列
uniform mat4 view       ; // ビュー行列
uniform mat4 projection ; // プロジェクション行列

out     vec3 Normal     ; // フラグメントへの法線出力
out     vec3 FragPos    ; // フラグメントの位置座標出力
out     vec2 TexCoords  ; // テクスチャ座標

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos     = vec3(model * vec4(aPos, 1.0));
    Normal      = mat3(transpose(inverse(model))) * aNormal;
	TexCoords   = aTexCoords;
}