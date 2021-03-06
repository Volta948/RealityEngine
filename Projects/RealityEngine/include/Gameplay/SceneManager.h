// Copyright Reality Engine. All Rights Reserved.

#pragma once

#include "Scene.h"

namespace Reality {
	class SceneManager final {
	public:
		Scene* ActiveScene{};

		Scene& CreateScene(std::string_view name);
		Scene* GetScene(std::string_view name);

	private:
		std::unordered_map<std::string, Scene>	m_Scenes;
	};

	RE_CORE extern SceneManager* g_SceneManager;
}

inline Reality::Scene& Reality::SceneManager::CreateScene(std::string_view name) {
	ActiveScene = &m_Scenes.emplace(name, Scene{ name }).first->second;
	return *ActiveScene;
}

inline Reality::Scene* Reality::SceneManager::GetScene(std::string_view name) {
	const auto it{ m_Scenes.find(name.data()) };
	return it != m_Scenes.cend() ? &it->second : nullptr;
}