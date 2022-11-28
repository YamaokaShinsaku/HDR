// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// GLSL 3.3 を要求
#version 330

// 頂点シェーダーからの入力
// テクスチャ座標
in vec2 fragTexCoord;
// 法線（ワールド空間）
in vec3 fragNormal;
// 頂点位置（ワールド空間）
in vec3 fragWorldPos;

// 出力カラー（出力ピクセルカラー）
out vec4 outColor;

// テクスチャサンプリング
uniform sampler2D uTexture;

// ディレクショナルライト用構造体
struct DirectionalLight
{
	// ライト方向
	vec3 mDirection;
	// ディフューズ色
	vec3 mDiffuseColor;
	// スペキュラー色
	vec3 mSpecColor;
};

// ライティング用変数
// カメラ位置（ワールド空間）
uniform vec3 uCameraPos;
// ポリゴン表面のスペキュラー強度
uniform float uSpecPower;
// アンビエントライト色
uniform vec3 uAmbientLight;

// ディレクショナルライト
uniform DirectionalLight uDirLight;

void main()
{
	// ポリゴン表面の法線（フラグメントシェーダー上で補間されている）
	vec3 N = normalize(fragNormal);
	// ポリゴン表面からライト方向へのベクトル
	vec3 L = normalize(-uDirLight.mDirection);
	// ポリゴン表面からカメラ方向
	vec3 V = normalize(uCameraPos - fragWorldPos);
	// -L ベクトルを 法線 N に対して反射したベクトルRを求める
	vec3 R = normalize(reflect(-L, N));

	// フォン反射計算
	vec3 Phong = uAmbientLight;
	float NdotL = dot(N, L);

	vec3 Diffuse;
	vec3 Specular;

	Diffuse = uDirLight.mDiffuseColor * max(NdotL,0.0f);
	Specular = uDirLight.mSpecColor * pow(max(0.0, dot(R, V)), uSpecPower);

	// Final color is texture color times phong light (alpha = 1)
	outColor = texture(uTexture, fragTexCoord) * vec4((Diffuse + uAmbientLight),1.0f) + vec4(Specular,1.0f);
}
