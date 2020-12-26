// Copyright Reality Engine. All Rights Reserved.

#pragma once

#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

#include "Core/Tools/Randomizer.h"
#include "Component/Transform.h"
#include "ComponentManager.h"

namespace reality {
	class GameObject final {
	public:
		friend class Scene;

		std::string	Name;
		CTransform Transform;
		bool IsActive{ true };

		explicit GameObject(std::string_view name = {}, class Scene* scene = {}, ComponentManager* manager = {});
		GameObject(const GameObject&);
		GameObject& operator=(const GameObject&);
		~GameObject();

		void RemoveAllComponents();
		std::vector<std::unique_ptr<Component>>& GetAllComponents();
		Scene* GetScene() const;
		uint64 GetId() const;
		void SetParent(GameObject& parent);
		template <class T>
		T* AddComponent();
		void AddComponent(const Component* component);
		template <class T>
		bool HasComponent() const;
		template <class T>
		T* GetComponent() const;
		template <class T>
		T* GetComponentInParent() const;
		template <class T>
		T* GetComponentInChildren() const;
		template <class T>
		std::vector<T*> GetComponents() const;
		template <class T>
		std::vector<T*> GetComponentsInParent() const;
		template <class T>
		std::vector<T*> GetComponentsInChildren() const;
		template <class T>
		void RemoveComponent();
		template <class T>
		void RemoveComponents();

	private:
		std::vector<std::unique_ptr<Component>> m_Components;
		ComponentManager* m_Manager{};
		Scene* m_Scene{};
		uint64 m_Id{};

		friend class cereal::access;
		template <class Archive>
		void load(Archive& archive) {
			archive(CEREAL_NVP(Name));
			archive(CEREAL_NVP(m_Id));
			archive(CEREAL_NVP(IsActive));
			archive(CEREAL_NVP(Transform));
			Transform.m_GameObject = this;
			archive(CEREAL_NVP(m_Components));
		}

		template <class Archive>
		void save(Archive& archive) const {
			archive(CEREAL_NVP(Name));
			archive(CEREAL_NVP(m_Id));
			archive(CEREAL_NVP(IsActive));
			archive(CEREAL_NVP(Transform));
			archive(CEREAL_NVP(m_Components));
		}
	};
}

inline reality::GameObject::GameObject(std::string_view name, Scene* scene, ComponentManager* manager) :
	Name{ name }, m_Manager{ manager }, m_Scene{ scene }, 
	m_Id{ g_Randomizer->GetInt(std::numeric_limits<uint64>::min(), std::numeric_limits<uint64>::max()) }
{
	Transform.m_GameObject = this;
}

inline reality::GameObject::GameObject(const GameObject& other) :
	Transform{ other.Transform }, Name{ other.Name }, IsActive{ other.IsActive },
	m_Manager{ other.m_Manager }, m_Scene{ other.m_Scene }, 
	m_Id{ g_Randomizer->GetInt(std::numeric_limits<uint64>::min(), std::numeric_limits<uint64>::max()) }
{
	for (const auto& component : other.m_Components) {
		auto& comp{ m_Components.emplace_back(component->Clone()) };
		comp->m_GameObject = this;
		if (m_Manager) {
			m_Manager->AddComponent(comp.get());
		}
	}
	Transform.m_GameObject = this;
}

inline reality::GameObject& reality::GameObject::operator=(const GameObject& other) {
	Transform = other.Transform;
	Name = other.Name;
	IsActive = other.IsActive;
	m_Manager = other.m_Manager;
	m_Scene = other.m_Scene;

	for (const auto& component : other.m_Components) {
		auto& comp{ m_Components.emplace_back(component->Clone()) };
		comp->m_GameObject = this;
		if (m_Manager) {
			m_Manager->AddComponent(comp.get());
		}
	}
	Transform.m_GameObject = this;
	return *this;
}

inline reality::GameObject::~GameObject() {
	RemoveAllComponents();
}

inline void reality::GameObject::RemoveAllComponents() {
	if (m_Manager) {
		for (const auto& component : m_Components) {
			m_Manager->RemoveComponent(component.get());
		}
	}
	m_Components.clear();
}

inline std::vector<std::unique_ptr<reality::Component>>& reality::GameObject::GetAllComponents() {
	return m_Components;
}

inline reality::Scene* reality::GameObject::GetScene() const {
	return m_Scene;
}

inline reality::uint64 reality::GameObject::GetId() const {
	return m_Id;
}

inline void reality::GameObject::SetParent(GameObject& parent) {
	Transform.SetParent(&parent.Transform);
}

template <class T>
T* reality::GameObject::AddComponent() {
	static_assert(std::is_base_of_v<Component, T>);

	if (HasComponent<T>()) {
		return nullptr;
	}

	auto component{ m_Components.emplace_back(std::make_unique<T>()).get() };
	component->m_GameObject = this;

	if (m_Manager) {
		m_Manager->AddComponent(component);
	}
	return static_cast<T*>(component);
}

inline void reality::GameObject::AddComponent(const Component* component) {
	if (std::any_of(m_Components.cbegin(), m_Components.cend(), 
		[component](auto& comp) { return rttr::type::get(*component) == rttr::type::get(*comp); })) 
	{
		return;
	}

	auto comp{ m_Components.emplace_back(component->Clone()).get() };
	comp->m_GameObject = this;

	if (m_Manager) {
		m_Manager->AddComponent(comp);
	}
}

template <class T>
bool reality::GameObject::HasComponent() const {
	return std::any_of(m_Components.cbegin(), m_Components.cend(), [](auto& comp) {
		return rttr::type::get<T>() == rttr::type::get(*comp);
	});
}

template <class T>
T* reality::GameObject::GetComponent() const {
	static_assert(std::is_base_of_v<Component, T>);

	for (const auto& component : m_Components) {
		if (auto comp{ rttr::rttr_cast<T*>(component.get()) }) {
			return comp;
		}
	}
	return nullptr;
}

template <class T>
T* reality::GameObject::GetComponentInParent() const {
	static_assert(std::is_base_of_v<Component, T>);

	return !Transform.GetParent() ? GetComponent() : Transform.GetParent()->GetGameObject().GetComponent();
}

template <class T>
T* reality::GameObject::GetComponentInChildren() const {
	static_assert(std::is_base_of_v<Component, T>);

	for (const auto& child : Transform.GetChildren()) {
		if (auto component{ child->GetGameObject().GetComponent() }) {
			return component;
		}
	}
	return GetComponent();
}

template <class T>
std::vector<T*> reality::GameObject::GetComponents() const {
	static_assert(std::is_base_of_v<Component, T>);

	std::vector<T*> components;
	for (const auto& component : m_Components) {
		if (auto comp{ rttr::rttr_cast<T*>(component.get()) }) {
			components.emplace_back(comp);
		}
	}
	return components;
}

template <class T>
std::vector<T*> reality::GameObject::GetComponentsInParent() const {
	static_assert(std::is_base_of_v<Component, T>);

	return !Transform.GetParent() ? GetComponents() : Transform.GetParent()->GetGameObject().GetComponents();
}

template <class T>
std::vector<T*> reality::GameObject::GetComponentsInChildren() const {
	static_assert(std::is_base_of_v<Component, T>);

	if (!Transform.GetChildrenSize()) {
		return GetComponents();
	}

	std::vector<T*> components;
	for (const auto& child : Transform.GetChildren()) {
		for (const auto& component : child->GetGameObject().m_Components) {
			if (auto comp{ rttr::rttr_cast<T*>(component.get()) }) {
				components.emplace_back(comp);
			}
		}
	}
	return components;
}

template <class T>
void reality::GameObject::RemoveComponent() {
	static_assert(std::is_base_of_v<Component, T>);

	for (auto it{ m_Components.cbegin() }; it != m_Components.cend(); ++it) {
		if (auto comp{ rttr::rttr_cast<T*>(it->get()) }) {
			if (m_Manager) {
				m_Manager->RemoveComponent(comp);
			}
			m_Components.erase(it);
			return;
		}
	}
}

template <class T>
void reality::GameObject::RemoveComponents() {
	static_assert(std::is_base_of_v<Component, T>);

	for (auto it{ m_Components.cbegin() }; it != m_Components.cend(); ) {
		if (auto comp{ rttr::rttr_cast<T*>(it->get()) }) {
			if (m_Manager) {
				m_Manager->RemoveComponent(comp);
			}
			it = m_Components.erase(it);
		}
		else {
			++it;
		}
	}
}