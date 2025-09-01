/// @file NewImGui.h
/// @brief ImGuiのラッピング
/// @author ONUKI seiya
#pragma once
#include "SoLib_Traits.h"
#include "../Success.h"
#include <imgui.h>

namespace SoLib {
	namespace NewImGui {

		namespace Test {

			/// @brief スコープ管理のクラス
			class ScopedID {
			public:
				ScopedID(const void *id) { ImGui::PushID(id); }
				~ScopedID() { ImGui::PopID(); }
			};
			/// @brief スコープの無効化の管理を行うクラス
			class ScopedDisable {
			public:
				ScopedDisable(bool disabled) {
					if (disabled) { ImGui::BeginDisabled(true); wasDisabled_ = true; }
				}
				~ScopedDisable() { if (wasDisabled_) ImGui::EndDisabled(); }
			private:

				bool wasDisabled_{};
			};

			/// @brief ImGuiのコンテキストを管理するクラス
			class UiContext {
			public:
				/// @brief コマンドのコンテキスト
				ImGuiContext *ctx_{};
			};

			/// @brief UIが描画可能かどうかを判定するコンセプト
			template<class T>
			concept Drawable = requires(T t, UiContext & c) {
				{ t.Draw(c) } -> std::same_as<void>;
			};

			/// @brief 
			class IWidget {
			public:
				virtual ~IWidget() = default;
			};

			class Container : public IWidget {
			public:

				/// @brief コンテナにウィジェットを追加する
				/// @tparam ...Args 引数の型
				/// @tparam W 追加するウィジェットの型
				/// @param ...args ウィジェットのコンストラクタに渡す引数
				/// @return 追加されたウィジェットの参照
				template<Drawable W, class... Args>
				W &Emplace(Args&&... args) {
					// ウィジェットにパラメータを渡してインスタンス化する
					auto ptr = std::make_unique<W>(std::forward<Args>(args)...);
					// ポインタから参照を取得する
					auto &ref = *ptr;
					// 子供のリストに追加する
					children_.push_back(std::move(ptr));
					// 参照を返す
					return ref;
				}

				/// @brief コンテナ内の全てのウィジェットを描画する
				/// @param[in] ctx 描画コンテキスト
				void Draw(UiContext &ctx) {
					for (auto &c : children_) {
						if (auto *d = dynamic_cast<DrawableBase *>(c.get())) {
							d->Draw(ctx);
						}
					}
				}

			private:
				struct DrawableBase {
					virtual void Draw(UiContext &) = 0;
					virtual ~DrawableBase() = default;
				};

				std::vector<std::unique_ptr<IWidget>> children_;
			};

			// Window（Containerを継承）
			class Window : public Container {
			public:
				explicit Window(std::string title) : title_(std::move(title)) {}

				void Draw(UiContext &ctx) {
					if (ImGui::Begin(title_.c_str())) {
						Container::Draw(ctx);
					}
					ImGui::End();
				}

			private:
				std::string title_;
			};

			// Button（Leaf）
			class Button {
			public:
				Button(std::string label, std::function<void()> onClick)
					: label_(std::move(label)), onClick_(std::move(onClick)) {
				}

				void Draw(UiContext &) {
					if (ImGui::Button(label_.c_str())) {
						if (onClick_) onClick_();
					}
				}

			private:
				std::string label_;
				std::function<void()> onClick_;
			};



		}

		/// @brief ウィジェットのベースとなるクラス
		class ImGuiWidgetBase {
		public:
			virtual ~ImGuiWidgetBase() = default;

			/// @brief ImGuiのウィジェットのラベルを取得する
			/// @return ラベル
			const std::string_view GetLabel() const { return label_; }

			/// @brief ImGuiのウィジェットのラベルを設定する
			/// @param[in] label ラベル
			void SetLabel(const std::string_view &label) { label_ = label; }

			/// @brief ImGuiのウィジェットを描画する
			/// @return 更新されたか
			virtual IsSuccess Draw() const = 0;

		private:

			virtual bool IsDestroyed() const { return false; }
			std::string label_;
		};

		/// @brief 複数のウィジェットをまとめて描画するクラス
		class ImGuiWidgetGroup {
		public:



		private:
			std::list<std::unique_ptr<ImGuiWidgetBase>> widgets_;
		};


		/// @brief ウィンドウ描画を行う
		class ImGuiWindow {
		public:


			IsSuccess Draw() const;

		private:

			std::list<std::unique_ptr<ImGuiWidgetBase>> widgets_;

			/// @brief ウィンドウのタイトル
			std::string title_;

		};

		class ComboBox : public ImGuiWidgetBase {
		public:

			IsSuccess Draw() const override;

		private:
			/// @brief コンボボックス自体のラベル
			std::string label_;
			/// @brief 内包しているアイテムのラベルのデータ
			std::string items_;
			/// @brief 選択中の添字
			uint32_t index_ = 0u;

		};

		struct Button
		{
			std::string label_;
			std::string id_;
			std::function< auto () -> void > onClick_;
			ImVec2 size_;
			auto operator()() const
			{
				if (
					ImGui::Button
					((id_.empty() ? label_ : label_ + "###" + id_).c_str(), size_)
					)
					onClick_();
			}
		};



		template<IsRealType T>
		IsSuccess ImGuiWidget(const T *const value);

		/// @brief 文字列の表示
		/// @param[in] value 文字列
		/// @return 成功したか
		IsSuccess ImGuiWidget(const char *const value);

		IsSuccess ImGuiWidget(const std::string_view &value);

		template<IsRealType T>
		IsSuccess ImGuiInput(const std::string_view &label, T *const value);

		template<IsRealType T>
		IsSuccess ImGuiWidget(const std::string_view &label, const T *const value);

		template<IsRealType T, IsContainsType<T> C, SoLib::IsIteratorOf<T> Itr, IsFunction<std::string, T> Func>
		Itr ImGuiWidget(const std::string_view &label, C *const, Itr &itr, const Func &func = SoLib::to_string);

		template<IsRealType T, IsContainsType<T> C, IsIteratorOf<T> Itr, IsFunction<const std::string_view, T> Func>
		Itr ImGuiWidget(const std::string_view &label, C *const, Itr &itr, const Func &func);


	}
}

#pragma region inline関数の記述


namespace SoLib::NewImGui {

	template<IsRealType T, IsContainsType<T> C, SoLib::IsIteratorOf<T> Itr, IsFunction<std::string, T> Func>
	inline Itr ImGuiWidget(const std::string_view &label, C *const container, Itr &itr, const Func &func)
	{

		// イテレータが無効であった場合に初期化する
		if (itr == container->end()) {
			// 初期化
			itr = container->begin();
		}
		// 表示文字列
		std::string previewName;

		// コンテナが空でなければ
		if (container->size()) {
			// 文字列を代入
			previewName = SoLib::to_string(*itr);
		}

		if (ImGui::BeginCombo(label.data(), previewName.c_str())) {
			for (Itr i = container->begin(); i != container->end(); ++i) {

				// アイテムの文字列
				std::string itemName = SoLib::to_string(*itr);

				// もし空文字列なら表示しない
				if (itemName != "") {

					if (ImGui::Selectable(itemName.c_str())) {
						itr = i;
						break;	// 選択されたらループを抜ける
					}
				}
			}
			ImGui::EndCombo();
		}

		return itr;
	}

}

#pragma endregion inline関数の記述