/// @file ImGuiRegistry.hpp
/// @brief ImGuiの状態管理
/// @author ONUKI seiya
#pragma once

#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <charconv>
#include <algorithm>
#include "../Success.h"

/// @brief Immediate Mode UIの一時性を補う、UI専用状態のレジストリ。
/// @details
/// - 目的: Window開閉・Splitter比率・選択インデックス等をIDキーで集中管理し、永続化可能にする。
/// - 保存形式: 独自INIライク (例)
///     [OpenFlags]
///     MainWindow=true
///     [SplitterRatios]
///     MainSplitter=0.33
///     [SelectedIndex]
///     AssetList=2
/// 
/// 命名規約: 関数=ビッグキャメルケース、変数=ローワーキャメルケース末尾に '_'。
class UiStateRegistry {
public:
	/// @brief Windowなどの開閉フラグを取得（無ければ作成）。
	bool &GetOpenFlag(const std::string &id) {
		return openFlags_[id];
	}

	/// @brief Splitter等の比率を取得(無ければdefaultRatioで作成、0.0〜1.0にクランプ)
	/// @param[in] id 検索するID
	/// @param[in] defaultRaito 存在しなかった場合の値
	float &GetSplitterRatio(const std::string &id, float defaultRatio = 0.5f) {
		auto it = splitterRatios_.find(id);
		if (it == splitterRatios_.end()) {
			float v = std::clamp(defaultRatio, 0.0f, 1.0f);
			it = splitterRatios_.emplace(id, v).first;
		}
		return it->second;
	}

	/// @brief リスト等の選択インデックスを取得（無ければ-1で作成）。
	int32_t &GetSelectedIndex(const std::string &id, int32_t defaultIndex = -1) {
		auto it = selectedIndex_.find(id);
		if (it == selectedIndex_.end()) {
			it = selectedIndex_.emplace(id, defaultIndex).first;
		}
		return it->second;
	}

	/// @brief 一時的な文字列バッファ（非永続）を取得（無ければ空文字）。
	std::string &GetTransientTextBuffer(const std::string &id) {
		return transientTextBuffers_[id];
	}

	/// @brief 値の明示設定（Set系は必要に応じて使用）。
	void SetOpenFlag(const std::string &id, bool value) { openFlags_[id] = value; }
	void SetSplitterRatio(const std::string &id, float value) { splitterRatios_[id] = std::clamp(value, 0.0f, 1.0f); }
	void SetSelectedIndex(const std::string &id, int32_t value) { selectedIndex_[id] = value; }

	// @brief ファイルへ保存（テキストINI風）
	// @param path 保存先パス
	// @return 成否
	bool SerializeToFile(const std::filesystem::path &path) const {
		std::error_code ec;
		if (path.has_parent_path()) {
			std::filesystem::create_directories(path.parent_path(), ec);
		}
		std::ofstream ofs(path, std::ios::binary);
		if (not ofs) { return false; }

		ofs << "# UiStateRegistry v1\n";

		// bool（OpenFlags）
		ofs << "\n[OpenFlags]\n";
		for (const auto &[k, v] : openFlags_) {
			ofs << EscapeKey(k) << "=" << (v ? "true" : "false") << "\n";
		}

		// float（SplitterRatios）
		ofs << "\n[SplitterRatios]\n";
		for (const auto &[k, v] : splitterRatios_) {
			ofs << EscapeKey(k) << "=" << v << "\n";
		}

		// int32_t（SelectedIndex）
		ofs << "\n[SelectedIndex]\n";
		for (const auto &[k, v] : selectedIndex_) {
			ofs << EscapeKey(k) << "=" << v << "\n";
		}

		// transientTextBuffers_ は**非永続**のため保存しない

		ofs.flush();
		return static_cast<bool>(ofs);
	}

	/// @brief ファイルから読み込み（テキストINI風）
	/// @param path 読み込み元パス
	/// @return 成否
	IsSuccess DeserializeFromFile(const std::filesystem::path &path) {
		std::ifstream ifs(path, std::ios::binary);
		if (not ifs) { return false; }

		// 読み込み時は既存にマージする（未知キーは追加／既知キーは上書き）
		enum class Section { kNone, kOpen, kSplit, kSelect };
		Section section = Section::kNone;

		std::string line;
		while (std::getline(ifs, line)) {
			TrimInPlace(line);
			if (line.empty() or line[0] == '#' or line[0] == ';') { continue; }

			if (line.front() == '[' and line.back() == ']') {
				auto name = line.substr(1, line.size() - 2);
				if (EqualNoCase(name, "OpenFlags")) section = Section::kOpen;
				else if (EqualNoCase(name, "SplitterRatios")) section = Section::kSplit;
				else if (EqualNoCase(name, "SelectedIndex")) section = Section::kSelect;
				else section = Section::kNone;
				continue;
			}

			auto kv = SplitKeyValue(line);
			if (not kv) { continue; }
			const auto &key = UnescapeKey(kv->first);
			const auto &val = kv->second;

			switch (section) {
			case Section::kOpen:
				auto b = ParseBool(val);
				if (b.has_value()) { openFlags_[key] = b.value(); }
				break;
			case Section::kSplit:
				auto f = ParseFloat(val);
				if (f.has_value()) { splitterRatios_[key] = std::clamp(f.value(), 0.0f, 1.0f); }
				break;
			case Section::kSelect:
				auto i = ParseInt(val);
				if (i.has_value()) { selectedIndex_[key] = i.value(); }
				break;
			default:
				break;
			}
		}
		return true;
	}

	/// @brief 既存データをクリア（必要に応じて使用）
	void Clear() {
		openFlags_.clear();
		splitterRatios_.clear();
		selectedIndex_.clear();
		transientTextBuffers_.clear();
	}

private:
	// ---- 永続対象 ----
	std::unordered_map<std::string, bool> openFlags_;        ///< ウィンドウ/パネル開閉
	std::unordered_map<std::string, float> splitterRatios_;  ///< Splitter比率(0..1)
	std::unordered_map<std::string, int32_t> selectedIndex_;     ///< 選択中インデックス

	// ---- 非永続（フレーム間保持に使うバッファ等）----
	std::unordered_map<std::string, std::string> transientTextBuffers_;

	// ---- ユーティリティ群 ----

	static std::optional<std::pair<std::string, std::string>> SplitKeyValue(const std::string &line) {
		auto eq = line.find('=');
		if (eq == std::string::npos) { return std::nullopt; }
		std::string k = line.substr(0, eq);
		std::string v = line.substr(eq + 1);
		TrimInPlace(k);
		TrimInPlace(v);
		return std::make_pair(k, v);
	}

	static bool EqualNoCase(std::string_view a, std::string_view b) {
		if (a.size() != b.size()) { return false; }
		for (size_t i = 0; i < a.size(); ++i) {
			if (ToLower(a[i]) != ToLower(b[i])) { return false; }
		}
		return true;
	}

	static char ToLower(char c) {
		if (c >= 'A' and c <= 'Z') { return static_cast<char>(c - 'A' + 'a'); }
		return c;
	}

	static void TrimInPlace(std::string &s) {
		auto notSpace = [](unsigned char c) { return !std::isspace(c); };
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
		s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
	}

	static std::optional<bool> ParseBool(std::string_view s) {
		if (EqualNoCase(s, "true") or EqualNoCase(s, "1")) {
			return true;
		}
		if (EqualNoCase(s, "false") or EqualNoCase(s, "0")) {
			return false;
		}
		return std::nullopt;
	}

	static std::optional<float> ParseFloat(std::string_view s) {
		float out{};
		// std::from_chars for float is C++17+, but some libs only for int32_tegral; use std::stof fallback
		try {
			size_t idx = 0;
			out = std::stof(std::string(s), &idx);
			if (idx == s.size()) return out;
		}
		catch (...) {}
		return std::nullopt;
	}

	static std::optional<int32_t> ParseInt(std::string_view s) {
		int32_t out{};
		auto first = s.data();
		auto last = s.data() + s.size();
		if (auto [p, ec] = std::from_chars(first, last, out); ec == std::errc{} and p == last) {
			return out;
		}
		return std::nullopt;
	}

	/// @brief キーの簡易エスケープ（'=' と 空白 と '[' ']' を避ける）
	static std::string EscapeKey(const std::string &key) {
		std::string r; r.reserve(key.size());
		for (char c : key) {
			switch (c) {
			case '=': r += "\\="; break;
			case '[': r += "\\["; break;
			case ']': r += "\\]"; break;
			case ' ': r += "\\s"; break;
			case '\\': r += "\\\\"; break;
			default: r += c; break;
			}
		}
		return r;
	}

	/// @brief エスケープ解除
	static std::string UnescapeKey(const std::string &key) {
		std::string r;
		r.reserve(key.size());
		for (size_t i = 0; i < key.size(); ++i) {
			char c = key[i];
			if (c == '\\' and i + 1 < key.size()) {
				char n = key[i + 1];
				if (n == '=' or n == '[' or n == ']' or n == '\\') {
					r += n; ++i;
					continue;
				}
				if (n == 's') {
					r += ' '; ++i;
					continue;
				}
			}
			r += c;
		}
		return r;
	}
};

