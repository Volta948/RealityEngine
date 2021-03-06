// Copyright Reality Engine. All Rights Reserved.

#pragma once

#include <string>
#include <cereal/types/string.hpp>

#include "Component.h"
#include "Resources/ResourceManager.h"

namespace Reality {
	struct CMeshRenderer : Component {
		RE_COMPONENT(CMeshRenderer, Component)

		std::string_view GetName() const;
		const GLModel* GetModel() const;
		void SetModel(std::string_view name);

	private:
		std::string m_Name;
		const struct GLModel* m_Model{};

	private:
		template <class Archive>
		void load(Archive& archive) {
			archive(CEREAL_NVP(m_Name));
			SetModel(m_Name);
		}

		template <class Archive>
		void save(Archive& archive) const {
			archive(CEREAL_NVP(m_Name));
		}
	};
}

inline std::string_view Reality::CMeshRenderer::GetName() const {
	return m_Name;
}

inline const Reality::GLModel* Reality::CMeshRenderer::GetModel() const {
	return m_Model;
}

inline void Reality::CMeshRenderer::SetModel(std::string_view name) {
	if (auto model{ g_ResourceManager->Models.Get(name) }) {
		m_Model = model;
		m_Name = name;
	}
}

CEREAL_REGISTER_TYPE_WITH_NAME(Reality::CMeshRenderer, "MeshRenderer");
CEREAL_REGISTER_POLYMORPHIC_RELATION(Reality::Component, Reality::CMeshRenderer)