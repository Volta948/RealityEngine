// Copyright Reality Engine. All Rights Reserved.

#pragma once

#include <unordered_map>
#include <fstream>
#include <string>

#include "Core/Platform.h"

namespace Reality {
	class PlayerPref {
	public:
		explicit PlayerPref(std::string_view fileName = "PlayerPref.txt");
		~PlayerPref();

		void DeleteAll();
		void DeleteKey(std::string_view key);
		void Save() const;
		bool HasKey(std::string_view key) const;
		float GetFloat(std::string_view key) const;
		int GetInt(std::string_view key) const;
		std::string GetString(std::string_view key) const;
		void SetFloat(std::string_view key, float value);
		void SetInt(std::string_view key, int value);
		void SetString(std::string_view key, std::string value);

	private:
		std::string m_FileName;
		std::unordered_map<std::string, std::string> m_Values;
		bool m_HasChanged{};
	};

	RE_CORE extern PlayerPref* g_PlayerPref;
}

inline Reality::PlayerPref::PlayerPref(std::string_view fileName) :
	m_FileName{ fileName } 
{
	std::ifstream file{ m_FileName };
	std::string key, value;
	while (file >> key >> value) {
		m_Values.emplace(std::make_pair(key, value));
	}
}

inline Reality::PlayerPref::~PlayerPref() {
	Save();
}

inline void Reality::PlayerPref::DeleteAll() {
	std::ofstream{ m_FileName, std::ofstream::trunc };
	m_Values.clear();
	m_HasChanged = true;
}

inline void Reality::PlayerPref::DeleteKey(std::string_view key) {
	if (auto it{ m_Values.find(key.data()) }; it != m_Values.cend()) {
		m_Values.erase(it);
		m_HasChanged = true;
	}
}

inline void Reality::PlayerPref::Save() const {
	if (!m_HasChanged) {
		return;
	}

	std::ofstream file{ m_FileName, std::ofstream::trunc };
	for (const auto& [key, value] : m_Values) {
		file << key << ' ' << value << '\n';
	}
}

inline bool Reality::PlayerPref::HasKey(std::string_view key) const {
	return m_Values.find(key.data()) != m_Values.cend();
}

inline float Reality::PlayerPref::GetFloat(std::string_view key) const {
	return std::stof(m_Values.at(key.data()));
}

inline int Reality::PlayerPref::GetInt(std::string_view key) const {
	return std::stoi(m_Values.at(key.data()));
}

inline std::string Reality::PlayerPref::GetString(std::string_view key) const {
	return m_Values.at(key.data());
}

inline void Reality::PlayerPref::SetFloat(std::string_view key, float value) {
	m_Values[key.data()] = std::to_string(value);
	m_HasChanged = true;
}

inline void Reality::PlayerPref::SetInt(std::string_view key, int value) {
	m_Values[key.data()] = std::to_string(value);
	m_HasChanged = true;
}

inline void Reality::PlayerPref::SetString(std::string_view key, std::string value) {
	m_Values[key.data()] = std::move(value);
	m_HasChanged = true;
}