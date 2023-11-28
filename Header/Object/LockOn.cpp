#include "LockOn.h"
#include "../../Engine/DirectBase/Input/Input.h"

void LockOn::Init()
{
	sprite_.reset(Sprite::Create(TextureManager::Load("UI/circle.png")));
	sprite_->SetPivot(Vector2{ 0.5f, 0.5f });
	sprite_->SetScale(Vector2{ 128.f, 128.f });
}

void LockOn::Update([[maybe_unused]] const std::list<std::unique_ptr<Enemy>> &enemyList, [[maybe_unused]] const Camera3D &camera)
{
	static auto *const input = Input::GetInstance();
	if (input->GetXInput()->IsTrigger(KeyCode::Y)) {
		if (target_) {
			target_ = nullptr;
		}
		else {
			target_ = FindTarget(enemyList, camera);
		}
	}

	if (target_) {
		// 敵の中心座標を算出
		Vector3 positionWorld = Vector3::up * target_->GetWorldTransform()->matWorld_;

		// ビューポート行列
		const Matrix4x4 &matViewPort = Render::MakeViewportMatrix(Vector2{ 0.f, 0.f }, WinApp::kWindowWidth, WinApp::kWindowHeight, 0.f, 1.f);

		// スクリーン変換行列
		const Matrix4x4 matVPVp = camera.matView_ * camera.matProjection_ * matViewPort;

		// ワールド座標からスクリーン座標に変換
		Vector3 posScrean = Render::WorldToScreen(positionWorld, matVPVp);

		sprite_->SetPosition(posScrean.ToVec2());
	}
}

void LockOn::Draw2D() const
{
	if (target_) {
		sprite_->Draw();
	}
}

const Enemy *const LockOn::FindTarget(const std::list<std::unique_ptr<Enemy>> &enemyList, const Camera3D &camera) const
{
	// 距離 : ターゲットのペア
	std::list<std::pair<float, const Enemy *>> targetList;
	for (const auto &enemy : enemyList) {
		// ロックオン対象のグローバル座標
		Vector3 positionWorld = Vector3::up * enemy->GetWorldTransform()->matWorld_;
		// ビュー座標の取得
		Vector3 positionView = positionWorld * camera.matView_;

		// y要素を排除して xzのみで計算する
		positionView.y = positionView.y;

		// z座標が最小距離から最大距離の間に存在するか
		if (minDistance_ <= positionView.z && positionView.z <= maxDistance_) {
			// 角度の取得
			float angle = Vector3::front * positionView.Nomalize();
			// 角度範囲内ならtargetに追加
			if (angle > lockAngle_) {
				targetList.emplace_back(std::make_pair(positionView.z, enemy.get()));
			}
		}
	}

	const Enemy *target = nullptr;
	// targetListが空でなければ実行
	if (targetList.size()) {
		// 距離順でソート
		targetList.sort([](auto &pair1, auto &pair2) -> bool { return pair1.first < pair2.first; });

		target = targetList.front().second;
	}

	return target;
}