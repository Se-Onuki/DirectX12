#pragma once
#include <vector>
#include "../Render/Render.hpp"

#include <string>
#include <fstream>
#include <sstream>

#include <list>
#include <unordered_map>

#include "../Math/Vector4.h"
#include "../Math/Vector3.h"
#include "../Math/Vector2.h"
#include "../Math/Matrix4x4.h"
#include <wrl.h>
#include <d3d12.h>


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
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:

	//D3D12_VERTEX_BUFFER_VIEW vbView_ = {};
	//D3D12_INDEX_BUFFER_VIEW ibView_ = {};

	//ComPtr<ID3D12Resource> vertBuff_;
	//ComPtr<ID3D12Resource> indexBuff_;

	std::vector<Render::VertexData> vertices_;
	std::vector<uint32_t> indexs_;
	Material material_;

	void CreateBuffer();
};

struct Transform;

class Model
{
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	Model();
	~Model();

	std::string name_;
	std::vector<Mesh *> meshList_;
	std::unordered_map<std::string, Material *> materialMap_;

	void LoadObjFile(const std::string &directoryPath, const std::string &fileName);

	void Draw(ID3D12GraphicsCommandList *const commandList, const Transform &transform, const Matrix4x4 &viewProjection) const;
private:

};
