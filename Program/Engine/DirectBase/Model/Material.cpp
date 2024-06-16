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
		std::unique_ptr<Material> materialResult = std::make_unique<Material>();
		// シーン内のマテリアル
		aiMaterial *material = materialArray[source.index_];
		// ディフューズのテクスチャが存在するか
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {

			// 文字列の保存先
			aiString textureFilePath;
			// マテリアルからテクスチャ名を取得
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);

			// テクスチャの読み込み
			materialResult->texHandle_ = ::TextureManager::Load(directoryPath + textureFilePath.C_Str());

			materialResult->blendMode_ = Model::BlendMode::kNone;

		}

		materialResult->materialData_ = Material::MaterialData{
			.color = Vector4{1.f, 1.f, 1.f, 1.f},
			.emissive = {},
			.uvTransform = Matrix4x4::Identity(),
			.shininess = 1.f,
		};

		ai_real alfa;
		material->Get(AI_MATKEY_OPACITY, alfa);
		aiVector3D color;
		material->Get(AI_MATKEY_COLOR_AMBIENT, color);
		if (color.x == 0.f and color.y == 0.f and color.z == 0.f) { color = { 1.f,1.f,1.f }; }
		materialResult->materialData_->color = { color.x, color.y, color.z , alfa };

		ai_real shininess;
		material->Get(AI_MATKEY_SHININESS, shininess);
		materialResult->materialData_->shininess = shininess;

		//ai_real shininessStrength;
		//material->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength);
		//materialResult->materialData_->shininessStrength = shininessStrength;

		aiVector3D emissive;
		material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
		materialResult->materialData_->emissive = { emissive.x, emissive.y, emissive.z };

		// マテリアル名の設定
		// materialResult->name_ = material->GetName().C_Str();

		// 数値の文字列をキーとしてマテリアルのデータを追加
		/*result->materialMap_.insert({ std::to_string(materialIndex), std::move(materialResult) });*/


		return std::move(materialResult);
	}
}

template<>
bool SoLib::ImGuiWidget([[maybe_unused]] const char *const label, [[maybe_unused]] SolEngine::Material *const value)
{
#ifdef USE_IMGUI
	bool result = false;
	if (ImGui::TreeNode(label)) {
		static BaseTransform transform;
		transform.MatToSRT(value->materialData_->uvTransform);

		if (transform.ImGuiWidget2D()) {
			value->materialData_->uvTransform = transform.Affine();
		}
		if (ImGui::Button("ResetTransform")) {
			value->materialData_->uvTransform = Matrix4x4::Identity();
		}

		ImGui::ColorEdit4("BaseColor", &value->materialData_->color.r);
		ImGui::ColorEdit3("EmissiveColor", &value->materialData_->emissive.r);

		const static std::array<std::string, 6u> blendStr{ "kNone", "kNormal", "kAdd", "kSubtract", "kMultily", "kScreen" };

		if (ImGui::BeginCombo("BlendMode", blendStr[static_cast<uint32_t>(value->blendMode_)].c_str())) {

			for (uint32_t i = 0; i < blendStr.size(); i++) {
				if (ImGui::Selectable(blendStr[i].c_str())) {
					value->blendMode_ = static_cast<Model::BlendMode>(i);
					break;
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::TreeNode("Texture")) {
			value->texHandle_ = TextureManager::GetInstance()->ImGuiTextureSelecter(value->texHandle_);
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