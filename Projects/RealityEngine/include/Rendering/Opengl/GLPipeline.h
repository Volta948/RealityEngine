// Copyright Reality Engine. All Rights Reserved.

#pragma once

#include "Core/Platform.h"
#include "GLRenderPass.h"

namespace Reality {
	class RE_CORE GLPipeline {
	public:
		explicit GLPipeline(const Viewport& viewport = {});

		void ResizePasses(const Viewport& viewport);
		void BeginShadowPass() const;
		void BeginScenePass(const class GLCubeMap* skybox = {}) const;
		void BeginPostProcess() const;
		const GLRenderPass& GetDefaultPass() const;
		const GLRenderPass& GetFinalPass() const;

	private:
		GLRenderPass m_ShadowPass, m_ScenePass, m_MidPass, m_BlurPasses[2], m_DownPass, m_LensPass, m_FinalPass, m_DefaultPass;
	};
}

inline void Reality::GLPipeline::ResizePasses(const Viewport& viewport) {
	m_ScenePass.Resize(viewport);
	m_MidPass.Resize(viewport);
	for (auto& blurPass : m_BlurPasses) {
		blurPass.Resize(viewport);
	}
	m_DownPass.Resize(viewport);
	m_LensPass.Resize(viewport);
	m_FinalPass.Resize(viewport);
	m_DefaultPass.Resize(viewport);
}

inline const Reality::GLRenderPass& Reality::GLPipeline::GetDefaultPass() const {
	return m_DefaultPass;
}

inline const Reality::GLRenderPass& Reality::GLPipeline::GetFinalPass() const {
	return m_FinalPass;
}