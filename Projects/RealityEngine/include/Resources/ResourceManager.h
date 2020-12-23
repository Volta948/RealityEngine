// Copyright Reality Engine. All Rights Reserved.

#pragma once

#include "ResourceHandler.h"

namespace reality {
	struct RE_CORE ResourceManager final {
		std::unordered_map<std::string, GLMesh> Meshes;
		std::unordered_map<std::string, GLMaterial> Materials;
		ResourceHandler<GLModel, ModelSettings, GLMeshSettings, Model> Models;
		ResourceHandler<GLCubeMap, SkyboxSettings, GLCubeMapSettings, Skybox> Skyboxes;
		ResourceHandler<GLTexture, TextureSettings, GLTextureSettings, Texture> Textures;
		ResourceHandler<GLShader, ShaderSettings, GLShaderSettings, Shader> Shaders;
		ResourceHandler<GLFont, FontSettings, GLFontSettings, Font> Fonts;

		ResourceManager();
		ResourceManager(ResourceManager&&) = delete;
		ResourceManager& operator=(ResourceManager&&) = delete;

		void Update();
	};

	RE_CORE extern ResourceManager* g_ResourceManager;
}

inline reality::ResourceManager::ResourceManager() :
	Textures{ "Resources/Textures.json" }, Models{ "Resources/Models.json" }, Skyboxes{ "Resources/Skyboxes.json" },
	Shaders{ "Resources/Shaders.json" }, Fonts{ "Resources/Fonts.json" }
{}

inline void reality::ResourceManager::Update() {
	Models.Update();
	Textures.Update();
	Fonts.Update();
	Shaders.Update();
	Skyboxes.Update();
}