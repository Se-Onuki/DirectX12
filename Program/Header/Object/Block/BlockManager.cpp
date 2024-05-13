#include "BlockManager.h"

void BlockManager::Init(uint32_t maxCount)
{
	blocks_.CreateBuffer(maxCount);

	// デバイスの取得
	auto *device = DirectXCommon::GetInstance()->GetDevice();
	// SRVヒープの取得
	auto *srvHeap = DirectXCommon::GetInstance()->GetSRVHeap();

	if (not heapRange_) {
		heapRange_ = srvHeap->RequestHeapAllocation(1);
		device->CreateShaderResourceView(blocks_.GetResources(), &blocks_.GetDesc(), heapRange_.GetHandle(0).cpuHandle_);
	}
}

void BlockManager::Draw(const Camera3D &camera)
{
	// 始点となる添え字
	uint32_t index = 0u;
	// サイズと始点を設定
	std::list<std::pair<uint32_t, uint32_t>> sizeAndIndex;
	// パーティクルのモデルと集合を1つづつ取得
	for (auto &[model, blockList] : blockMap_) {
		// パーティクルのリストを取得
		const auto &blocks = blockList->GetBlockList();
		// もし、サイズが0なら飛ばす
		if (blocks.size() == 0u) { continue; }
		// パーティクルの書き込み先を設定 
		uint32_t targetIndex = index;
		// パーティクルをひとつづつ取得
		for (const auto &block : blocks) {
			// 書き込み先にパーティクルのデータを渡す
			blocks_[targetIndex].transform.World = block.transMat_;
			blocks_[targetIndex].color = block.color_;
			targetIndex++;
		}
		// パーティクルの量と先頭位置を設定
		sizeAndIndex.push_back({ static_cast<uint32_t>(blocks.size()), index });
		blockList->SetLocation(index);
		// 次の先頭位置を更新
		index += static_cast<uint32_t>(blocks.size());
	}
	// 量と先頭位置のリストのイテレータ
	std::list<std::pair<uint32_t, uint32_t>>::const_iterator indexItr = sizeAndIndex.cbegin();
	// GPUアドレスを取得
	const auto &gpuAddress = heapRange_.GetHandle(0u).gpuHandle_;

	// 構造体のサイズ
	//const uint32_t matrixSize = sizeof(decltype(particles_)::map_matrix);

	// パーティクルのモデルと集合を1つづつ取得
	for (const auto &[model, particleList] : blockMap_) {

		// もし、サイズが0なら飛ばす
		if (particleList->GetBlockList().size() == 0u) { continue; }
		// インスタンシングで描画。
		model->Draw(gpuAddress, indexItr->first, particleList->GetLoaction(), camera);
		// 次の先頭位置に更新
		indexItr++;
	}
}

void BlockManager::Draw(const CBuffer<Camera3D::CameraMatrix> &camera) {
	// 始点となる添え字
	uint32_t index = 0u;
	// サイズと始点を設定
	std::list<std::pair<uint32_t, uint32_t>> sizeAndIndex;
	// パーティクルのモデルと集合を1つづつ取得
	for (auto &[model, blockList] : blockMap_) {
		// パーティクルのリストを取得
		const auto &blocks = blockList->GetBlockList();
		// もし、サイズが0なら飛ばす
		if (blocks.size() == 0u) { continue; }
		// パーティクルの書き込み先を設定 
		uint32_t targetIndex = index;
		// パーティクルをひとつづつ取得
		for (const auto &block : blocks) {
			// 書き込み先にパーティクルのデータを渡す
			blocks_[targetIndex].transform.World = block.transMat_;
			blocks_[targetIndex].color = block.color_;
			targetIndex++;
		}
		// パーティクルの量と先頭位置を設定
		sizeAndIndex.push_back({ static_cast<uint32_t>(blocks.size()), index });
		blockList->SetLocation(index);
		// 次の先頭位置を更新
		index += static_cast<uint32_t>(blocks.size());
	}
	// 量と先頭位置のリストのイテレータ
	std::list<std::pair<uint32_t, uint32_t>>::const_iterator indexItr = sizeAndIndex.cbegin();
	// GPUアドレスを取得
	const auto &gpuAddress = heapRange_.GetHandle(0u).gpuHandle_;

	// 構造体のサイズ
	//const uint32_t matrixSize = sizeof(decltype(particles_)::map_matrix);

	// パーティクルのモデルと集合を1つづつ取得
	for (const auto &[model, particleList] : blockMap_) {

		// もし、サイズが0なら飛ばす
		if (particleList->GetBlockList().size() == 0u) { continue; }
		// インスタンシングで描画。
		model->Draw(gpuAddress, indexItr->first, particleList->GetLoaction(), camera);
		// 次の先頭位置に更新
		indexItr++;
	}
}

IBlock *const BlockList::push_back(IBlock &&block)
{
	blocks_.push_back(std::move(block));
	return &blocks_.back();
}

void SkinModelListManager::Init(uint32_t maxCount)
{
	blocks_.CreateBuffer(maxCount);

	// デバイスの取得
	auto *device = DirectXCommon::GetInstance()->GetDevice();
	// SRVヒープの取得
	auto *srvHeap = DirectXCommon::GetInstance()->GetSRVHeap();

	if (not heapRange_) {
		heapRange_ = srvHeap->RequestHeapAllocation(1);
		device->CreateShaderResourceView(blocks_.GetResources(), &blocks_.GetDesc(), heapRange_.GetHandle(0).cpuHandle_);
	}
}

void SkinModelListManager::Draw(const Camera3D &camera)
{// 始点となる添え字
	uint32_t index = 0u;
	// サイズと始点を設定
	std::list<std::pair<uint32_t, uint32_t>> sizeAndIndex;
	// パーティクルのモデルと集合を1つづつ取得
	for (auto &[model, blockList] : blockMap_) {
		// パーティクルのリストを取得
		const auto &blocks = blockList->GetBlockList();
		// もし、サイズが0なら飛ばす
		if (blocks.size() == 0u) { continue; }
		// パーティクルの書き込み先を設定 
		uint32_t targetIndex = index;
		// パーティクルをひとつづつ取得
		for (const auto &block : blocks) {
			// 書き込み先にパーティクルのデータを渡す
			blocks_[targetIndex].transform.World = block.transMat_;
			blocks_[targetIndex].color = block.color_;
			targetIndex++;
		}
		// パーティクルの量と先頭位置を設定
		sizeAndIndex.push_back({ static_cast<uint32_t>(blocks.size()), index });
		blockList->SetLocation(index);
		// 次の先頭位置を更新
		index += static_cast<uint32_t>(blocks.size());
	}
	// 量と先頭位置のリストのイテレータ
	std::list<std::pair<uint32_t, uint32_t>>::const_iterator indexItr = sizeAndIndex.cbegin();
	// GPUアドレスを取得
	const auto &gpuAddress = heapRange_.GetHandle(0u).gpuHandle_;

	// 構造体のサイズ
	//const uint32_t matrixSize = sizeof(decltype(particles_)::map_matrix);

	// パーティクルのモデルと集合を1つづつ取得
	for (const auto &[model, particleList] : blockMap_) {

		// もし、サイズが0なら飛ばす
		if (particleList->GetBlockList().size() == 0u) { continue; }
		// インスタンシングで描画。
		model.first->Draw(*(model.second->skinCluster_), gpuAddress, indexItr->first, particleList->GetLoaction(), camera);
		// 次の先頭位置に更新
		indexItr++;
	}
}
