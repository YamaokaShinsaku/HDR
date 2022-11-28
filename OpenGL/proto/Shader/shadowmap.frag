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
uniform sampler2D depthMap    ; // （シャドウ）デプスマップ

in      vec3  FragPos         ; // フラグメント位置のワールド座標
in      vec3  Normal          ; // フラグメント位置の法線ベクトル
in      vec2  TexCoords       ; // テクスチャ座標
in      vec4  FragPosLightSpace;// ライト空間でのフラグメント座標
out     vec4  FragColor       ; // このフラグメントの出力

float ShadowCaluculation(vec4 fragPosLightSpace)
{
   // パースペクティブ除算
   vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
   projCoords = projCoords * 0.5 + 0.5;

   // シャドウマップよりライトに最も近いフラグメントの深度値を得る
   float closestDepth = texture(depthMap, projCoords.xy).r;
   // 現在描画しようとしているフラグメントの深度値
   float currentDepth = projCoords.z;
   // シャドウ判定 (1.0:シャドウ 0.0:シャドウの外)
   float bias = max(0.0005 * (1.0 - dot(normalize(Normal), light.direction)), 0.00005);;
   float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
   return shadow;
}

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

    float shadow     = ShadowCaluculation(FragPosLightSpace);
    vec3 result      = ambient + (1.0 - shadow) * (diffuse + specular);
    FragColor        = vec4(result, 1.0);
}