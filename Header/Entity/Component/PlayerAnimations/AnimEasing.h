#pragma once
#include <numbers>
#include "../../../../Utils/Math/Vector3.h"
#include "PlayerBone.h"

/// <summary>
/// アニメーション用イージング関数群
/// </summary>
class AnimEasing
{
public: // サブクラス

	/// <summary>
	/// イージングタイプ列挙子
	/// </summary>
	enum EaseingType {
		kLinear,
		kEaseIn,
		kEaseOut,
		KEaseInOut
	};

public: // パブリックなメンバ関数

	/// <summary>
	/// 線形補間関数(Normal)(float)
	/// </summary>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <param name="time">かかる秒数</param>
	/// <returns>イージングされた値(float)</returns>
	static float Linear(float t, float start, float end, float time);
	/// <summary>
	/// 線形補間関数(Normal)(int)
	/// </summary>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <param name="time">かかる秒数</param>
	/// <returns>イージングされた値(float)</returns>
	static int Linear(float t, int start, int end, float time);
	/// <summary>
	/// 線形補間関数(Normal)(Vector3)
	/// </summary>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <param name="time">かかる秒数</param>
	/// <returns>イージングされた値(float)</returns>
	static Vector3 Linear(float t, Vector3 start, Vector3 end, float time);
	/// <summary>
	/// イージングにかかる秒数を指定しない線形補間関数(Normal)(float)
	/// </summary>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <returns>イージングされた値(float)</returns>
	static float Linear(float t, float start, float end);
	/// <summary>
	/// イージングにかかる秒数を指定しない線形補間関数(Normal)(int)
	/// </summary>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <returns>イージングされた値(float)</returns>
	static int Linear(float t, int start, int end);
	/// <summary>
	/// イージングにかかる秒数を指定しない線形補間関数(Normal)(Vector3)
	/// </summary>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <returns>イージングされた値(float)</returns>
	static Vector3 Linear(float t, Vector3 start, Vector3 end);

	/// <summary>
	/// 線形補間関数(EaseIn)(float)
	/// </summary>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <param name="time">かかる秒数</param>
	/// <returns>イージングされた値(float)</returns>
	static float EaseIn(float t, float start, float end, float time);
	/// <summary>
	/// 線形補間関数(EaseIn)(int)
	/// </summary>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <param name="time">かかる秒数</param>
	/// <returns>イージングされた値(float)</returns>
	static int EaseIn(float t, int start, int end, float time);
	/// <summary>
	/// 線形補間関数(EaseIn)(Vector3)
	/// </summary>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <param name="time">かかる秒数</param>
	/// <returns>イージングされた値(Vector3)</returns>
	static Vector3 EaseIn(float t, Vector3 start, Vector3 end, float time);
	/// <summary>
	/// イージングにかかる秒数を指定しない線形補間関数(EaseIn)(float)
	/// </summary>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <returns>イージングされた値(float)</returns>
	static float EaseIn(float t, float start, float end);
	/// <summary>
	/// イージングにかかる秒数を指定しない線形補間関数(EaseIn)(int)
	/// </summary>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <returns>イージングされた値(float)</returns>
	static int EaseIn(float t, int start, int end);

	/// <summary>
	/// 線形補間関数(EaseOut)(float)
	/// </summary>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <param name="time">かかる秒数</param>
	/// <returns>イージングされた値(float)</returns>
	static float EaseOut(float t, float start, float end, float time);
	/// <summary>
	/// 線形補間関数(EaseOut)(int)
	/// </summary>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <param name="time">かかる秒数</param>
	/// <returns>イージングされた値(float)</returns>
	static int EaseOut(float t, int start, int end, float time);
	/// <summary>
	/// 線形補間関数(EaseOut)(Vector3)
	/// </summary>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <param name="time">かかる秒数</param>
	/// <returns>イージングされた値(Vector3)</returns>
	static Vector3 EaseOut(float t, Vector3 start, Vector3 end, float time);
	/// <summary>
	/// イージングにかかる秒数を指定しない線形補間関数(EaseOut)(float)
	/// </summary>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <returns>イージングされた値(float)</returns>
	static float EaseOut(float t, float start, float end);
	/// <summary>
	/// イージングにかかる秒数を指定しない線形補間関数(EaseOut)(int)
	/// </summary>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <returns>イージングされた値(float)</returns>
	static int EaseOut(float t, int start, int end);

	/// <summary>
	/// 線形補間関数(EaseInOut)(float)
	/// </summary>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <param name="time">かかる秒数</param>
	/// <returns>イージングされた値(float)</returns>
	static float EaseInOut(float t, float start, float end, float time);
	/// <summary>
	/// 線形補間関数(EaseInOut)(int)
	/// </summary>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <param name="time">かかる秒数</param>
	/// <returns>イージングされた値(float)</returns>
	static int EaseInOut(float t, int start, int end, float time);
	/// <summary>
	/// 線形補間関数(EaseInOut)(Vector3)
	/// </summary>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <param name="time">かかる秒数</param>
	/// <returns>イージングされた値(Vector3)</returns>
	static Vector3 EaseInOut(float t, Vector3 start, Vector3 end, float time);
	/// <summary>
	/// イージングにかかる秒数を指定しない線形補間関数(EaseInOut)(float)
	/// </summary>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <returns>イージングされた値(float)</returns>
	static float EaseInOut(float t, float start, float end);
	/// <summary>
	/// イージングにかかる秒数を指定しない線形補間関数(EaseInOut)(int)
	/// </summary>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <returns>イージングされた値(float)</returns>
	static int EaseInOut(float t, int start, int end);

	/// <summary>
	/// 輪っか以外の全てのボーンをイージングにて動作させる線形補間関数
	/// </summary>
	/// <param name="type">イージングタイプ</param>
	/// <param name="t">現在のt</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終端値</param>
	/// <param name="time">時間</param>
	/// <returns>イージングされた値(float)</returns>
	static PlayerBone::Bone Ease(EaseingType type, float t, PlayerBone::Bone start, PlayerBone::Bone end, float time);
};

