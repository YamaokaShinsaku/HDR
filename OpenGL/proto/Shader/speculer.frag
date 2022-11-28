#version 330 core

// ライト
struct Light
{
        vec3  direction       ; // ライトの方向(ディレクショナルライト)
        vec3  ambient         ; // ライトアンビエント
        vec3  diffuse         ; // ライトディフューズ
        vec3  specular        ; // ライトスペキュラー
};
uniform Light     light       ;

uniform vec3      viewPos     ; // 視点
uniform sampler2D diffuseMap  ; // ディフューズテクスチャ
uniform sampler2D specularMap ; // スペキュラーテクスチャ

in      vec3  FragPos         ; // フラグメント位置のワールド座標
in      vec3  Normal          ; // フラグメント位置の法線ベクトル
in      vec2  TexCoords       ; // テクスチャ座標

out     vec4  FragColor       ; // このフラグメントの出力

void main()
{
    // アンビエント
    vec3 ambient     = light.ambient * vec3(texture(diffuseMap, TexCoords));

    // ディフューズ
    vec3  norm       = normalize(Normal);
    float diff       = max(dot(norm,-light.direction), 0.0);
    vec3  diffuse    = light.diffuse * diff * vec3(texture(diffuseMap, TexCoords));

    // スペキュラー
    vec3  viewDir    = normalize(viewPos - FragPos);
    vec3  reflectDir = reflect(light.direction, norm);
    float spec       = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3  specular   = light.specular * spec * vec3(texture(specularMap,TexCoords));

    vec3 result      = ambient + diffuse + specular;
    FragColor        = vec4(result, 1.0);
}