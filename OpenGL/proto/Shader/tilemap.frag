// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
#version 330

// 頂点シェーダーからのテクスチャ座標を入力として受け取る
in vec2 fragTexCoord;

// 出力カラー
out vec4 outColor;

// uniform
uniform sampler2D uTexture;                     // テクスチャ
uniform int       uTileIndex;                   // 描画したいタイルナンバー
uniform vec2      uTileSetSplitNum;             // テクスチャを縦横分割する数
uniform float     uAlpha;                       // 透明度
									            
void main()
{
	ivec2 texSize  = textureSize(uTexture, 0);  // テクスチャサイズ
	ivec2 splitNum = ivec2(uTileSetSplitNum);   // テクスチャ分割数

	int xpos = uTileIndex % splitNum.x;
	int ypos = uTileIndex / splitNum.x;

	// 基準uvを算出
	vec2 uv = vec2(xpos, ypos) / splitNum;

	// オフセット算出
	vec2 offset = vec2(1.0f, 1.0f) / splitNum;
	offset = offset * fragTexCoord;            
	
	// 最終テクスチャ座標算出
	uv += offset;

	// テクスチャからカラーをサンプリング
	vec4 color = texture(uTexture, uv);
	color.a = color.a * uAlpha;
    outColor = color;
}
