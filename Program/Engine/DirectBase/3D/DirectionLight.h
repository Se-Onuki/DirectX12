/// @file DirectionLight.h
/// @brief 並行光源の実装
/// @author ONUKI seiya
#pragma once
#include "../../Engine/Utils/Graphics/Color.h"
#include "../../Engine/Utils/Math/Euler.h"
#include "../../Engine/Utils/Math/Vector3.h"
#include "../../Engine/Utils/Math/Vector4.h"
#include "../../Engine/Utils/Math/Math.hpp"
#include "../../Engine/DirectBase/Base/CBuffer.h"
#include <stdint.h>
#include <wrl.h>
#include <d3d12.h>
#include <memory>

/// @namespace Light
/// @brief ライティング
namespace Light {
	/// @struct Direction
	/// @brief 並行光源
	struct Direction {
		Vector4 color;		// 色(RGBA)
		Vector3 direction;	// ライトの向き
		float intensity;	// 輝度

		int32_t pattern;	// ライティングの種類
	};
}

/// @class DirectionLight
/// @brief 並行光源
class DirectionLight {
	/// @brief ComPtrのエイリアス
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	DirectionLight() = default;
	~DirectionLight() = default;

	/// @struct Direction
	/// @brief 並行光源
	struct Direction {
		SoLib::Color::RGB4 color;		// 色(RGBA)
		Vector3 direction;	// ライトの向き
		float intensity;	// 輝度
		int32_t pattern;	// ライティングの種類
	};

	/// @enum Pattern
	/// @brief ライティングの種類
	enum class Pattern {
		kNone,
		kLambert,
		kHalfLambert
	};

	/// @fn void Init(void)
	/// @brief 初期化
	void Init();
	/// @fn void SetLight(ID3D12GraphicsCommandList *const)
	/// @brief ライトの設定
	/// @param[in] commandList コマンドリストの借用
	void SetLight(ID3D12GraphicsCommandList *const commandList);

	/// @fn void ImGuiWidget(void)
	/// @brief ImGuiの表示
	void ImGuiWidget();

	/// @fn static std::unique_ptr<DirectionLight> Generate(void)
	/// @brief 並行光源の生成
	/// @return 並行光源
	[[nodiscard]] static std::unique_ptr<DirectionLight> Generate();

	/// @brief 角度の取得
	/// @return 角度
	const SoLib::Math::Euler &GetEuler() const { return euler_; }

	/// @brief 角度の設定
	/// @return 角度
	void SetEuler(const SoLib::Math::Euler &euler) {
		euler_ = euler;
		lightData_->direction = SoLib::EulerToDirection(euler_).Normalize();
	}

private:

	CBuffer<Direction> lightData_;
	SoLib::Math::Euler euler_;

};