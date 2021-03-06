// Copyright Reality Engine. All Rights Reserved.

#pragma once

#include <span>

#include "Core/Platform.h"
#include "Core/Maths/Frustrum.h"
#include "Core/Maths/Vector3.h"

namespace Reality {
	class RE_CORE GLContext final {
	public:
		using ProcAddr = void(*)(const char*);

		static constexpr auto s_MaxLightsSize{ 100u };

		static void Init(ProcAddr proc);
		static void DrawSkybox(const class GLCubeMap& skybox);
		static void SetLights(std::span<struct GLLight> lights);
		static void SetModelMatrix(const struct Matrix4& model);
		static void SetViewMatrix(const Matrix4& view);
		static void SetProjectionMatrix(const Matrix4& projection);
		static void SetShadowMatrix(Vector3 lightDirection, Vector3 viewPosition = {}, const Frustrum& frustrum = {});

	private:
		inline static auto m_MatricesLocation{ 0u };
		inline static auto m_LightsLocation{ 0u };

		static void DebugOutput(unsigned source, unsigned type, unsigned id, unsigned severity, int, 
			const char* message, const void*);
	};
}