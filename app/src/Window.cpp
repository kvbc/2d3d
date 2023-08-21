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
        Rectangle sourceRect = {0, 0, m_renderTexture.texture.width, m_renderTexture.texture.height};
        rlImGuiImageRect(&m_renderTexture.texture, size.x, size.y, sourceRect);
    }

}