// Copyright Reality Engine. All Rights Reserved.

#include "Editor.h"

#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>

#include "Gameplay/ComponentSystem.h"

reality::Editor::Editor() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	editorTheme::RealityStyle();
	auto& io{ ImGui::GetIO() };
	io.Fonts->AddFontFromFileTTF("../../Projects/RealityEditor/Config/LucidaGrande.ttf", 12.f);
	io.IniFilename = "Config/imgui.ini";
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	ImGui_ImplGlfw_InitForOpenGL(reinterpret_cast<GLFWwindow*>(g_Io->Window->GetHandle()), true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
	g_Logger->Callback = [this](auto msg) { m_Log.AddLog(msg); };
	g_Io->Window->SetDropCallback([this](auto count, auto paths) {
		for (auto i{ 0 }; i < count; ++i) {
			m_Assets.DropResource(paths[i]);
		}
	});
}

reality::Editor::~Editor() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void reality::Editor::Run() {
	auto& activeScene{ *g_SceneManager->ActiveScene };
	auto& camera{ activeScene.FindGameObject("Camera")->Transform };

	while (g_Io->Window->IsRunning() && !g_Io->Input->GetKeyDown(keycode::RE_KEY_ESCAPE)) {
		UpdateIo();

		m_Engine.Update();

		camera.SetPosition(m_Camera.Position);
		activeScene.Update();
		m_ComponentSystem.UpdateTransforms(activeScene);
		m_ComponentSystem.UpdateCameras(activeScene, m_Viewport.Size);
		m_ComponentSystem.UpdateLights(activeScene);

		Update();

		Render(m_Camera.GetViewMatrix());

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		g_Io->Update();
	}
}

void reality::Editor::Render(const Matrix4& view) const {
	GLContext::SetViewMatrix(view);

	m_Pipeline.BeginShadowPass();
	m_ComponentSystem.UpdateMeshesShadow(*g_SceneManager->ActiveScene);

	m_Pipeline.BeginScenePass(g_ResourceManager->Skyboxes.Get("Saturne"));
	m_ComponentSystem.UpdateMeshes(*g_SceneManager->ActiveScene);
	m_ComponentSystem.UpdateParticles(*g_SceneManager->ActiveScene);

	m_Pipeline.BeginPostProcess();
	m_Pipeline.GetDefaultPass().Bind();
}

void reality::Editor::Update() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
	UpdateWindows();
}

void reality::Editor::UpdateWindows() {
	ImGui::SliderFloat("Camera Speed", &m_Camera.MovementSpeed, 0.1f, 50.f);
	m_Dock.Begin();
	m_Assets.Draw();
	m_Log.Draw();
	m_Menu.Draw();
	m_Hierarchy.Draw(*g_SceneManager->ActiveScene);
	m_Inspector.Draw(m_Hierarchy.GetSelected());
	m_Scene.Draw(m_Pipeline, m_Camera, m_Viewport, m_Hierarchy.GetSelected());
	m_Dock.End();
}

void reality::Editor::UpdateIo() {
	if (ImGui::IsKeyDown(keycode::RE_KEY_F) && !m_Hierarchy.GetSelected().empty()) {
		m_Camera.Focus(m_Hierarchy.GetSelected().back());
	}
	m_Camera.UpdateFocus();

	if (ImGui::IsMouseDown(keycode::RE_MOUSE_BUTTON_RIGHT)) {
		if (!m_EnabledCamera && m_Scene.IsHovered()) {
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
			g_Io->Window->HideAndLockCursor();
			m_EnabledCamera = true;
		}
		if (m_EnabledCamera) {
			m_Camera.Update();
		}
	}
	else if (m_EnabledCamera) {
		ImGui::GetIO().ConfigFlags ^= ImGuiConfigFlags_NoMouse;
		g_Io->Window->ShowCursor();
		m_EnabledCamera = false;
	}
}