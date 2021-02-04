// Copyright Reality Engine. All Rights Reserved.

#pragma once

#include <vector>
#include <span>

#include "Core/Platform.h"
#include "Core/Maths/MathsSerializer.h"
#include "Component.h"

namespace reality {
	struct RE_CORE CTransform : Component {
		CTransform() = default;
		CTransform(const CTransform&);
		CTransform& operator=(const CTransform&);
		~CTransform();

		Vector3 TransformPoint(const Vector3& position) const;
		Vector3 InverseTransformPoint(const Vector3& position) const;
		void Rotate(const Vector3& eulerAngles);
		void Translate(const Vector3& translation);
		bool IsRoot() const;
		bool HasChanged() const;
		CTransform* GetRoot();
		CTransform* GetParent() const;
		std::span<CTransform*> GetChildren();
		CTransform* GetChild(unsigned index) const;
		unsigned GetChildrenSize() const;
		unsigned GetLevel() const;
		uint64 GetParentId() const;
		const Vector3& GetPosition() const;
		const Quaternion& GetRotation() const;
		const Vector3& GetScale() const;
		Quaternion GetLocalRotation() const;
		Vector3 GetLocalPosition() const;
		Matrix4 GetWorldToLocalMatrix() const;
		const Matrix4& GetTrs() const;
		Vector3 GetRight() const;
		Vector3 GetUp() const;
		Vector3 GetForward() const;
		void SetHasChanged(bool hasChanged);
		void SetParent(CTransform* parent);
		void SetPosition(Vector3 position);
		void SetRotation(Vector3 eulerAngles);
		void SetRotation(Quaternion rotation);
		void SetScale(Vector3 scale);
		void SetTrs(const Matrix4& trs);

	private:
		Matrix4 m_Trs{ Matrix4::Identity };
		Quaternion m_Rotation;
		Vector3 m_Position;
		Vector3 m_Scale{ Vector3::One };
		std::vector<CTransform*> m_Children;
		CTransform* m_Parent{};
		uint64 m_ParentId{};
		unsigned m_Level{};
		bool m_HasChanged{};

	public:
		virtual Component* Clone() const override;
		void Reset();
		static std::shared_ptr<CTransform> Instantiate();

	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive);
	};
}

inline reality::CTransform::CTransform(const CTransform& other) :
	m_Trs{ other.m_Trs }, m_Rotation{ other.m_Rotation }, m_Position{ other.m_Position }, m_Scale{ other.m_Scale }
{
	SetHasChanged(true);
	SetParent(other.m_Parent);
}

inline reality::CTransform& reality::CTransform::operator=(const CTransform& other) {
	SetHasChanged(true);
	m_Trs = other.m_Trs;
	m_Rotation = other.m_Rotation;
	m_Position = other.m_Position;
	m_Scale = other.m_Scale;
	SetParent(other.m_Parent);
	return *this;
}

inline reality::CTransform::~CTransform() {
	if (m_Parent && !m_Parent->m_Children.empty()) {
		std::erase(m_Parent->m_Children, this);
	}
}

inline reality::Vector3 reality::CTransform::TransformPoint(const Vector3& position) const {
	return {
		m_Trs[0] * position.X + m_Trs[4] * position.Y + m_Trs[8] * position.Z + m_Trs[12],
		m_Trs[1] * position.X + m_Trs[5] * position.Y + m_Trs[9] * position.Z + m_Trs[13],
		m_Trs[2] * position.X + m_Trs[6] * position.Y + m_Trs[10] * position.Z + m_Trs[14]
	};
}

inline reality::Vector3 reality::CTransform::InverseTransformPoint(const Vector3& position) const {
	const auto pos{ position - m_Position };
	return {
		m_Trs[0] * pos.X + m_Trs[1] * pos.Y + m_Trs[2] * pos.Z + m_Trs[4],
		m_Trs[4] * pos.X + m_Trs[5] * pos.Y + m_Trs[6] * pos.Z + m_Trs[7],
		m_Trs[8] * pos.X + m_Trs[9] * pos.Y + m_Trs[10] * pos.Z + m_Trs[11]
	};
}

inline void reality::CTransform::Rotate(const Vector3& eulerAngles) {
	m_Rotation = Quaternion::Normalize(m_Rotation * Quaternion{ eulerAngles * Mathf::Deg2Rad });
	SetHasChanged(true);
}

inline void reality::CTransform::Translate(const Vector3& translation) {
	m_Position += translation;
	SetHasChanged(true);
}

inline bool reality::CTransform::IsRoot() const {
	return !m_Level;
}

inline bool reality::CTransform::HasChanged() const {
	return m_Parent ? m_Parent->HasChanged() : m_HasChanged;
}

inline reality::CTransform* reality::CTransform::GetRoot() {
	return m_Parent ? m_Parent->GetRoot() : this;
}

inline reality::CTransform* reality::CTransform::GetParent() const {
	return m_Parent;
}

inline std::span<reality::CTransform*> reality::CTransform::GetChildren() {
	return m_Children;
}

inline reality::CTransform* reality::CTransform::GetChild(unsigned index) const {
	return index < m_Children.size() ? m_Children[index] : nullptr;
}

inline unsigned reality::CTransform::GetChildrenSize() const {
	return (unsigned)m_Children.size();
}

inline unsigned reality::CTransform::GetLevel() const {
	return m_Level;
}

inline reality::uint64 reality::CTransform::GetParentId() const {
	return m_ParentId;
}

inline const reality::Vector3& reality::CTransform::GetPosition() const {
	return m_Position;
}

inline const reality::Quaternion& reality::CTransform::GetRotation() const {
	return m_Rotation;
}

inline const reality::Vector3& reality::CTransform::GetScale() const {
	return m_Scale;
}

inline reality::Quaternion reality::CTransform::GetLocalRotation() const {
	return m_Parent ? Quaternion::Inverse(m_Parent->m_Rotation * m_Rotation) : m_Rotation;
}

inline reality::Vector3 reality::CTransform::GetLocalPosition() const {
	return m_Parent ? m_Parent->TransformPoint(m_Position) : m_Position;
}

inline reality::Matrix4 reality::CTransform::GetWorldToLocalMatrix() const {
	return Matrix4::Inverse(m_Trs);
}

inline const reality::Matrix4& reality::CTransform::GetTrs() const {
	return m_Trs;
}

inline reality::Vector3 reality::CTransform::GetRight() const {
	return m_Trs.GetRow3(0);
}

inline reality::Vector3 reality::CTransform::GetUp() const {
	return m_Trs.GetRow3(1);
}

inline reality::Vector3 reality::CTransform::GetForward() const {
	return m_Trs.GetRow3(2);
}

inline void reality::CTransform::SetHasChanged(bool hasChanged) {
	m_HasChanged = hasChanged;
	if (m_Parent && hasChanged) {
		m_Parent->SetHasChanged(m_HasChanged);
	}
}

inline void reality::CTransform::SetPosition(Vector3 position) {
	m_Position = std::move(position);
	SetHasChanged(true);
}

inline void reality::CTransform::SetRotation(Vector3 eulerAngles) {
	m_Rotation = Quaternion{ eulerAngles * Mathf::Deg2Rad };
	SetHasChanged(true);
}

inline void reality::CTransform::SetRotation(Quaternion rotation) {
	m_Rotation = std::move(rotation);
	SetHasChanged(true);
}

inline void reality::CTransform::SetScale(Vector3 scale) {
	m_Scale = std::move(scale);
	SetHasChanged(true);
}

inline void reality::CTransform::SetTrs(const Matrix4& trs) {
	m_Trs = trs;
	SetHasChanged(true);
}

inline reality::Component* reality::CTransform::Clone() const {
	return new CTransform{ *this };
}

inline void reality::CTransform::Reset() {
	auto owner{ m_GameObject };
	*this = CTransform{};
	m_GameObject = owner;
}

inline std::shared_ptr<reality::CTransform> reality::CTransform::Instantiate() {
	return std::make_shared<CTransform>();
}

template <class Archive>
void reality::CTransform::serialize(Archive& archive) {
	archive(CEREAL_NVP(m_Trs));
	archive(CEREAL_NVP(m_Rotation));
	archive(CEREAL_NVP(m_Position));
	archive(CEREAL_NVP(m_Scale));
	archive(CEREAL_NVP(m_Level));
	archive(CEREAL_NVP(m_HasChanged));
	archive(CEREAL_NVP(m_ParentId));
}

CEREAL_REGISTER_TYPE_WITH_NAME(reality::CTransform, "Transform");
CEREAL_REGISTER_POLYMORPHIC_RELATION(reality::Component, reality::CTransform)