#include "Window.hpp"
#include "imgui.h"
#include "rlImGui.h"

namespace App {

    // 
    // Protected
    // 

    Window::Window():
        m_redraw(false),
        m_renderTexture(LoadRenderTexture(GetScreenWidth(), GetScreenHeight()))
    {}

    Window::~Window() {
        UnloadRenderTexture(m_renderTexture);
    }

    void Window::redrawTexture() {
        m_redraw = true;
    }

    void Window::renderTexture() {
        if(m_redraw) {
            m_redraw = false;
            if(IsWindowResized()) {
                UnloadRenderTexture(m_renderTexture);
                m_renderTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
            }
            BeginTextureMode(m_renderTexture);
            drawTexture();
            EndTextureMode();
        }
        ImVec2 size = ImGui::GetContentRegionAvail();
        // opengl uses bottom-left coordinate system and raylib uses top-right,
        // flip the render texture on Y
        Rectangle sourceRect = {0, 0, (float)m_renderTexture.texture.width, -1.0f * m_renderTexture.texture.height};
        rlImGuiImageRect(&m_renderTexture.texture, size.x, size.y, sourceRect);
    }

}