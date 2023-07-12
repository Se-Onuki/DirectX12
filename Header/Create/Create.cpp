#include "Create.h"
#include "../Render/Render.hpp"
#include "../Math/Math.hpp"
#include "../Math/Vector3.h"

#include <numbers>
#include <wrl.h>

Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device *device, const size_t &sizeBytes)
{
	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	// バッファリソース。 テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeBytes;					// リソースのサイズ。今回はVector4を3頂点分
	// バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	assert(SUCCEEDED(device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource))));
	return vertexResource;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ID3D12Device *device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
	// ディスクリプタヒープの生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	/*HRESULT hr = ;*/
	// デスクリプタヒープが生成できなかったので起動できない
	assert(SUCCEEDED(device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap))));
	return descriptorHeap;
}


void CreateSphere(Render::VertexData *const vertex,
	ID3D12Resource *const indexResource, const uint32_t &subdivision) {
	const float kLonEvery = 360u * Angle::Dig2Rad / subdivision; // 経度
	const float kLatEvery = 180u * Angle::Dig2Rad / subdivision; // 緯度

	// 緯度の方向に分解 -90 ~ 90
	for (uint32_t latIndex = 0u; latIndex < subdivision; ++latIndex) {
		const float lat = static_cast<float>(-std::numbers::pi) / 2.f + kLatEvery * latIndex;
		// 経度の方向に分解 0 ~ 360
		for (uint32_t lonIndex = 0u; lonIndex < subdivision; ++lonIndex) {
			uint32_t indexStart = (latIndex * subdivision + lonIndex) * 4;
			const float lon = lonIndex * kLonEvery;

#pragma region Vertex[0] 左下

			Render::VertexData *vertexData = &vertex[indexStart + 0u];
			vertexData->position = { //左下
				std::cos(lat) * std::cos(lon),std::sin(lat),
				std::cos(lat) * std::sin(lon), 1
			};
			vertexData->texCoord = {
				static_cast<float>(lonIndex) / subdivision,
				1.f - static_cast<float>(latIndex) / subdivision
			};
			vertexData->normal.x = vertexData->position.x;
			vertexData->normal.y = vertexData->position.y;
			vertexData->normal.z = vertexData->position.z;
			vertexData = nullptr;

#pragma endregion

#pragma region Vertex[1] 左上

			vertexData = &vertex[indexStart + 1u];
			vertexData->position = { //左上
				std::cos(lat + kLatEvery) * std::cos(lon),
				std::sin(lat + kLatEvery),
				std::cos(lat + kLatEvery) * std::sin(lon), 1
			};
			vertexData->texCoord = {
				static_cast<float>(lonIndex) / subdivision,
				1.f - static_cast<float>(latIndex + 1u) / subdivision
			};
			vertexData->normal.x = vertexData->position.x;
			vertexData->normal.y = vertexData->position.y;
			vertexData->normal.z = vertexData->position.z;
			vertexData = nullptr;

#pragma endregion

#pragma region Vertex[2] 右下

			vertexData = &vertex[indexStart + 2u];
			vertexData->position = { //右下
				std::cos(lat) * std::cos(lon + kLonEvery),
				std::sin(lat),
				std::cos(lat) * std::sin(lon + kLonEvery), 1
			};
			vertexData->texCoord = {
				static_cast<float>(lonIndex + 1u) / subdivision,
				1.f - static_cast<float>(latIndex) / subdivision
			};
			vertexData->normal.x = vertexData->position.x;
			vertexData->normal.y = vertexData->position.y;
			vertexData->normal.z = vertexData->position.z;
			vertexData = nullptr;

#pragma endregion
			//
			//#pragma region Vertex[3] 左上
			//
			//			vertexData = &vertex[indexStart + 3u];
			//			vertexData->position = { //左上
			//				std::cos(lat + kLatEvery) * std::cos(lon),
			//				std::sin(lat + kLatEvery),
			//				std::cos(lat + kLatEvery) * std::sin(lon), 1
			//			};
			//			vertexData->texCoord = {
			//				static_cast<float>(lonIndex) / subdivision,
			//				1.f - static_cast<float>(latIndex + 1u) / subdivision
			//			};
			//			vertexData->normal.x = vertexData->position.x;
			//			vertexData->normal.y = vertexData->position.y;
			//			vertexData->normal.z = vertexData->position.z;
			//			vertexData = nullptr;
			//
			//#pragma endregion

#pragma region Vertex[3] 右上

			vertexData = &vertex[indexStart + 3u];
			vertexData->position = { //右上
				std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery),
				std::sin(lat + kLatEvery),
				std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery), 1
			};
			vertexData->texCoord = {
				static_cast<float>(lonIndex + 1u) / subdivision,
				1.f - static_cast<float>(latIndex + 1u) / subdivision
			};
			vertexData->normal.x = vertexData->position.x;
			vertexData->normal.y = vertexData->position.y;
			vertexData->normal.z = vertexData->position.z;
			vertexData = nullptr;

#pragma endregion
			//
			//#pragma region Vertex[5] 右下
			//
			//			vertexData = &vertex[indexStart + 5u];
			//			vertexData->position = { //右下
			//				std::cos(lat) * std::cos(lon + kLonEvery),
			//				std::sin(lat),
			//				std::cos(lat) * std::sin(lon + kLonEvery), 1
			//			};
			//			vertexData->texCoord = {
			//				static_cast<float>(lonIndex + 1u) / subdivision,
			//				1.f - static_cast<float>(latIndex) / subdivision
			//			};
			//			vertexData->normal.x = vertexData->position.x;
			//			vertexData->normal.y = vertexData->position.y;
			//			vertexData->normal.z = vertexData->position.z;
			//			vertexData = nullptr;
			//
			//#pragma endregion

		}
	}

	uint32_t *indexData =
		nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void **>(&indexData));
	for (uint32_t i = 0; i < subdivision * subdivision; i++) {

		indexData[i * 6u + 0u] = i * 4u + 0u; indexData[i * 6u + 1u] = i * 4u + 1u; indexData[i * 6u + 2u] = i * 4u + 2u;
		indexData[i * 6u + 3u] = i * 4u + 1u; indexData[i * 6u + 4u] = i * 4u + 3u; indexData[i * 6u + 5u] = i * 4u + 2u;

	}
}