// Copyright Reality Engine. All Rights Reserved.

#pragma once

#include <imgui/imgui.h>
#include <filesystem>

#include "Resources/ResourceManager.h"

namespace reality {
	class EditorAssets {
	public:
		void Draw();
		void DropResource(std::string_view path) const;
	};
}

inline void reality::EditorAssets::Draw() {
	ImGui::Begin("Assets");
	{

	}
	ImGui::End();
}

inline void reality::EditorAssets::DropResource(std::string_view filename) const {
	std::filesystem::path path{ filename };
	auto extension{ path.extension().string() };
	if (Model::IsExtensionSupported(extension)) {
		g_ResourceManager->Models.Load(path.stem().string().c_str(), { filename.data() });
	}
	else if (Texture::IsExtensionSupported(filename)) {
		g_ResourceManager->Textures.Load(path.stem().string().c_str(), { filename.data() });
	}
	else if (Font::IsExtensionSupported(extension)) {
		g_ResourceManager->Fonts.Load(path.stem().string().c_str(), { filename.data() });
	}
}