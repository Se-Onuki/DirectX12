/// @file Material.cpp
/// @brief マテリアルクラス
/// @author ONUKI seiya
#include "Material.h"
#include "../Base/TextureManager.h"

namespace SolEngine {
	std::unique_ptr<Material> ResourceCreater<Material>::CreateObject(const ResourceSource<Material> &source) const {


		const aiScene *const scene = source.assimpHandle_->importer_->GetScene();
		const std::span<aiMaterial *> materialArray = { scene->mMaterials, scene->mNumMaterials };

		const auto *assimpSource = ResourceObjectManager<AssimpData>::GetInstance()->GetSource(source.assimpHandle_);

		assert(assimpSource and "Assimpのデータが存在しません");

		const std::string &directoryPath = assimpSource->directoryPath_;

		// 保存先のマテリアル
		std::unique_ptr<Material> result = std::make_unique<Material>();
		// シーン内のマテリアル
		const aiMaterial *material = materialArray[source.index_];

		// 読み出されたテクスチャへのハンドル
		std::vector<uint32_t> textureHandles;
		textureHandles.reserve(scene->mNumTextures);

		// 内包されたテクスチャを持っているか
		if (scene->HasTextures()) {
			// 内包されたテクスチャの配列
			const std::span<aiTexture *> textures = { scene->mTextures, scene->mNumTextures };

			// テクスチャを1つづつ処理する
			for (const auto *const texture : textures) {
				// テクスチャ名
				const auto &texName = texture->mFilename;

				// テクスチャのデータ
				const std::span<std::byte> texData = { reinterpret_cast<std::byte *>(&(texture->pcData->b)), texture->mHeight ? texture->mHeight * texture->mWidth * 4u : texture->mWidth * 4u };

				// テクスチャのデータを読み込んで保存する
				textureHandles.push_back(::TextureManager::Load(texName.C_Str(), texData));

			}
		}

		// ディフューズのテクスチャが存在するか
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			// 文字列の保存先
			aiString textureFilePath;
			// マテリアルからテクスチャ名を取得
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);

			// 内包されたテクスチャを読んでいない場合
			if (textureFilePath.data[0] != '*') {
				// テクスチャの読み込み
				result->texHandle_ = ::TextureManager::Load(directoryPath + textureFilePath.C_Str());
			}
			// テクスチャが内包されていた場合
			else {
				// テクスチャの読み込み
				result->texHandle_ = textureHandles.at(
					std::stoul(textureFilePath.C_Str() + 1)	// '*'の次の文字からを数字として扱う
				);
			}

		}

		result->blendMode_ = Model::BlendMode::kNone;

		result->materialData_ = Material::MaterialData{
			.color = Vector4{1.f, 1.f, 1.f, 1.f},
			.emissive = {},
			.ambient = {},
			.uvTransform = Matrix4x4::Identity(),
			.shininess = 1.f,
			.shininessStrength = 0.f,
		};

		if (aiColor3D ambient; material->Get(AI_MATKEY_COLOR_AMBIENT, ambient) == AI_SUCCESS) {
			result->materialData_->ambient = { ambient.r, ambient.g, ambient.b , 1.f };
		}

		ai_real alfa;
		bool isAlfa = material->Get(AI_MATKEY_OPACITY, alfa) == AI_SUCCESS;

		if (aiColor3D color; material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
			result->materialData_->color = { color.r, color.g, color.b , isAlfa ? alfa : 1.f };
		}

		if (ai_real shininess; material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
			result->materialData_->shininess = shininess;
		}

		ai_real shininessStrength;
		if (material->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength) == AI_SUCCESS) {
			result->materialData_->shininessStrength = shininessStrength;
		}

		aiColor3D emissive;
		if (material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive) == AI_SUCCESS) {
			result->materialData_->emissive = { emissive.r, emissive.g, emissive.b };
		}

		// マテリアル名の設定
		// materialResult->name_ = material->GetName().C_Str();

		// 数値の文字列をキーとしてマテリアルのデータを追加
		/*result->materialMap_.insert({ std::to_string(materialIndex), std::move(materialResult) });*/


		return std::move(result);
	}
}

template<>
bool SoLib::ImGuiWidget([[maybe_unused]] const char *const label, [[maybe_unused]] SolEngine::Material *const value)
{
#ifdef USE_IMGUI
	bool result = false;
	if (ImGui::TreeNode(label)) {
		//static BaseTransform transform;

		Vector2 scale{ value->materialData_->uvTransform.GetRight().Length(),  value->materialData_->uvTransform.GetUp().Length() };
		float rotate = std::acos(value->materialData_->uvTransform.m[0][0] / scale.x);
		Vector2 translate = value->materialData_->uvTransform.GetTranslate().ToVec2();

		//transform.MatToSRT(value->materialData_->uvTransform);

		result |= ImGui::DragFloat2("Scale", &scale.x, 0.01f, 0.001f, 2048.f);
		result |= ImGui::DragFloat("Rotate", &rotate, Angle::Dig2Rad);
		result |= ImGui::DragFloat2("Transform", &translate.x, 0.01f, -100.f, 100.f);

		if (result) {
			value->materialData_->uvTransform = SoLib::Affine(Vector3{ scale.x, scale.y }, Vector3{ .z = rotate }, Vector3{ translate.x, translate.y });
		}
		if (ImGui::Button("ResetTransform")) {
			result = true;
			value->materialData_->uvTransform = Matrix4x4::Identity();
		}

		result |= ImGui::ColorEdit4("BaseColor", &value->materialData_->color.r);
		result |= ImGui::ColorEdit3("EmissiveColor", &value->materialData_->emissive.r);

		const static std::array<std::string, 6u> blendStr{ "kNone", "kNormal", "kAdd", "kSubtract", "kMultily", "kScreen" };

		if (ImGui::BeginCombo("BlendMode", blendStr[static_cast<uint32_t>(value->blendMode_)].c_str())) {

			for (uint32_t i = 0; i < blendStr.size(); i++) {
				if (ImGui::Selectable(blendStr[i].c_str())) {
					value->blendMode_ = static_cast<Model::BlendMode>(i);
					result = true;
					break;
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::TreeNode("Texture")) {
			value->texHandle_ = TextureManager::GetInstance()->ImGuiTextureSelecter(value->texHandle_);
			result = true;
			ImGui::TreePop();
		}
		result |= ImGui::SliderFloat("Shininess", &value->materialData_->shininess, 0.f, 1000.f);
		result |= ImGui::SliderFloat("ShininessStrength", &value->materialData_->shininessStrength, 0.f, 1.f);

		ImGui::TreePop();
	}
	return result;
#else
	return false;
#endif // USE_IMGUI
}