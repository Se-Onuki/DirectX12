#pragma once
#include "../../Engine/ResourceObject/ResourceObject.h"
#include "../../Engine/ResourceObject/ResourceObjectManager.h"
#include "../../Engine/Utils/SoLib/SoLib_Timer.h"
#include "../AssimpData.h"
#include "../../Engine/Utils/Math/Vector3.h"
#include "../../Engine/Utils/Math/Quaternion.h"

namespace SolEngine {

	namespace SkeletonAnimation {

		struct IKeyFlame {};

		template <SoLib::IsRealType T>
		struct KeyFlameTemplate;

		template<>
		struct KeyFlameTemplate<Vector3> : IKeyFlame {
			Vector3 kValue_;                   // キーフレームの時の値
			SoLib::Time::SecondF time_; // キーフレームの時刻

			using value_type = Vector3;
		};

		template<>
		struct KeyFlameTemplate<Quaternion> : IKeyFlame {
			Quaternion kValue_;                   // キーフレームの時の値
			SoLib::Time::SecondF time_; // キーフレームの時刻

			using value_type = Quaternion;
		};

		/// @brief Vector3のキーフレーム
		using KeyFlameVector3 = KeyFlameTemplate<Vector3>;

		/// @brief Quaternionのキーフレーム
		using KeyFlameQuaternion = KeyFlameTemplate<Quaternion>;

		template <typename T>
		concept KeyFlameTypes = SoLib::IsBased<T, IKeyFlame>;

		template <KeyFlameTypes T>
		struct AnimationCurve : public std::vector<T> {

			using KeyFlamesType = std::vector<T>;

			operator KeyFlamesType &() { return GetKeyFlames(); }
			operator const KeyFlamesType &() const { return GetKeyFlames(); }

			KeyFlamesType &GetKeyFlames() { return static_cast<KeyFlamesType &>(*this); }
			const KeyFlamesType &GetKeyFlames() const { return static_cast<const KeyFlamesType &>(*this); }

			/// @brief キーフレームごとの値を計算する
			/// @param sec 秒数
			/// @return キーフレーム間を補完した値
			T::value_type CalcValue(const SoLib::Time::SecondF sec) const
			{
				const auto &keyFlames = static_cast<const std::vector<T>&>(*this);
				assert(not this->empty() and "キーフレームが存在しません");
				if (keyFlames.size() == 1 || sec < keyFlames.at(0).time_) { // キーが1個か､時刻がキーフレーム前なら最初の値とする
					return keyFlames.at(0).kValue_;
				}

				for (size_t index = 0; index < keyFlames.size() - 1; index++) {
					size_t nextIndex = index + 1;
					// indexとnextIndexの2つのkeyFlameを取得して範囲内に時刻があるかを判定
					if (keyFlames[index].time_ <= sec and sec <= keyFlames[nextIndex].time_) {
						// 範囲内を補完する
						float t = (sec - keyFlames[index].time_) / (keyFlames[nextIndex].time_ - keyFlames[index].time_);

						if constexpr (std::is_same_v<T::value_type, Quaternion>) {	// クォータニオンである場合はSlerpを使用する
							return Quaternion::Slerp(keyFlames[index].kValue_, keyFlames[nextIndex].kValue_, t);
						}
						else {
							return SoLib::Lerp(keyFlames[index].kValue_, keyFlames[nextIndex].kValue_, t);
						}
					}
					// もし現在の時間が既に範囲外なら､終わる｡
					else if (keyFlames[index].time_ > sec) { break; }
				}
				// ここまで来たら､一番最後の時刻より後ろなので､最後の時刻を返す｡
				return (*keyFlames.rbegin()).kValue_;
			}
		};

		// ノードごとのアニメーション ( AnimationCurve )
		struct NodeAnimation {

			AnimationCurve<KeyFlameVector3> scale_;     // スケール要素のAnimationCurve
			AnimationCurve<KeyFlameQuaternion> rotate_; // 回転要素のAnimationCurve
			AnimationCurve<KeyFlameVector3> translate_; // 平行移動要素のAnimationCurve
		};

		class Animation : public IResourceObject {
		public:
			SoLib::Time::SecondF duration_;                       // アニメーション全体の尺
			std::unordered_map<std::string, NodeAnimation> nodeAnimations_; // NodeAnimationの集合｡Node名で検索ができる｡

		};
	}

	using Animation = SkeletonAnimation::Animation;

	template <>
	class ResourceSource<Animation> {
	public:
		ResourceSource<Animation>() = default;
		ResourceSource<Animation>(const ResourceSource<Animation> &) = default;
		ResourceSource<Animation> &operator=(const ResourceSource<Animation> &) = default;
		ResourceSource<Animation>(const ResourceHandle<AssimpData> assimpHandle, uint32_t index = 0u) : assimpHandle_(assimpHandle), index_(index) {}

		// ファイルのハンドル
		ResourceHandle<AssimpData> assimpHandle_;
		// materialのmodel内Index
		uint32_t index_;

		bool operator==(const ResourceSource &other) const = default;
	};

	template <>
	class ResourceCreater<Animation> {
	public:

		std::unique_ptr<Animation> CreateObject(const ResourceSource<Animation> &source) const;

	};


}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::Animation>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::Animation> &data) const {
			return size_t{ static_cast<size_t>(data.assimpHandle_.GetHandle()) << 32u | static_cast<size_t>(data.index_) };
		}

	};
}