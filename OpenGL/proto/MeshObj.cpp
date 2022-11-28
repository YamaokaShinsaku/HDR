#include <iostream>
#include "MeshObj.h"
#include "tiny_obj_loader.h"


MeshObj::MeshObj()
	: mReady(false)
	, mVAO(0)
	, mVBO(0)
	, mEBO(0)
	, mIndexSize(0)
	, mVBOSize(0)
	, mVFormat(VertexFormatEnum_PosNormalTex)
	, mTexturesNum(0)
{
	for (int i = 0; i < 8; i++)
	{
		mTextures[i] = 0;
	}
}

MeshObj::~MeshObj()
{
	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mEBO);
}

void MeshObj::loadMesh(const char* fileName)
{
	Matrix4 identity;
	loadMesh(fileName, identity);
}

void MeshObj::loadMesh(const char* fileName, Matrix4& transMat)
{
	// ���f���̃��[�h
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fileName))
	{
		std::cout << "�t�@�C���ǂݍ��ݎ��s : " << fileName << std::endl;
		std::cout << err << std::endl;
		return;
	}

	const int attribVertexNum = 3; // ���_���W��     x,y,z     : 3��   
	const int attribNormalNum = 3; // �@��vector��  nx, ny, nz : 3��
	const int attribTexCoordNum = 2; // texture���W�� u, v       : 2��
	const int attribStride = attribVertexNum + attribNormalNum + attribTexCoordNum;

	int vertexNum = attrib.vertices.size() / attribVertexNum; //���_��

	// �C���f�b�N�X�o�b�t�@�̎擾
	std::vector<int> indices;
	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			indices.push_back(index.vertex_index);
		}
	}

	// ���_�f�[�^���m�� ���_���~1�̒��_�f�[�^�ɕK�v�ȗv�f��(x,y,z,nx,ny,nz,u,v)�@1���_��8��float
	std::vector<float> vertexVec(vertexNum * attribStride);

	// �`�󃋁[�v
	for (const auto& shape : shapes)
	{
		size_t indexOffset = 0;
		// ���̖ʁi�O�p�`or�l�p�`�j�̃��[�v
		for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
		{
			size_t num_vertices = shape.mesh.num_face_vertices[f];
			// ���_�����[�v
			for (size_t v = 0; v < num_vertices; v++)
			{
				// �ʂ��\������C���f�b�N�X���擾
				tinyobj::index_t idx = shape.mesh.indices[indexOffset + v];

				Vector3 position = Vector3(attrib.vertices[3 * idx.vertex_index + 0],
					attrib.vertices[3 * idx.vertex_index + 1],
					attrib.vertices[3 * idx.vertex_index + 2]);
				position = Vector3::Transform(position, transMat);

				// ���_���W��vertexVec�ɃR�s�[
				vertexVec[idx.vertex_index * attribStride + 0] = position.x;
				vertexVec[idx.vertex_index * attribStride + 1] = position.y;
				vertexVec[idx.vertex_index * attribStride + 2] = position.z;

				// �@���f�[�^��vertexVec�ɃR�s�[
				vertexVec[idx.vertex_index * attribStride + 3] = attrib.normals[3 * idx.normal_index + 0];
				vertexVec[idx.vertex_index * attribStride + 4] = attrib.normals[3 * idx.normal_index + 1];
				vertexVec[idx.vertex_index * attribStride + 5] = attrib.normals[3 * idx.normal_index + 2];

				// uv�f�[�^��vertexVec
				vertexVec[idx.vertex_index * attribStride + 6] = attrib.texcoords[2 * idx.texcoord_index + 0];
				vertexVec[idx.vertex_index * attribStride + 7] = 1.0f - attrib.texcoords[2 * idx.texcoord_index + 1];
			}
			indexOffset += num_vertices;
		}
	}

	// GPU�ɓ]��
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glGenBuffers(1, &mEBO);

	glBindVertexArray(mVAO);
	{
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexNum * attribStride, vertexVec.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, attribStride * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, attribStride * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, attribStride * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	mReady = true;
	mVBOSize = vertexVec.size();
	mIndexSize = indices.size();
	mVFormat = VertexFormatEnum_PosNormalTex;
}

void MeshObj::draw() const
{
	if (!mReady)
	{
		return;
	}

	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, mIndexSize, GL_UNSIGNED_INT, 0);

}

bool MeshObj::convertTangentMesh()
{

	if (mVFormat != VertexFormatEnum_PosNormalTex)
	{
		return false;
	}
	const int tangentStride = 11;
	const int oldStride = 8;
	const int insertPoint = 8;
	const int vertexNum = mVBOSize / oldStride;

	float* vboptr;
	float* vertexsrc = new float[mVBOSize];
	int* eboptr;
	int* indexBuffer = new int[mIndexSize];

	// GPU���VBO�̃f�[�^��vertexSrc�ɃR�s�[����
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	vboptr = static_cast<float*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY));
	if (!vboptr)
	{
		return false;
	}
	memcpy(vertexsrc, vboptr, mVBOSize * sizeof(float));
	glUnmapBuffer(GL_ARRAY_BUFFER);

	// GPU���EBO�̃f�[�^��indexBuffer�ɃR�s�[����
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	eboptr = static_cast<int*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY));
	if (!eboptr)
	{
		return false;
	}
	memcpy(indexBuffer, eboptr, mIndexSize * sizeof(int));
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	// �V�������_�t�H�[�}�b�g�̃������m��
	float* newTangentArray = new float[vertexNum * tangentStride];

	// �V�������_�t�H�[�}�b�g�ɃR�s�[����
	for (int i = 0; i < vertexNum; i++)
	{
		copyPointNormalUVtoTangentArray(newTangentArray, i, tangentStride, vertexsrc);
	}

	// �O�p�`�ʂ�N��UV�l���A�^���W�F���g���v�Z
		// 1���ڂ̎O�p�`
	Vector3 pos[3], tangent;
	Vector2 uv[3];
	for (unsigned int i = 0; i < mIndexSize / 3; i++)
	{	
		getPosVec(pos[0], vertexsrc, indexBuffer[i * 3 + 0]);
		getPosVec(pos[1], vertexsrc, indexBuffer[i * 3 + 1]);
		getPosVec(pos[2], vertexsrc, indexBuffer[i * 3 + 2]);

		getUVVec(uv[0], vertexsrc, indexBuffer[i * 3 + 0]);
		getUVVec(uv[1], vertexsrc, indexBuffer[i * 3 + 1]);
		getUVVec(uv[2], vertexsrc, indexBuffer[i * 3 + 2]);

		calcTangent(tangent, pos[0], pos[1], pos[2], uv[0], uv[1], uv[2]);
		insertVec(newTangentArray, indexBuffer[i * 3 + 0], tangentStride, insertPoint, tangent);
		insertVec(newTangentArray, indexBuffer[i * 3 + 1], tangentStride, insertPoint, tangent);
		insertVec(newTangentArray, indexBuffer[i * 3 + 2], tangentStride, insertPoint, tangent);
	}

	//debug
	//for (int i = 0; i < vertexNum; i++)
	//{
	//	for (int j = 0; j < tangentStride; j++)
	//	{
	//		printf("% .3f ", newTangentArray[i * tangentStride + j]);
	//	}
	//	printf("\n");
	//}

	// ����VAO���폜
	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mEBO);
	mVAO = 0;
	mVBO = 0;
	mEBO = 0;

	// �V�K��VAO�쐬
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glGenBuffers(1, &mEBO);

	glBindVertexArray(mVAO);
	{
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexNum * tangentStride, newTangentArray, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * mIndexSize, indexBuffer, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, tangentStride * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, tangentStride * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, tangentStride * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, tangentStride * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);
	}

	// GPU�ɓ]�������̂Ō��f�[�^�폜
	delete[] newTangentArray;
	delete[] indexBuffer;
	delete[] vertexsrc;

	mVFormat = VertexFormatEnum_PosNormalTexTangent;

	return true;
}

void MeshObj::setTexture(GLuint textureID, int textureStageNum)
{
	if (textureStageNum < mTextureNumMax && textureStageNum >= 0)
	{
		mTextures[textureStageNum] = textureID;
		mTexturesNum++;
	}
}

GLuint MeshObj::getTextureID(int textureStageNum) const
{
	if (textureStageNum < mTextureNumMax && textureStageNum >= 0)
	{
		return mTextures[textureStageNum];
	}
	// �w�肳�ꂽ�e�N�X�`���X�e�[�W�͖���
	return 0;
}

float* MeshObj::calcInsertPoint(float* dst, int vertexIndex, int stride, int insertPoint)
{
	// ���_�o�b�t�@�z���stride�ɍ��킹���K�؂ȑ}����|�C���^�̌v�Z
	return dst + vertexIndex * stride + insertPoint;
}

// ���_�z��̓K�؂Ȉʒu��vec���R�s�[
void MeshObj::insertVec(float* dst, int vertexIndex, int stride, int insertPoint, const Vector3& vec)
{
	float* ins = calcInsertPoint(dst, vertexIndex, stride, insertPoint);
	ins[0] = vec.x;
	ins[1] = vec.y;
	ins[2] = vec.z;
}

// ���_�t�H�[�}�b�g��PosNormalUV ���� �^���W�F���g�t���ɕϊ�����
void MeshObj::copyPointNormalUVtoTangentArray(float* dst, int vertexIndex, int stride, float* srcPNUVArray)
{
	const int PNUVStride = 8; // PointNormalUV�z��̃X�g���C�h
	float* ins = calcInsertPoint(dst, vertexIndex, stride, 0);              // �}����
	float* src = calcInsertPoint(srcPNUVArray, vertexIndex, PNUVStride, 0); // �R�s�[��

	// Tangent�t���z��ɃR�s�[
	for (int i = 0; i < PNUVStride; i++)
	{
		ins[i] = src[i];
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// �^���W�F���g�x�N�g�����v�Z
// �O�p�`�ʂ�pos1�`pos3 ,uv0�`uv2 ���g����Tangent�x�N�g���� destTangent�ɕԂ�
//
// inout destTangent Tangent���i�[����ϐ�
// in : pos0, pos1, pos2 �O�p�`�ʂ̒��_
// in : uv0, uv1, uv2    �O�p�`�ʂ̒��_�ɑΉ�����UV���W
///////////////////////////////////////////////////////////////////////////////////////
void MeshObj::calcTangent(Vector3& destTangent, const Vector3& pos1, const Vector3& pos2, const Vector3& pos3, const Vector2& uv1, const Vector2& uv2, const Vector2& uv3)
{
	Vector3 edge1, edge2;
	edge1 = pos2 - pos1;
	edge2 = pos3 - pos1;

	Vector2 deltaUV1 = uv2 - uv1;
	Vector2 deltaUV2 = uv3 - uv1;

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	destTangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	destTangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	destTangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

	destTangent.Normalize();

}
/////////////////////////////////////////////////
// ���_���璸�_���W�l�����o��
// inout : destUV  ���_��UV���W���i�[����ϐ�
// in    : fv      ���_&UV�f�[�^�z��̐擪�A�h���X���i�[���ꂽ�|�C���^
//         index   ���_&UV�f�[�^�z��̃C���f�b�N�X�l
////////////////////////////////////////////////
void MeshObj::getPosVec(Vector3& destPos, const float* fv, int index)
{
	destPos.x = fv[index * 8 + 0];
	destPos.y = fv[index * 8 + 1];
	destPos.z = fv[index * 8 + 2];
}
/////////////////////////////////////////////////
// ���_����UV�l�����o��
// inout : destUV  ���_��UV���W���i�[����ϐ�
// in    : fv      ���_&UV�f�[�^�z��̐擪�A�h���X���i�[���ꂽ�|�C���^
//         index   ���_&UV�f�[�^�z��̃C���f�b�N�X�l
////////////////////////////////////////////////
void MeshObj::getUVVec(Vector2& destUV, const float* fv, int index)
{
	destUV.x = fv[index * 8 + 6 + 0];
	destUV.y = fv[index * 8 + 6 + 1];
}

