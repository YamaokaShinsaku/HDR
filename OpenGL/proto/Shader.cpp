#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1. filePathから頂点/フラグメントのソースコードを取得します
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // ifstreamオブジェクトが例外をスローできることを確認します。
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // ファイルを開く
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // ファイルのバッファの内容をストリームに読み込みます
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // ファイルハンドルを閉じる
        vShaderFile.close();
        fShaderFile.close();
        // ストリームからstringに変換する
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::ファイル読み込み失敗" << std::endl;
    }

    // ストリングからC文字列に変換
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. シェーダーのコンパイル
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // 頂点シェーダー
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    // エラーが出た場合はエラー表示する
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR" << vertexPath << "::SHADER::頂点シェーダーコンパイル失敗\n" << infoLog << std::endl;
    };

    // フラグメントシェーダーも同様に行います
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    // エラーが出た場合はエラーを表示する
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR" << fragmentPath << "::SHADER::フラグメントシェーダー失敗\n" << infoLog << std::endl;
    }

    // シェーダープログラム
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    // リンクエラーが出たらエラーを表示
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::シェーダーリンクエラー\n" << infoLog << std::endl;
    }

    // シェーダーは現在プログラムＩＤにリンクされ、不要になったためシェーダーを削除します
    glDeleteShader(vertex);
    glDeleteShader(fragment);

}

void Shader::use()
{
    if (ID != -1)
    {
        glUseProgram(ID);
    }
}

void Shader::setBool(const std::string& valiableName, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, valiableName.c_str()), (int)value);
}

void Shader::setInt(const std::string& valiableName, int value) const
{
    unsigned int id = glGetUniformLocation(ID, valiableName.c_str());
    glUniform1i(id, value);
}

void Shader::setFloat(const std::string& valiableName, float value) const
{
    unsigned int id = glGetUniformLocation(ID, valiableName.c_str());
    glUniform1f(id, value);
}

void Shader::setVec3(const std::string& valiableName, float x, float y, float z)
{
    unsigned int id = glGetUniformLocation(ID, valiableName.c_str());
    glUniform3f(id, x, y, z);
}

void Shader::setVec3(const std::string& valiableName, Vector3& vec)
{
    unsigned int id = glGetUniformLocation(ID, valiableName.c_str());
    glUniform3f(id, vec.x, vec.y, vec.z);
}

void Shader::setVec4(const std::string& valiableName, float x, float y, float z, float w)
{
    unsigned int id = glGetUniformLocation(ID, valiableName.c_str());
    glUniform4f(id, x, y, z, w);
}

void Shader::setVec4(const std::string& valiableName, Vector3& vec, float w)
{
    unsigned int id = glGetUniformLocation(ID, valiableName.c_str());
    glUniform4f(id, vec.x, vec.y, vec.z, w);
}

void Shader::setMatrix(const std::string& valiableName, const float* matrix)
{
    unsigned int id = glGetUniformLocation(ID, valiableName.c_str());
    glUniformMatrix4fv(id, 1, GL_FALSE, matrix);
}

void Shader::setTextureUniformString(std::string textureUniformName, unsigned int textureUnitStageNum)
{
    if (textureUnitStageNum > mMaxTextureUnitNum)
    {
        return;
    }
    mTextureUniformStrings[textureUnitStageNum] = textureUniformName;
}

std::string Shader::getShaderStageUniformName(unsigned int textureUnitStageNum)
{
    if (textureUnitStageNum > mMaxTextureUnitNum)
    {
        return std::string();
    }
    return mTextureUniformStrings[textureUnitStageNum];
}
