// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// GLSL 3.3 を要求
#version 330

uniform vec3 uColor;
out vec4 outColor;

void main()
{
	// スクリーン座標位置から計算して破線にする 
    //uint x = uint(gl_FragCoord.x);
	//uint y = uint(gl_FragCoord.y);
	//if( mod(x, 2) == 0 || mod(y, 2) == 0)
	//{
	//   discard; //描画破棄
	//}
	outColor = vec4(uColor.r, uColor.g, uColor.b, 1.0);
}
