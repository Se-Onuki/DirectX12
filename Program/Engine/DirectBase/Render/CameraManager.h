/// @file CameraManager.h
/// @brief カメラマネージャーの実装
/// @author ONUKI seiya
#pragma once
#include "../../Engine/Utils/Containers/Singleton.h"
#include "../../Engine/DirectBase/Render/Camera.h"

namespace SolEngine {

	class CameraManager : public SoLib::Singleton<CameraManager> {
		/// @brief シングルトンクラスのコンストラクタ系の定義
		ConstructSingleton(CameraManager);
	public:

		void Init();

		/// @brief 保存されてるデータの初期化
		void Clear();

		/// @brief カメラの取得
		/// @return メインカメラが指定されていたら返す
		Camera3D *const GetCamera() const;

		/// @brief カメラの取得
		/// @param[in] key 紐づいたキーの検索
		/// @return ヒットしたら有効な値を返す
		Camera3D *const GetCamera(const std::string &key) const;

		Camera3D *const AddCamera(const std::string &key);
		Camera3D *const AddCamera(const std::string &key, std::unique_ptr<Camera3D> &&camera);

		void SetMainCamera(const std::string &key);

		/// @brief 全てのカメラを更新する
		void CalcAll();


	private:


		/// @brief カメラの保存
		std::unordered_map<std::string, std::unique_ptr<Camera3D>> cameras_;

		/// @brief メインのカメラ
		decltype(cameras_)::iterator mainCamera_;

	};

}