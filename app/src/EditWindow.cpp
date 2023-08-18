#include "EditWindow.hpp"
#include <iostream>
#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"
#include "raymath.h"
#include "rlgl.h"
#include <algorithm>

namespace App {
    EditWindow::Shape::Shape() {}
    bool EditWindow::Shape::canAddPoint(Vector2 gridPoint) {
        // skip 1st point check!
        for(size_t i = 1; i < points.size(); i++)
            if(Vector2Equals(points[i], gridPoint))
                return false;
        return true;
    }

    // 
    // Private
    // 

    Vector2 EditWindow::getWorldToGridPosition(Vector2 worldPosition) {
        worldPosition.x = round(worldPosition.x / GRID_CELL_SIZE_PX);
        worldPosition.y = round(worldPosition.y / GRID_CELL_SIZE_PX);
        return worldPosition;
    }

    Vector2 EditWindow::getGridToWorldPosition(Vector2 gridPosition) {
        return Vector2Scale(gridPosition, GRID_CELL_SIZE_PX);
    }

    Vector2 EditWindow::getScreenToGridPosition(Vector2 screenPos) {
        screenPos.x -= m_windowPos.x;
        screenPos.y -= m_windowPos.y;
        screenPos.x *= GetScreenWidth() / m_windowSize.x;
        screenPos.y *= GetScreenHeight() / m_windowSize.y;
        screenPos.y = GetScreenHeight() - screenPos.y;
        Vector2 worldPos = GetScreenToWorld2D(screenPos, m_camera);
        return getWorldToGridPosition(worldPos);
    }

    Vector2 EditWindow::getMouseGridPosition() {
        return getScreenToGridPosition(GetMousePosition());
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

            float topY = GetScreenToWorld2D({0, (float)GetScreenHeight()}, m_camera).y;
            float botY = GetScreenToWorld2D({0,0}, m_camera).y;
            float leftX = GetScreenToWorld2D({0,0}, m_camera).x;
            float rightX = GetScreenToWorld2D({(float)GetScreenWidth(), 0}, m_camera).x;

            int gridXStart = floor(leftX / cellSizePx);
            int gridXEnd = ceil(rightX / cellSizePx);
            for(int x = gridXStart; x <= gridXEnd; x++) {
                if(x != 0) {
                    float px = x * cellSizePx;
                    DrawLineEx({px, botY}, {px, topY}, GRID_THICKNESS_PX, GRID_COLOR);
                }
            }

            int gridYStart = floor(botY / cellSizePx);
            int gridYEnd = ceil(topY / cellSizePx);
            for(int y = gridYStart; y <= gridYEnd; y++) {
                if(y != 0) {
                    float px = y * cellSizePx;
                    DrawLineEx({leftX, px}, {rightX, px}, GRID_THICKNESS_PX, GRID_COLOR);
                }
            }

            if(gridXStart <= 0 && gridXEnd >= 0) {
                DrawLineEx({0, botY}, {0, topY}, GRID_THICKNESS_PX, GRID_AXIS_COLOR);
            }
            if(gridYStart <= 0 && gridYEnd >= 0) {
                DrawLineEx({leftX, 0}, {rightX, 0}, GRID_THICKNESS_PX, GRID_AXIS_COLOR);
            }
        }

        // Draw shapes
        for(Shape& shape : m_shapes) {
            Vector2 mouseGridPos = getMouseGridPosition();
            bool canAddPoint = shape.canAddPoint(mouseGridPos);
            if(!shape.isComplete)
                shape.points.push_back(mouseGridPos);
            size_t endIndex = shape.points.size() - 1;

            // Draw lines (connections)
            for(size_t i = 1; i < shape.points.size(); i++) {
                const Vector2& gridPoint = shape.points[i];
                const Vector2& prevGridPoint = shape.points[i - 1];
                Vector2 prevWorldPoint = getGridToWorldPosition(prevGridPoint);
                Vector2 worldPoint = getGridToWorldPosition(gridPoint);
                Color color = {50, 50, 50, 255};
                if(!shape.isComplete)
                if(i == endIndex) {
                    if(!canAddPoint)
                        color = RED;
                    color.a /= 2;
                }
                DrawLineEx(prevWorldPoint, worldPoint, GRID_THICKNESS_PX, color);
            }

            // Draw circles
            for(size_t i = 0; i < shape.points.size(); i++) {
                const Vector2& gridPoint = shape.points[i];
                Vector2 worldPoint = getGridToWorldPosition(gridPoint);
                Color color = GOLD;
                if(!shape.isComplete)
                if(i == endIndex) {
                    if(!canAddPoint)
                        color = RED;
                    color.a /= 2;
                }
                DrawCircle(worldPoint.x, worldPoint.y, GRID_CELL_SIZE_PX / 4, color);
            }

            if(!shape.isComplete)
                shape.points.pop_back();
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
                mouseDelta.y *= -1;
                m_camera.target = Vector2Subtract(m_camera.target, mouseDelta);

                dragging = true;
                redraw = true;
            } else {
                dragging = false;
            }
        }

        // if(redraw) {
        //     redraw = false;
        //     redrawTexture();
        // }
        redrawTexture();

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

    void EditWindow::draw() {
        if(ImGui::Begin("Edit", nullptr, ImGuiWindowFlags_MenuBar)) {
            m_windowPos = ImGui::GetWindowPos();
            m_windowPos.x += ImGui::GetWindowContentRegionMin().x;
            m_windowPos.y += ImGui::GetWindowContentRegionMin().y;
            m_windowSize = ImGui::GetContentRegionAvail();

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
                if(m_state == State::NONE)
                if(ImGui::MenuItem("Add")) {
                    m_state = State::ADDING;
                    m_shapes.push_back(Shape());
                }
                ImGui::EndPopup();
            }
        }
        ImGui::End();
    }

    // 
    // Public
    // 

    EditWindow::EditWindow():
        m_camera({}),
        m_renderTexture(LoadRenderTexture(GetScreenWidth(), GetScreenHeight())),
        m_state(State::NONE),
        m_windowPos(0, 0),
        m_windowSize(0, 0)
    {
        resetCamera();
    }

    EditWindow::~EditWindow() {
        UnloadRenderTexture(m_renderTexture);
    }

    void EditWindow::update() {
        if(m_state == State::ADDING) {
            Shape& shape = m_shapes.back();
            static Vector2 lastMousePos;
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                lastMousePos = GetMousePosition();
            } else if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                Vector2 mousePos = GetMousePosition();
                int dx = lastMousePos.x - mousePos.x;
                int dy = lastMousePos.y - mousePos.y;
                if(dx == 0 && dy == 0) {
                    Vector2 mouseGridPos = getMouseGridPosition();
                    if(shape.canAddPoint(mouseGridPos)) {
                        if(shape.points.size() > 1)
                        if(Vector2Equals(shape.points.front(), mouseGridPos)) {
                            shape.isComplete = true;
                            m_state = State::NONE;
                        }
                        shape.points.push_back(getMouseGridPosition());
                    }
                }
            } else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                m_shapes.pop_back();
                m_state = State::NONE;
                // TODO: Make context menu not show
            }
        }

        draw();
    }
}
