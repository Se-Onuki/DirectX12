#pragma once
#include <d3d12.h>
//#include <wrl.h>

class DirectXCommon
{
	//template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	DirectXCommon();
	~DirectXCommon();

	void Init();

	//ComPtr<ID3D12Device> device_;
};