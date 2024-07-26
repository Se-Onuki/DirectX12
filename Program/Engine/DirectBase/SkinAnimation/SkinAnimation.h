#pragma once
#include "../../../Utils/SoLib/SoLib_Timer.h"
#include <unordered_map>
#include "../Model/AssimpData.h"
#include "../Model/ModelData.h"

namespace SolEngine {
	struct IKeyFlame {};

	template <SoLib::IsRealType T>
	struct KeyFlameTemplate : IKeyFlame {
		T kValue_;                   // キーフレームの時の値
		SoLib::Time::SecondF time_; // キーフレームの時刻

		using value_type = T;
	};

	/// @brief Vector3のキーフレーム
	using KeyFlameVector3 = KeyFlameTemplate<Vector3>;

	/// @brief Quaternionのキーフレーム
	using KeyFlameQuaternion = KeyFlameTemplate<Quaternion>;

	template <typename T>
	concept KeyFlameTypes = SoLib::IsBased<T, IKeyFlame>;

	template <KeyFlameTypes T>
	struct AnimationCurve {
		std::vector<T> keyFlames_;
		auto begin() { return keyFlames_.begin(); }
		auto begin() const { return keyFlames_.begin(); }
		auto cbegin() const { return keyFlames_.cbegin(); }

		auto end() { return keyFlames_.end(); }
		auto end() const { return keyFlames_.end(); }
		auto cend() const { return keyFlames_.cend(); }

		size_t size() const { return keyFlames_.size(); }

		T &at(size_t i) { return keyFlames_.at(i); }
		const T &at(size_t i) const { return keyFlames_.at(i); }

		T &operator[](size_t i) { return keyFlames_[i]; }
		const T &operator[](size_t i) const { return keyFlames_[i]; }

		T *data() { return keyFlames_.data(); }
		const T *data() const { return keyFlames_.data(); }


		/// @brief キーフレームごとの値を計算する
		/// @param sec 秒数
		/// @return キーフレーム間を補完した値
		T::value_type CalcValue(const SoLib::Time::SecondF sec) const
		{
			assert(not keyFlames_.empty() and "キーフレームが存在しません");
			if (keyFlames_.size() == 1 || sec < keyFlames_.at(0).time_) { // キーが1個か､時刻がキーフレーム前なら最初の値とする
				return keyFlames_.at(0).kValue_;
			}

			for (size_t index = 0; index < keyFlames_.size() - 1; index++) {
				size_t nextIndex = index + 1;
				// indexとnextIndexの2つのkeyFlameを取得して範囲内に時刻があるかを判定
				if (keyFlames_[index].time_ <= sec and sec <= keyFlames_[nextIndex].time_) {
					// 範囲内を補完する
					float t = (sec - keyFlames_[index].time_) / (keyFlames_[nextIndex].time_ - keyFlames_[index].time_);

					if constexpr (std::is_same_v<T::value_type, Quaternion>) {	// クォータニオンである場合はSlerpを使用する
						return Quaternion::Slerp(keyFlames_[index].kValue_, keyFlames_[nextIndex].kValue_, t);
					}
					else {
						return SoLib::Lerp(keyFlames_[index].kValue_, keyFlames_[nextIndex].kValue_, t);
					}
				}
			}
			// ここまで来たら､一番最後の時刻より後ろなので､最後の時刻を返す｡
			return (*keyFlames_.rbegin()).kValue_;
		}
	};
	// ノードごとのアニメーション ( AnimationCurve )
	struct NodeAnimation {

		AnimationCurve<KeyFlameVector3> scale_;     // スケール要素のAnimationCurve
		AnimationCurve<KeyFlameQuaternion> rotate_; // 回転要素のAnimationCurve
		AnimationCurve<KeyFlameVector3> translate_; // 平行移動要素のAnimationCurve
	};

	struct Animation {
		SoLib::Time::SecondF duration_;                       // アニメーション全体の尺
		std::unordered_map<std::string, NodeAnimation> nodeAnimations_; // NodeAnimationの集合｡Node名で検索ができる｡

		static std::unique_ptr<Animation> Create(const SolEngine::AssimpData *assimpData, uint32_t index = 0);
	};

	class AnimationPlayer {
	public:
		AnimationPlayer() = default;
		AnimationPlayer(const Animation *animation) : animation_(animation) {}

		inline void SetAnimation(const Animation *animation) { animation_ = animation; }

		const Animation *GetAnimation() const { return animation_; }

		void Start(bool isLoop = false);

		void Update(float deltaTime, SolEngine::ModelData *model);

		const SoLib::DeltaTimer &GetDeltaTimer() const {
			return animationTimer_;
		}

	private:

		/// @brief 再起的にモデルの姿勢を取得する
		/// @param animateTime アニメーションの時間
		/// @param modelNode モデルのノードの参照
		void CalcTransform(float animateTime, ModelNode *const modelNode) const;

		SoLib::Time::DeltaTimer animationTimer_; // アニメーションの時刻

		const Animation *animation_;
	};

}