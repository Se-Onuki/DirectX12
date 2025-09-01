/// @file ImGuiObject.hpp
/// @brief ImGuiのオブジェクトによる管理
/// @author ONUKI seiya
#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <optional>
#include <cstdint>
#include <imgui.h>
#include <imgui_stdlib.h>
#include "../Success.h"
#include "../SoLib/SoLib_Traits.h"

#include "../../DirectBase/Base/TextureManager.h"

namespace SoLib::ImGuiObject {
	/*
		UiNode (抽象)
		├─ UiContainer(抽象)  … 子ノードを複数持てる
		│    ├─ UiWindow
		│    ├─ UiPanel / UiGroup
		│    └─ UiDockSpace / UiSplit
		└─ UiLeaf(抽象)      … 子を持たない
		├─ UiButton
		├─ UiCheckbox / UiCombo / UiInputText 等
		└─ UiPlot / UiImage など
	*/

	/// @brief 基底となるUIノード
	class UiNode {
	public:
		virtual ~UiNode() = default;

		/// @brief 毎フレーム呼び出されるImGuiの構築関数
		/// @return 構築に成功したか否か
		virtual IsSuccess Build() = 0;

		/// @brief 可視状態の設定
		/// @param[in] visible 可視化フラグ
		void SetVisible(bool visible) {
			visible_ = visible;
		}

		/// @brief 可視状態の取得
		/// @return 可視化フラグ
		bool IsVisible() const {
			return visible_;
		}

		/// @brief ノードの文字列を取得する
		/// @return ノードの文字列
		const std::string_view GetId() const {
			return id_;
		}

	protected:
		/// @brief 明示的に呼び出される構築子
		/// @param[in] id ノードID
		explicit UiNode(const std::string &id) : id_(id) {}

		/// @brief ノードのID
		std::string id_;
		/// @brief ノードの表示状態
		bool visible_ = true;
	};


	class UiContainer : public UiNode {
	public:
		explicit UiContainer(const std::string &id) : UiNode(id) {}

		void AddChild(std::unique_ptr<UiNode> child) {
			children_.emplace_back(std::move(child));
		}

	protected:

		/// @brief 子ノードすべてのBuildを呼び出す
		/// @return Buildが成功したかどうか
		IsSuccess Build() override {

			// 子ノードが存在しない場合は何もしない
			if (children_.empty()) {
				return false; // 子ノードがない場合は何もしない
			}

			// すべての子ノードに対してBuildを呼び出す
			for (auto &child : children_) {
				// 子ノードが存在し、可視状態であればBuildを呼び出す
				if (child && child->IsVisible()) {
					child->Build();
				}
			}
			// 描画に成功した場合はtrueを返す
			return true;
		}


	private:
		/// @brief 子ノードのリスト
		std::vector<std::unique_ptr<UiNode>> children_;
	};

	/// @brief Begin/Endのスコープを管理するクラス
	class ImGuiScopedWindow {
	public:
		ImGuiScopedWindow(const char *title, bool *pOpen = nullptr, ImGuiWindowFlags flags = 0) :
			// ウィンドウの開始処理の返り値を格納
			active_(ImGui::Begin(title, pOpen, flags))
		{
		}

		~ImGuiScopedWindow() {
			// ウィンドウが有効ならEnd処理を行う
			if (active_) {
				ImGui::End();
			}
		}
		bool IsActive() const {
			return active_;
		}

	private:
		// スコープがアクティブかどうか
		bool active_ = false;
	};

	class ImGuiScopedID {
	public:
		ImGuiScopedID(const char *const id) {
			// idが無効であったら
			if (id == nullptr or *id == '\0') {
				isIdActive_ = false;
				return;
			}
			ImGui::PushID(id);
			isIdActive_ = true;
		}
		~ImGuiScopedID() {
			if (not isIdActive_) { return; }
			ImGui::PopID();
		}
		/// @brief IDの構築が有効か
		/// @return IDがPushされているならTrue
		bool IsActive() const { return isIdActive_; }
	private:
		bool isIdActive_ = false;
	};

	class UiWindow : public UiContainer {
	public:
		/// @brief コンストラクタ
		/// @param[in] id ノードID
		/// @param title ImGuiのウィンドウタイトル
		/// @param pOpen 外部借用の開閉フラグ
		UiWindow(const std::string &id, const std::string &title, bool *pOpen = nullptr)
			: UiContainer(id), title_(title), pOpen_(pOpen) {
		}

		IsSuccess Build() override {
			if (not pOpen_ or not *pOpen_) {
				return false; // ウィンドウが閉じている場合は何もしない
			}
			// もし構築できない状態なら終わる
			if (title_.empty()) {
				return false;
			}

			// ImGuiのウィンドウを開始
			ImGuiScopedWindow scopedWindow(title_.c_str(), pOpen_, flags_);
			if (not scopedWindow.IsActive()) {
				return false; // ウィンドウがアクティブでない場合は何もしない
			}

			// IDのスコープを管理する
			ImGuiScopedID scopedId(id_.c_str());
			// 子ノードのBuildを呼び出す
			UiContainer::Build();

		}

		/// @brief フラグの設定
		/// @param[in] flags フラグ
		void SetFlags(ImGuiWindowFlags flags) {
			flags_ = flags;
		}

	private:
		/// @brief 表示タイトル
		std::string title_;
		/// @brief ウィンドウの開閉状態を管理するポインタ
		bool *pOpen_ = nullptr;
		/// @brief ウィンドウフラグ
		ImGuiWindowFlags flags_ = 0;
	};

	template<IsRealType T>
	class ValueSource {
	public:
		using Getter = std::function<T()>;
		using Setter = std::function<void(const T &)>;

		ValueSource(Getter g, Setter s) : getter_(std::move(g)), setter_(std::move(s)) {}
		ValueSource(T &value) : getter_([&value]() {return value; }), setter_([&value](const T &v) { value = v; }) {}

		/// @brief 値を取得する
		/// @return 値
		T Get() const {
			return getter_();
		}

		/// @brief 値を設定する
		/// @param[in] value 設定する値
		void Set(const T &value) const {
			setter_(value);
		}

	private:
		/// @brief 値を取得する関数
		Getter getter_;
		/// @brief 値を設定する関数
		Setter setter_;
	};

	class UiButton : public UiNode {
	public:
		using Command = std::function<void()>;

		UiButton(const std::string &id, const std::string &label, const Command &command)
			: UiNode(id), label_(label), command_(command) {
		}

		IsSuccess Build() override {
			if (label_.empty() or command_) {
				return false; // IDやラベルが空の場合は何もしない
			}

			ImGuiScopedID scopedId(id_.c_str());
			// ボタンの表示
			if (ImGui::Button(label_.c_str())) {
				if (command_) { command_(); }
			}
		}

	private:

		/// @brief ボタンの表示名
		std::string label_;

		/// @brief ボタンが押されたときのコマンド
		Command command_;

	};

	//class UiImageButton : public UiNode {
	//public:

	//	//IsSuccess Build() override {

	//	//	ImGui::ImageButton(texture_->GetTextureID(), );

	//	//}

	//private:
	//	TextureManager::TextureHandle texture_;
	//};

	class UiInputText : public UiNode {
	public:

		UiInputText(const std::string &id, const std::string &label, const ValueSource<std::string> &source, const std::string &help = "")
			: UiNode(id), label_(label), source_(source), help_(help) {
		}

		IsSuccess Build() override {
			ImGuiScopedID scopedID(id_.c_str());

			buffer_ = source_.Get();

			if (help_.empty()) {
				if (ImGui::InputText(label_.c_str(), &buffer_)) {
					source_.Set(buffer_);
				}
			}
			else {
				if (ImGui::InputTextWithHint(label_.c_str(), help_.c_str(), &buffer_)) {
					source_.Set(buffer_);
				}
			}
		}


	private:

		std::string label_;
		std::string help_;
		ValueSource<std::string> source_;
		std::string buffer_;
	};

} // namespace SoLib::ImGuiObject