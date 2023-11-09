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
