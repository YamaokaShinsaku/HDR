#pragma once

#include <glad/glad.h>
#include <vector>
#include "Math.h"

class MeshObj
{
public:
	enum VertexFormat
	{
		VertexFormatEnum_PosNormalTex,
		VertexFormatEnum_PosNormalTexTangent
	};

	MeshObj();
	~MeshObj();
	void                  loadMesh(const char* fileName);                         // メッシュのロード
	void                  loadMesh(const char* fileName, Matrix4& transMat);      // メッシュを変換してロード
	void                  draw() const;                                           // 描画
	MeshObj::VertexFormat getFormat() const { return MeshObj::mVFormat; }         // 頂点フォーマットの取得

	bool                  convertTangentMesh();                                   // 法線マップ用にタンジェントベクトル付き頂点フォーマットに変換
	void                  setTexture(GLuint textureID, int textureStageNum);      // テクスチャIDをテクスチャステージにセット
	GLuint                getTextureID(int textureStageNum) const;                // テクスチャステージにセットされているテクスチャIDを返す
	unsigned int          getTextureNum() const { return mTexturesNum; }

private:
	float*                calcInsertPoint(float* dst, int vertexIndex, int stride, int insertPoint);
	void                  insertVec(float* dst, int vertexIndex,int stride, int insertPoint, const Vector3& vec);
	void                  copyPointNormalUVtoTangentArray(float* dst, int vertexIndex, int stride, float* srcPNUVArray);
	void                  calcTangent(Vector3& destTangent, const Vector3& pos1, const Vector3& pos2, const Vector3& pos3, const Vector2& uv1, const Vector2& uv2, const Vector2& uv3);
	void                  getPosVec(Vector3& destPos, const float* fv, int index);
	void                  getUVVec(Vector2& destUV, const float* fv, int index);

	bool         mReady;                // 描画フラグ
	unsigned int mVAO;                  // 頂点配列オブジェクト
	unsigned int mVBO;                  // 頂点バッファオブジェクト
	unsigned int mEBO;                  // エレメントバッファオブジェクト（インデックスバッファ）
	unsigned int mIndexSize;			// インデックスバッファの個数
	unsigned int mVBOSize;				// VBOに格納されている個数
	const int    mTextureNumMax = 8;	// テクスチャステージ最大数
	GLuint       mTextures[8];			// テクスチャステージに登録するテクスチャ
	unsigned int mTexturesNum;          // 登録テクスチャ枚数        
	VertexFormat mVFormat;				// 頂点フォーマット
};


