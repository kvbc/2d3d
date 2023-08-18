#include "EditWindow.hpp"
#include <iostream>
#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"
#include "raymath.h"
#include "rlgl.h"
#include <algorithm>

namespace App {
    // 
    // Private
    // 

    Vector2 EditWindow::getWorldToGridPosition(Vector2 worldPosition) {
        worldPosition.x = floor(worldPosition.x / GRID_CELL_SIZE_PX);
        worldPosition.y = floor(worldPosition.y / GRID_CELL_SIZE_PX);
        return worldPosition;
    }

    Vector2 EditWindow::getGridToWorldPosition(Vector2 gridPosition) {
        return Vector2Scale(gridPosition, GRID_CELL_SIZE_PX);
    }

    Vector2 EditWindow::getScreenToGridPosition(Vector2 screenPosition) {
        return getWorldToGridPosition(GetScreenToWorld2D(screenPosition, m_camera));
    }

    Vector2 EditWindow::getGridToScreenPosition(Vector2 gridPosition) {
        return GetWorldToScreen2D(getGridToWorldPosition(gridPosition), m_camera);
    }

    void EditWindow::redrawTexture() {
        if(IsWindowResized()) {
            UnloadRenderTexture(m_renderTexture);
            m_renderTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        }

        BeginTextureMode(m_renderTexture);
        BeginMode2D(m_camera);

        ClearBackground(BLACK);

        // Draw grid
        {
            float cellSizePx = GRID_CELL_SIZE_PX;

            int topY = GetScreenToWorld2D({0, (float)GetScreenHeight()}, m_camera).y;
            int botY = GetScreenToWorld2D({0,0}, m_camera).y;
            int leftX = GetScreenToWorld2D({0,0}, m_camera).x;
            int rightX = GetScreenToWorld2D({(float)GetScreenWidth(), 0}, m_camera).x;

            int gridXStart = floor(leftX / cellSizePx);
            int gridXEnd = ceil(rightX / cellSizePx);
            for(int x = gridXStart; x <= gridXEnd; x++) {
                int px = x * cellSizePx;
                Vector2 startPos = {px, botY};
                Vector2 endPos = {px, topY};
                DrawLineEx(startPos, endPos, GRID_THICKNESS_PX, GRID_COLOR);
            }

            int gridYStart = floor(botY / cellSizePx);
            int gridYEnd = ceil(topY / cellSizePx);
            for(int y = gridYStart; y <= gridYEnd; y++) {
                int px = y * cellSizePx;
                Vector2 startPos = {leftX, px};
                Vector2 endPos = {rightX, px};
                DrawLineEx(startPos, endPos, GRID_THICKNESS_PX, GRID_COLOR);
            }
        }

        // Draw added points
        {
            for(const Vector2& gridPoint : m_addedGridPoints) {
                Vector2 worldPoint = getGridToWorldPosition(gridPoint);
                DrawCircle(worldPoint.x, worldPoint.y, GRID_CELL_SIZE_PX / 4, GOLD);
            }
        }

        DrawCircle(0, 0, 20, RED);

        EndMode2D();
        EndTextureMode();
    }

    void EditWindow::drawScene() {
        static bool redraw = true;
        static bool dragging = false;
        Vector2 mousePos = GetMousePosition();

        Rectangle contentRect = {
            ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x,
            ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y,
            ImGui::GetContentRegionAvail().x,
            ImGui::GetContentRegionAvail().y
        };
        if(CheckCollisionPointRec(mousePos, contentRect)) {
            float wheelMovement = GetMouseWheelMove();
            if(wheelMovement != 0) {
                Vector2 mouseWorldPos = GetScreenToWorld2D(mousePos, m_camera);
                m_camera.offset = mousePos;
                m_camera.target = mouseWorldPos;
                float zoomDelta = wheelMovement * ZOOM_SENSITIVITY;
                m_camera.zoom = std::clamp<float>(m_camera.zoom + zoomDelta, MIN_ZOOM, MAX_ZOOM);
                redraw = true;
            }

            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                Vector2 mouseDelta = GetMouseDelta();
                mouseDelta = Vector2Scale(mouseDelta, 1.0 / m_camera.zoom);
                mouseDelta.x *= -1;
                m_camera.target = Vector2Add(m_camera.target, mouseDelta);

                dragging = true;
                redraw = true;
            } else {
                dragging = false;
            }
        }


        if(redraw) {
            redraw = false;
            redrawTexture();
        }

        ImVec2 size = ImGui::GetContentRegionAvail();
        Rectangle sourceRect = {0, 0, m_renderTexture.texture.width, m_renderTexture.texture.height};
        rlImGuiImageRect(&m_renderTexture.texture, size.x, size.y, sourceRect);
    }

    void EditWindow::resetCamera() {
        m_camera.offset = Vector2{0,0};
        m_camera.rotation = 0;
        m_camera.target = Vector2{0,0};
        m_camera.zoom = 1;
        redrawTexture();
    }

    // 
    // Public
    // 

    EditWindow::EditWindow():
        m_camera({}),
        m_renderTexture(LoadRenderTexture(GetScreenWidth(), GetScreenHeight())),
        m_state(State::NONE)
    {
        resetCamera();
    }

    EditWindow::~EditWindow() {
        UnloadRenderTexture(m_renderTexture);
    }

    void EditWindow::draw() {
        if(ImGui::Begin("Edit", nullptr, ImGuiWindowFlags_MenuBar)) {
            drawScene();

            if(ImGui::BeginMenuBar()) {
                if(ImGui::BeginMenu("Camera")) {
                    if(ImGui::MenuItem("Reset")) {
                        resetCamera();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            if(ImGui::BeginPopupContextWindow()) {
                if(ImGui::MenuItem("Add")) {
                    m_state = State::ADDING;
                    Vector2 mouseGridPosition = getScreenToGridPosition(GetMousePosition());
                    m_addedGridPoints.push_back(mouseGridPosition);
                }
                ImGui::EndPopup();
            }
        }
        ImGui::End();
    }
}
