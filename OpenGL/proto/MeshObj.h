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
	void                  loadMesh(const char* fileName);                         // ���b�V���̃��[�h
	void                  loadMesh(const char* fileName, Matrix4& transMat);      // ���b�V����ϊ����ă��[�h
	void                  draw() const;                                           // �`��
	MeshObj::VertexFormat getFormat() const { return MeshObj::mVFormat; }         // ���_�t�H�[�}�b�g�̎擾

	bool                  convertTangentMesh();                                   // �@���}�b�v�p�Ƀ^���W�F���g�x�N�g���t�����_�t�H�[�}�b�g�ɕϊ�
	void                  setTexture(GLuint textureID, int textureStageNum);      // �e�N�X�`��ID���e�N�X�`���X�e�[�W�ɃZ�b�g
	GLuint                getTextureID(int textureStageNum) const;                // �e�N�X�`���X�e�[�W�ɃZ�b�g����Ă���e�N�X�`��ID��Ԃ�
	unsigned int          getTextureNum() const { return mTexturesNum; }

private:
	float*                calcInsertPoint(float* dst, int vertexIndex, int stride, int insertPoint);
	void                  insertVec(float* dst, int vertexIndex,int stride, int insertPoint, const Vector3& vec);
	void                  copyPointNormalUVtoTangentArray(float* dst, int vertexIndex, int stride, float* srcPNUVArray);
	void                  calcTangent(Vector3& destTangent, const Vector3& pos1, const Vector3& pos2, const Vector3& pos3, const Vector2& uv1, const Vector2& uv2, const Vector2& uv3);
	void                  getPosVec(Vector3& destPos, const float* fv, int index);
	void                  getUVVec(Vector2& destUV, const float* fv, int index);

	bool         mReady;                // �`��t���O
	unsigned int mVAO;                  // ���_�z��I�u�W�F�N�g
	unsigned int mVBO;                  // ���_�o�b�t�@�I�u�W�F�N�g
	unsigned int mEBO;                  // �G�������g�o�b�t�@�I�u�W�F�N�g�i�C���f�b�N�X�o�b�t�@�j
	unsigned int mIndexSize;			// �C���f�b�N�X�o�b�t�@�̌�
	unsigned int mVBOSize;				// VBO�Ɋi�[����Ă����
	const int    mTextureNumMax = 8;	// �e�N�X�`���X�e�[�W�ő吔
	GLuint       mTextures[8];			// �e�N�X�`���X�e�[�W�ɓo�^����e�N�X�`��
	unsigned int mTexturesNum;          // �o�^�e�N�X�`������        
	VertexFormat mVFormat;				// ���_�t�H�[�}�b�g
};


