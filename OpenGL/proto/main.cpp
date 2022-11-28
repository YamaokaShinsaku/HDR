#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "glad/glad.h"
#include <stdlib.h>
#include "Math.h"
#include "Shader.h"
#include "FlyCamera.h"
#include "MeshObj.h"

SDL_Window* SDLWindow;
SDL_GLContext context;
SDL_Renderer* renderer;

// 関数プロトタイプ宣言
bool initGL();
void destroyGL();
GLuint loadTexture(std::string textureFileName);
void drawMeshsInScene(Shader* shader, MeshObj& mesh);
void setTextureUnit(Shader* shader, std::vector<unsigned int>& textures);
void screenVAOSetting(unsigned int& vao);

void drawModelInScene(Shader* shader, MeshObj& mesh);

int main(int argc, char** argv)
{
	initGL();

	// カメラ設定
	Vector3 camerapos(18, 5, 5);
	FlyCamera flyCamera(camerapos);

	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// depthMapFBOにデプステクスチャをアタッチする
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// テクスチャ読み込み
	unsigned int floorTex, floorTexN, floorTexS, pillerTex, pillerTexN, pillerTexS;
	floorTex = loadTexture("mesh/T_BrickFloor_Clean_A.png");
	floorTexS = loadTexture("mesh/T_BrickFloor_Clean_S.png");
	pillerTex = loadTexture("mesh/T_Edge_Stones_And_Straight_Column_Texturing_Albedo.png");
	pillerTexS = loadTexture("mesh/T_Edge_Stones_And_Straight_Column_Texturing_Specular.png");

	// HDR関連
	unsigned int hdrFBO, rbo, floatColorTexture;
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	{
		// FBOに割り当てるためのからのテクスチャを作成
		glGenTextures(1, &floatColorTexture);
		glBindTexture(GL_TEXTURE_2D, floatColorTexture);

		// RGBA併せて64bit使う
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1024, 768, 0, GL_RGBA, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// FBOにカラーテクスチャとして frameColorTexture をアタッチする
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, floatColorTexture, 0);

		// レンダーバッファの作成
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1024, 768);

		// FBOにレンダーバッファーをアタッチする
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		}

		// 元のスクリーンに戻す
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// スクリーン全体を描く四角形用頂点配列
	float quadVertices[] = {
		// ポジション   // テクスチャ座標
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	// スクリーン全体を描く四角形用 VAO
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// スクリーン全体を覆う頂点バッファオブジェクト
	unsigned int screenVAO;
	screenVAOSetting(screenVAO);

	// メッシュ読み込み
	Matrix4 scale = Matrix4::CreateScale(0.01f);
	MeshObj floorMesh, pillerMesh, sphereMesh;
	floorMesh.loadMesh("mesh/SM_Floor_Internal.obj", scale);
	pillerMesh.loadMesh("mesh/SM_Pillar_Internal.obj", scale);

	// 球体だけずらして表示するために移動行列セット
	Matrix4 mat = Matrix4::CreateTranslation(Vector3(3.0, 3.0, 3.0));
	scale = Matrix4::CreateScale(0.1f);
	mat = scale * mat;
	sphereMesh.loadMesh("mesh/sphere.obj", mat);

	// メッシュにテクスチャ登録
	floorMesh.setTexture(floorTex, 0);
	floorMesh.setTexture(floorTexS, 1);
	floorMesh.setTexture(depthMap, 2);
	pillerMesh.setTexture(pillerTex, 0);
	pillerMesh.setTexture(pillerTexS, 1);
	pillerMesh.setTexture(depthMap, 2);

	// テクスチャ配列に登録
	std::vector<unsigned int>floorTextures;
	std::vector<unsigned int>pillerTextures;

	floorTextures.emplace_back(floorTex);
	floorTextures.emplace_back(floorTexS);
	floorTextures.emplace_back(depthMap);
	pillerTextures.emplace_back(pillerTex);
	pillerTextures.emplace_back(pillerTexS);
	pillerTextures.emplace_back(depthMap);

	// シェーダー
	Shader phongShader("shader/speculer.vert", "shader/speculer.frag");
	Shader depthMapShader("shader/depthmap.vert", "shader/depthmap.frag");
	Shader debugShader("shader/Debugdepthmap.vert", "shader/Debugdepthmap.frag");
	Shader shadowMapShader("shader/shadowmap.vert", "shader/shadowmap.frag");
	Shader HDRShader("shader/speculer.vert", "shader/HDR.frag");
	Shader sphereShader("shader/Sphere.vert", "shader/Sphere.frag");
	Shader toneMapShader("shader/screen.vert", "shader/tonemap.frag");
	Shader screenShader("shader/screen.vert", "shader/screen.frag");

	phongShader.setTextureUniformString("diffuseMap", 0);
	phongShader.setTextureUniformString("SpecluarMap", 1);

	// シェーダパラメータ
	Vector3 LightDir(0.5f, 0.5f, -0.5f);
	Vector3 ambient(0.4f, 0.4f, 0.4f);
	Vector3 specular(0.8f, 0.8f, 0.8f);
	Vector3 diffuse(1.0f, 1.0f, 1.0f);
	LightDir.Normalize();

	bool renderLoop = true;
	Uint32 lastTime = 0;
	float  deltaTime = 0;
	MOUSE_INSTANCE.SetRelativeMouseMode(true);
	float anim = 0.0f;

	float exposure = 1.0f;

	while (renderLoop)
	{
		INPUT_INSTANCE.Update();
		MOUSE_INSTANCE.Update(); // レンダーループの初めに1回だけ呼ぶ
		deltaTime = (SDL_GetTicks() - lastTime) / 1000.0f;
		lastTime = SDL_GetTicks();

		// 終了イベントのキャッチ
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_MOUSEWHEEL:
				MOUSE_INSTANCE.OnMouseWheelEvent(event);
				break;

			case SDL_QUIT:
				renderLoop = false;
				break;
			}
		}

		flyCamera.UpdateCamera(deltaTime);
		// フライカメラ
		Matrix4 viewMat, projMat;
		viewMat = flyCamera.GetViewMatrix();
		projMat = flyCamera.GetProjectionMatrix();
		Vector3 viewPos;
		viewPos = flyCamera.GetPositionVec();

		// ディレクショナルライト
		anim += 0.3f * deltaTime;
		LightDir = Vector3(0.5f * sinf(anim), -0.3f, 0.5f * cosf(anim));
		LightDir.Normalize();
		Vector3 forwardVec = flyCamera.GetFrontVec();
		Vector3 camPosVec = flyCamera.GetPositionVec();
		Vector3 LightPos;
		Vector3 mapCenterPos(4 * 6, 0, -4 * 6);
		LightPos = LightDir * -30.0f + mapCenterPos;
		Matrix4 lightProjection, lightView, lightSpaceMatrix;

		lightProjection = Matrix4::CreateOrtho(80.0f, 80.0f, 1.0f, 100.0f);
		lightView = Matrix4::CreateLookAt(LightPos, mapCenterPos, Vector3::UnitY);
		lightSpaceMatrix = lightView * lightProjection;

		if (INPUT_INSTANCE.IsKeyPressed(SDL_SCANCODE_UP))
		{
			exposure += 0.01f;
		}
		if (INPUT_INSTANCE.IsKeyPressed(SDL_SCANCODE_DOWN))
		{
			exposure -= 0.01f;
			if (exposure < 0.0f)
			{
				exposure = 0.0f;
			}
		}

		// シャドウマップパス
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		depthMapShader.use();
		depthMapShader.setMatrix("lightSpaceMatrix", lightSpaceMatrix.GetAsFloatPtr());
		drawMeshsInScene(&depthMapShader, floorMesh);
		drawMeshsInScene(&depthMapShader, pillerMesh);

		// 描画パス
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		{
			glEnable(GL_DEPTH_TEST);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glViewport(0, 0, 1024, 768);

			// シェーダーにuniform変数をセット
			phongShader.use();
			phongShader.setVec3("light.direction", LightDir);
			phongShader.setVec3("light.ambient", ambient);
			phongShader.setVec3("light.diffuse", diffuse);
			phongShader.setVec3("light.specular", specular);
			phongShader.setVec3("viewPos", viewPos);
			phongShader.setMatrix("view", viewMat.GetAsFloatPtr());
			phongShader.setMatrix("projection", projMat.GetAsFloatPtr());

			// メッシュにテクスチャを設定して描画
			setTextureUnit(&phongShader, floorTextures); // 床
			drawMeshsInScene(&phongShader, floorMesh);

			setTextureUnit(&phongShader, pillerTextures); // 柱
			drawMeshsInScene(&phongShader, pillerMesh);

			Vector3 lightColor(0.8, 0.5, 0.2);
			sphereShader.use();
			sphereShader.setMatrix("view", viewMat.GetAsFloatPtr());
			sphereShader.setMatrix("projection", projMat.GetAsFloatPtr());

			sphereShader.setVec3("color", lightColor);
			sphereShader.setFloat("luminance", 5.0);

			drawMeshsInScene(&sphereShader, sphereMesh);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		{
			glDisable(GL_DEPTH_TEST);
			glClear(GL_COLOR_BUFFER_BIT);
			glViewport(0, 0, 1024, 768);

			// スクリーンいっぱいの四角形を描画
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, floatColorTexture);

			toneMapShader.use();
			toneMapShader.setInt("screenTexture", 0);
			toneMapShader.setFloat("exposure", exposure);
			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		SDL_GL_SwapWindow(SDLWindow);

		// Escキーで終了
		if (INPUT_INSTANCE.IsKeyPullup(SDL_SCANCODE_ESCAPE))
		{
			exit(0);
		}
	}

	// テクスチャの削除
	glDeleteTextures(1, &floorTex);
	glDeleteTextures(1, &floorTexS);
	glDeleteTextures(1, &pillerTex);
	glDeleteTextures(1, &pillerTexS);

	destroyGL();

	return 0;
}

/// <summary>
/// SDLとGLの初期化
/// </summary>
/// <returns></returns>
bool initGL()
{
	// SDLの初期化
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		printf("SDL初期化失敗 : %s\n", SDL_GetError());
		return false;
	}

	// OpenGL アトリビュートのセット
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// GL version 3.1
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	// 8Bit RGBA チャンネル
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// ダブルバッファリング
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// ハードウェアアクセラレーションを強制
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	// Windowの作成
	SDLWindow = SDL_CreateWindow("SDL & GL Window",
		100, 80,
		1024, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!SDLWindow)
	{
		printf("Windowの作成に失敗: %s", SDL_GetError());
		return false;
	}

	// SDL Rendererの作成
	renderer = SDL_CreateRenderer(SDLWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer)
	{
		printf("SDLRendererの作成に失敗 %s", SDL_GetError());
		return false;
	}

	context = SDL_GL_CreateContext(SDLWindow);

	// GLADの初期化
	const int version = gladLoadGL();
	if (version == 0)
	{
		printf("glad初期化失敗！\n");
		return false;
	}

	return true;
}

/// <summary>
/// SDLとGLの後始末
/// </summary>
void destroyGL()
{
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(SDLWindow);
}


/// <summary>
/// テクスチャーの読み込み
/// </summary>
/// <param name="textureFileName"> テクスチャーのファイル名 </param>
/// <returns> textureID </returns>
GLuint loadTexture(std::string textureFileName)
{
	// 画像をSDLサーフェスに読み込む
	SDL_Surface* surf = IMG_Load(textureFileName.c_str());
	if (!surf)
	{
		std::cout << "ファイル読み込みに失敗 : " << textureFileName << std::endl;
		return -1;
	}

	// 画像の幅・高さを取得
	int width = surf->w;
	int	height = surf->h;
	int channels = surf->format->BytesPerPixel;

	// 画像フォーマットの判別
	int format = GL_RGB;
	if (channels == 4)
	{
		format = GL_RGBA;
	}

	// SDLサーフェスからＧＬテクスチャ作成＆各種パラメータセット
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// テクスチャラッピング＆フィルタリング設定
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// SDLデータをGLのテクスチャデータとしてコピーする
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
		GL_UNSIGNED_BYTE, surf->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	// SDLサーフェスは用済みなので解放
	SDL_FreeSurface(surf);

	return textureID;
}

/// <summary>
/// メッシュを8x8に並べてシーン描画する
/// </summary>
/// <param name="shader"> シェーダー </param>
/// <param name="mesh"> メッシュ </param>
void drawMeshsInScene(Shader* shader, MeshObj& mesh)
{
	// メッシュの描画（これ以前にshaderにmodel行列以前のデータは渡している前提)
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			Vector3 pos(i * 6.0f, 0.0f, -j * 6.0f);
			Matrix4 modelMat = Matrix4::CreateTranslation(pos);
			shader->setMatrix("model", modelMat.GetAsFloatPtr());
			mesh.draw();
		}
	}
}

/// <summary>
/// 追加モデルの描画
/// </summary>
/// <param name="shader"></param>
/// <param name="mesh"></param>
void drawModelInScene(Shader* shader, MeshObj& mesh)
{
	Vector3 pos(16.0f, 0.0f, -3.0f);
	Matrix4 modelMat = Matrix4::CreateTranslation(pos);
	shader->setMatrix("model", modelMat.GetAsFloatPtr());
	mesh.draw();
}

/// <summary>
/// テクスチャステージとテクスチャの設定
/// </summary>
/// <param name="textures"> テクスチャー </param>
void setTextureUnit(Shader* shader, std::vector<unsigned int>& textures)
{
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		shader->setInt(shader->getShaderStageUniformName(i), i);
	}
}

/// <summary>
/// 画面全体を覆う頂点定義
/// </summary>
/// <param name="vao"> 頂点配列オブジェクト(Vertex Array Object) </param>
void screenVAOSetting(unsigned int& vao)
{
	unsigned int vbo;	// 頂点バッファオブジェクト(Vertex Buffer Object)
	float quadVertices[] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	// setup plane VAO
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}