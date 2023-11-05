#include "AnimEasing.h"

float AnimEasing::Linear(float t, float start, float end, float time)
{
    return (float)((1.0 - t / time) * start + t / time * end);
}

int AnimEasing::Linear(float t, int start, int end, float time)
{
    return (int)((1.0 - t / time) * (float)start + t / time * (float)end);
}

Vector3 AnimEasing::Linear(float t, Vector3 start, Vector3 end, float time)
{
	// 結果格納用
	Vector3 result;

	result.x = (float)((1.0 - t / time) * start.x + t / time * end.x);
	result.y = (float)((1.0 - t / time) * start.y + t / time * end.y);
	result.z = (float)((1.0 - t / time) * start.z + t / time * end.z);

	return result;
}

float AnimEasing::Linear(float t, float start, float end) {
	return (float)((1.0 - t) * start + t * end);
}

int AnimEasing::Linear(float t, int start, int end) {
	return (int)((1.0 - t) * (float)start + t * (float)end);
}

Vector3 AnimEasing::Linear(float t, Vector3 start, Vector3 end)
{
	// 結果格納用
	Vector3 result;
	result.x = (float)((1.0 - t) * start.x + t * end.x);
	result.y = (float)((1.0 - t) * start.y + t * end.y);
	result.z = (float)((1.0 - t) * start.z + t * end.z);

	return result;
}

float AnimEasing::EaseIn(float t, float start, float end, float time) {
	float easeT = t / time * t / time;
	return (float)((1.0 - easeT) * start + easeT * end);
}

int AnimEasing::EaseIn(float t, int start, int end, float time) {
	float easeT = t / time * t / time;
	return (int)((1.0 - easeT * (float)start + easeT * (float)end));
}

Vector3 AnimEasing::EaseIn(float t, Vector3 start, Vector3 end, float time) {
	// 結果格納用
	Vector3 result;

	float easeT = t / time * t / time;

	result.x = (float)((1.0 - easeT) * start.x + easeT * end.x);
	result.y = (float)((1.0 - easeT) * start.y + easeT * end.y);
	result.z = (float)((1.0 - easeT) * start.z + easeT * end.z);

	return result;
}

float AnimEasing::EaseIn(float t, float start, float end)
{
	float easeT = t * t;
	return (float)((1.0 - easeT) * start + easeT * end);
}

int AnimEasing::EaseIn(float t, int start, int end) {
	float easeT = t * t;
	return (int)((1.0 - easeT) * (float)start + easeT * (float)end);
}

float AnimEasing::EaseOut(float t, float start, float end, float time) {
	float easeT = 1.0f - powf(1.0f - t / time, 3.0f);
	return (float)((1.0 - easeT) * start + easeT * end);
}

int AnimEasing::EaseOut(float t, int start, int end, float time) {
	float easeT = 1.0f - powf(1.0f - t / time, 3.0f);
	return (int)((1.0 - easeT) * (float)start + easeT * (float)end);
}

Vector3 AnimEasing::EaseOut(float t, Vector3 start, Vector3 end, float time) {
	// 結果格納用
	Vector3 result;

	float easeT = 1.0f - powf(1.0f - t / time, 3.0f);

	result.x = (float)((1.0 - easeT) * start.x + easeT * end.x);
	result.y = (float)((1.0 - easeT) * start.y + easeT * end.y);
	result.z = (float)((1.0 - easeT) * start.z + easeT * end.z);

	return result;
}

float AnimEasing::EaseOut(float t, float start, float end)
{
	float easeT = 1.0f - powf(1.0f - t, 3.0f);
	return (float)((1.0 - easeT) * start + easeT * end);
}

int AnimEasing::EaseOut(float t, int start, int end) {
	float easeT = 1.0f - powf(1.0f - t, 3.0f);
	return (int)((1.0 - easeT) * (float)start + easeT * (float)end);
}

float AnimEasing::EaseInOut(float t, float start, float end, float time)
{
	float easeT = -(cosf((float)std::numbers::pi * t / time) - 1.0f) / 2.0f;
	return (float)((1.0 - easeT) * start + easeT * end);
}

int AnimEasing::EaseInOut(float t, int start, int end, float time)
{
	float easeT = -(cosf((float)std::numbers::pi * t / time) - 1.0f) / 2.0f;
	return (int)((1.0 - easeT) * start + easeT * end);
}

Vector3 AnimEasing::EaseInOut(float t, Vector3 start, Vector3 end, float time)
{
	// 結果格納用
	Vector3 result;

	float easeT = -(cosf((float)std::numbers::pi * t / time) - 1.0f) / 2.0f;

	result.x = (float)((1.0 - easeT) * start.x + easeT * end.x);
	result.y = (float)((1.0 - easeT) * start.y + easeT * end.y);
	result.z = (float)((1.0 - easeT) * start.z + easeT * end.z);

	return result;

}

float AnimEasing::EaseInOut(float t, float start, float end)
{
	float easeT = -(cosf((float)std::numbers::pi * t) - 1.0f) / 2.0f;
	return (float)((1.0 - easeT) * start + easeT * end);
}

int AnimEasing::EaseInOut(float t, int start, int end)
{
	float easeT = -(cosf((float)std::numbers::pi * t) - 1.0f) / 2.0f;
	return (int)((1.0 - easeT) * start + easeT * end);
}

PlayerBone::Bone AnimEasing::Ease(EaseingType type, float t, PlayerBone::Bone start, PlayerBone::Bone end, float time)
{
	PlayerBone::Bone tempBone;
	tempBone.Initilaize();
	switch (type)
	{
	case AnimEasing::kLinear:
		tempBone.body.scale = AnimEasing::Linear(t, start.body.scale, end.body.scale, time);
		tempBone.body.rotate = AnimEasing::Linear(t, start.body.rotate, end.body.rotate, time);
		tempBone.body.translate = AnimEasing::Linear(t, start.body.translate, end.body.translate, time);
		tempBone.eye.scale = AnimEasing::Linear(t, start.eye.scale, end.eye.scale, time);
		tempBone.eye.rotate = AnimEasing::Linear(t, start.eye.rotate, end.eye.rotate, time);
		tempBone.eye.translate = AnimEasing::Linear(t, start.eye.translate, end.eye.translate, time);
		tempBone.arm_L.scale = AnimEasing::Linear(t, start.arm_L.scale, end.arm_L.scale, time);
		tempBone.arm_L.rotate = AnimEasing::Linear(t, start.arm_L.rotate, end.arm_L.rotate, time);
		tempBone.arm_L.translate = AnimEasing::Linear(t, start.arm_L.translate, end.arm_L.translate, time);
		tempBone.arm_R.scale = AnimEasing::Linear(t, start.arm_R.scale, end.arm_R.scale, time);
		tempBone.arm_R.rotate.x = AnimEasing::Linear(t, start.arm_R.rotate.x, end.arm_R.rotate.x, time);
		tempBone.arm_R.rotate.y = AnimEasing::Linear(t, start.arm_R.rotate.y, end.arm_R.rotate.y, time);
		tempBone.arm_R.rotate.z = -AnimEasing::Linear(t, start.arm_R.rotate.z, end.arm_R.rotate.z, time);
		tempBone.arm_R.translate = AnimEasing::Linear(t, start.arm_R.translate, end.arm_R.translate, time);
		tempBone.foot_L.scale = AnimEasing::Linear(t, start.foot_L.scale, end.foot_L.scale, time);
		tempBone.foot_L.rotate = AnimEasing::Linear(t, start.foot_L.rotate, end.foot_L.rotate, time);
		tempBone.foot_L.translate = AnimEasing::Linear(t, start.foot_L.translate, end.foot_L.translate, time);
		tempBone.foot_R.scale = AnimEasing::Linear(t, start.foot_R.scale, end.foot_R.scale, time);
		tempBone.foot_R.rotate = AnimEasing::Linear(t, start.foot_R.rotate, end.foot_R.rotate, time);
		tempBone.foot_R.translate = AnimEasing::Linear(t, start.foot_R.translate, end.foot_R.translate, time);
		break;
	case AnimEasing::kEaseIn:
		tempBone.body.scale = AnimEasing::EaseIn(t, start.body.scale, end.body.scale, time);
		tempBone.body.rotate = AnimEasing::EaseIn(t, start.body.rotate, end.body.rotate, time);
		tempBone.body.translate = AnimEasing::EaseIn(t, start.body.translate, end.body.translate, time);
		tempBone.eye.scale = AnimEasing::EaseIn(t, start.eye.scale, end.eye.scale, time);
		tempBone.eye.rotate = AnimEasing::EaseIn(t, start.eye.rotate, end.eye.rotate, time);
		tempBone.eye.translate = AnimEasing::EaseIn(t, start.eye.translate, end.eye.translate, time);
		tempBone.arm_L.scale = AnimEasing::EaseIn(t, start.arm_L.scale, end.arm_L.scale, time);
		tempBone.arm_L.rotate = AnimEasing::EaseIn(t, start.arm_L.rotate, end.arm_L.rotate, time);
		tempBone.arm_L.translate = AnimEasing::EaseIn(t, start.arm_L.translate, end.arm_L.translate, time);
		tempBone.arm_R.scale = AnimEasing::EaseIn(t, start.arm_R.scale, end.arm_R.scale, time);
		tempBone.arm_R.rotate.x = AnimEasing::EaseIn(t, start.arm_R.rotate.x, end.arm_R.rotate.x, time);
		tempBone.arm_R.rotate.y = AnimEasing::EaseIn(t, start.arm_R.rotate.y, end.arm_R.rotate.y, time);
		tempBone.arm_R.rotate.z = -AnimEasing::EaseIn(t, start.arm_R.rotate.z, end.arm_R.rotate.z, time);
		tempBone.arm_R.translate = AnimEasing::EaseIn(t, start.arm_R.translate, end.arm_R.translate, time);
		tempBone.foot_L.scale = AnimEasing::EaseIn(t, start.foot_L.scale, end.foot_L.scale, time);
		tempBone.foot_L.rotate = AnimEasing::EaseIn(t, start.foot_L.rotate, end.foot_L.rotate, time);
		tempBone.foot_L.translate = AnimEasing::EaseIn(t, start.foot_L.translate, end.foot_L.translate, time);
		tempBone.foot_R.scale = AnimEasing::EaseIn(t, start.foot_R.scale, end.foot_R.scale, time);
		tempBone.foot_R.rotate = AnimEasing::EaseIn(t, start.foot_R.rotate, end.foot_R.rotate, time);
		tempBone.foot_R.translate = AnimEasing::EaseIn(t, start.foot_R.translate, end.foot_R.translate, time);
		break;
	case AnimEasing::kEaseOut:
		tempBone.body.scale = AnimEasing::EaseOut(t, start.body.scale, end.body.scale, time);
		tempBone.body.rotate = AnimEasing::EaseOut(t, start.body.rotate, end.body.rotate, time);
		tempBone.body.translate = AnimEasing::EaseOut(t, start.body.translate, end.body.translate, time);
		tempBone.eye.scale = AnimEasing::EaseOut(t, start.eye.scale, end.eye.scale, time);
		tempBone.eye.rotate = AnimEasing::EaseOut(t, start.eye.rotate, end.eye.rotate, time);
		tempBone.eye.translate = AnimEasing::EaseOut(t, start.eye.translate, end.eye.translate, time);
		tempBone.arm_L.scale = AnimEasing::EaseOut(t, start.arm_L.scale, end.arm_L.scale, time);
		tempBone.arm_L.rotate = AnimEasing::EaseOut(t, start.arm_L.rotate, end.arm_L.rotate, time);
		tempBone.arm_L.translate = AnimEasing::EaseOut(t, start.arm_L.translate, end.arm_L.translate, time);
		tempBone.arm_R.scale = AnimEasing::EaseOut(t, start.arm_R.scale, end.arm_R.scale, time);
		tempBone.arm_R.rotate.x = AnimEasing::EaseOut(t, start.arm_R.rotate.x, end.arm_R.rotate.x, time);
		tempBone.arm_R.rotate.y = AnimEasing::EaseOut(t, start.arm_R.rotate.y, end.arm_R.rotate.y, time);
		tempBone.arm_R.rotate.z = -AnimEasing::EaseOut(t, start.arm_R.rotate.z, end.arm_R.rotate.z, time);
		tempBone.arm_R.translate = AnimEasing::EaseOut(t, start.arm_R.translate, end.arm_R.translate, time);
		tempBone.foot_L.scale = AnimEasing::EaseOut(t, start.foot_L.scale, end.foot_L.scale, time);
		tempBone.foot_L.rotate = AnimEasing::EaseOut(t, start.foot_L.rotate, end.foot_L.rotate, time);
		tempBone.foot_L.translate = AnimEasing::EaseOut(t, start.foot_L.translate, end.foot_L.translate, time);
		tempBone.foot_R.scale = AnimEasing::EaseOut(t, start.foot_R.scale, end.foot_R.scale, time);
		tempBone.foot_R.rotate = AnimEasing::EaseOut(t, start.foot_R.rotate, end.foot_R.rotate, time);
		tempBone.foot_R.translate = AnimEasing::EaseOut(t, start.foot_R.translate, end.foot_R.translate, time);
		break;
	case AnimEasing::KEaseInOut:
		tempBone.body.scale = AnimEasing::EaseInOut(t, start.body.scale, end.body.scale, time);
		tempBone.body.rotate = AnimEasing::EaseInOut(t, start.body.rotate, end.body.rotate, time);
		tempBone.body.translate = AnimEasing::EaseInOut(t, start.body.translate, end.body.translate, time);
		tempBone.eye.scale = AnimEasing::EaseInOut(t, start.eye.scale, end.eye.scale, time);
		tempBone.eye.rotate = AnimEasing::EaseInOut(t, start.eye.rotate, end.eye.rotate, time);
		tempBone.eye.translate = AnimEasing::EaseInOut(t, start.eye.translate, end.eye.translate, time);
		tempBone.arm_L.scale = AnimEasing::EaseInOut(t, start.arm_L.scale, end.arm_L.scale, time);
		tempBone.arm_L.rotate = AnimEasing::EaseInOut(t, start.arm_L.rotate, end.arm_L.rotate, time);
		tempBone.arm_L.translate = AnimEasing::EaseInOut(t, start.arm_L.translate, end.arm_L.translate, time);
		tempBone.arm_R.scale = AnimEasing::EaseInOut(t, start.arm_R.scale, end.arm_R.scale, time);
		tempBone.arm_R.rotate.x = AnimEasing::EaseInOut(t, start.arm_R.rotate.x, end.arm_R.rotate.x, time);
		tempBone.arm_R.rotate.y = AnimEasing::EaseInOut(t, start.arm_R.rotate.y, end.arm_R.rotate.y, time);
		tempBone.arm_R.rotate.z = -AnimEasing::EaseInOut(t, start.arm_R.rotate.z, end.arm_R.rotate.z, time);
		tempBone.arm_R.translate = AnimEasing::EaseInOut(t, start.arm_R.translate, end.arm_R.translate, time);
		tempBone.foot_L.scale = AnimEasing::EaseInOut(t, start.foot_L.scale, end.foot_L.scale, time);
		tempBone.foot_L.rotate = AnimEasing::EaseInOut(t, start.foot_L.rotate, end.foot_L.rotate, time);
		tempBone.foot_L.translate = AnimEasing::EaseInOut(t, start.foot_L.translate, end.foot_L.translate, time);
		tempBone.foot_R.scale = AnimEasing::EaseInOut(t, start.foot_R.scale, end.foot_R.scale, time);
		tempBone.foot_R.rotate = AnimEasing::EaseInOut(t, start.foot_R.rotate, end.foot_R.rotate, time);
		tempBone.foot_R.translate = AnimEasing::EaseInOut(t, start.foot_R.translate, end.foot_R.translate, time);
		break;
	}

	return tempBone;
}
