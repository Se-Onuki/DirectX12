#pragma once
#include <vector>
#include "../Render/Render.hpp"

#include <string>
#include <fstream>
#include <sstream>

#include <list>

#include "../Math/Vector4.h"
#include "../Math/Vector3.h"
#include "../Math/Vector2.h"

struct Material {
	std::string textureFilePath;

	static Material LoadFile(const std::string &directoryPath, const std::string &fileName) {
		Material materialData;
		std::string line;

		std::ifstream file{ directoryPath + "/" + fileName };
		if (!file.is_open()) return materialData;		// 開けなかった場合、処理を終了する

#pragma region ファイルからMaterialDataを構築

		while (std::getline(file, line)) {

			std::string identifier;
			std::istringstream s{ line };
			s >> identifier;

			if (identifier == "map_Kd") {
				// 連結してファイルバスにする
				std::string textureFilename;
				s >> textureFilename;

				materialData.textureFilePath = directoryPath + "/" + textureFilename;
			}
		}
#pragma endregion

		return materialData;
	}
};

struct Mesh
{
	std::vector<Render::VertexData> vertices;
	//std::vector<uint32_t> indexs;
	Material material;
	//std::vector<uint32_t> indexs;

	static Mesh LoadObjFile(const std::string &directoryPath, const std::string &fileName) {

#pragma region 1. 中で必要になる変数の宣言

		Mesh modelData;				// 構築するModelData
		std::vector<Vector4> positionList;	// 位置
		std::vector<Vector3> normalList;	// 法線
		std::vector<Vector2> texCoordList;	// テクスチャ座標

		std::string line;					// ファイルから一行を格納する

#pragma endregion

#pragma region 2. ファイルを開く

		std::ifstream file{ directoryPath + "/" + fileName };
		if (!file.is_open()) return modelData;		// 開けなかった場合、処理を終了する

#pragma endregion

#pragma region 3. ファイルを読み, ModelDataを構築

		while (std::getline(file, line)) {
			std::string identifier;
			std::istringstream s{ line };
			s >> identifier;

			if (identifier == "v") {
				Vector4 position;
				s >> position.x >> position.y >> position.z;
				position.z *= -1;
				position.w = 1.f;
				positionList.push_back(position);

			}
			else if (identifier == "vt") {
				Vector2 texCoord;
				s >> texCoord.x >> texCoord.y;
				texCoord.y = 1.f - texCoord.y;
				texCoordList.push_back(texCoord);
			}
			else if (identifier == "vn") {
				Vector3 normal;
				s >> normal.x >> normal.y >> normal.z;
				normal.z *= -1;
				normalList.push_back(normal);
			}
			else if (identifier == "f") {
				// 面は三角形限定。その他は未対応
				Render::VertexData triangle[3] = {};
				for (uint32_t faceVertex = 0u; faceVertex < 3u; ++faceVertex) {
					std::string vertexDefinition;
					s >> vertexDefinition;
					// 頂点の要素へのIndexは「 位置 / UV / 法線 」 で格納されているので、分解してIndex を取得する
					std::istringstream v{ vertexDefinition };
					uint32_t elementIndices[3];
					for (uint32_t element = 0; element < 3; ++element) {
						std::string index;
						std::getline(v, index, '/'); // 区切りでインデックスを読んでいく
						elementIndices[element] = std::stoi(index);
					}
					// 要素へのIndexから、実際の要素の値を取得して、頂点を機築する
					Vector4 position = positionList[elementIndices[0] - 1];
					Vector2 texCoord = texCoordList[elementIndices[1] - 1];
					Vector3 normal = normalList[elementIndices[2] - 1];
					// 末尾から順に(法線の逆転)
					triangle[2u - faceVertex] = { position,texCoord,normal };

				}
				// イテレータを用いた末尾への直接構築
				modelData.vertices.insert(modelData.vertices.end(), triangle, triangle + 3);
				/*modelData.indexs.push_back(modelData.vertices.size() - 3);
				modelData.indexs.push_back(modelData.vertices.size() - 2);
				modelData.indexs.push_back(modelData.vertices.size() - 1);*/
			}
			else if (identifier == "mtllib") {

				std::string materialFile;
				s >> materialFile;
				modelData.material = Material::LoadFile(directoryPath, materialFile);
			}
		}
#pragma endregion
		return modelData;

	}
};

class Model
{
public:
	Model();
	~Model();

	Mesh modelData_;

private:

};
