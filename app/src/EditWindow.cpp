#include "EditWindow.hpp"
#include <iostream>
#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"
#include "raymath.h"
#include "rlgl.h"
#include <algorithm>
#include "tesselator.h"
#include "App.hpp"

namespace App {
    // 
    // Private
    // 

    Vector2 EditWindow::getWorldToGridPosition(Vector2 worldPosition) const {
        worldPosition.x = round(worldPosition.x / GRID_CELL_SIZE_PX);
        worldPosition.y = round(worldPosition.y / GRID_CELL_SIZE_PX);
        return worldPosition;
    }

    Vector2 EditWindow::getGridToWorldPosition(Vector2 gridPosition) const {
        return Vector2Scale(gridPosition, GRID_CELL_SIZE_PX);
    }

    Vector2 EditWindow::getScreenToGridPosition(Vector2 screenPos) const {
        screenPos.x -= m_windowPos.x;
        screenPos.y -= m_windowPos.y;
        screenPos.x *= GetScreenWidth() / m_windowSize.x;
        screenPos.y *= GetScreenHeight() / m_windowSize.y;
        screenPos.y = GetScreenHeight() - screenPos.y;
        Vector2 worldPos = GetScreenToWorld2D(screenPos, m_camera);
        return getWorldToGridPosition(worldPos);
    }

    Vector2 EditWindow::getMouseGridPosition() const {
        return getScreenToGridPosition(GetMousePosition());
    }

    Vector3 EditWindow::getPointToVertex(Vector2 v) const {
        switch(m_view) {
            case View::Right :
            case View::Left  : return {0, v.x, v.y};
            case View::Top   :
            case View::Bottom: return {v.x, 0, v.y};
            case View::Front :
            case View::Back  : return {v.x, v.y, 0};
        }
        assert(false);
        return {};
    }

    Vector2 EditWindow::getVertexToPoint(Vector3 v) const {
        switch(m_view) {
            case View::Right :
            case View::Left  : return {v.y, v.z};
            case View::Top   :
            case View::Bottom: return {v.x, v.z};
            case View::Front :
            case View::Back  : return {v.x, v.y};
        }
        assert(false);
        return {};
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
        for(size_t shapeIdx = 0; shapeIdx < App::Get().GetShapeCount(); shapeIdx++) {
            Shape& shape = App::Get().GetShape(shapeIdx);
            Vector2 mouseGridPoint = getMouseGridPosition();
            Vector3 mouseGridVertex = getPointToVertex(mouseGridPoint);
            bool canAddVertex = shape.CanAddVertex(mouseGridVertex);
            if(!shape.IsComplete())
                shape.AddVertex(mouseGridVertex);
            size_t endIndex = shape.GetVertexCount() - 1;

            // Draw fill
            if(shape.IsComplete()) {
                std::vector<Vector2> gridPoints;
                for(Vector3 gridVertex : shape.GetVertices()) {
                    Vector2 gridPoint = getVertexToPoint(gridVertex);
                    gridPoints.push_back(gridPoint);
                }
                gridPoints.pop_back();
                std::vector<Vector2> tessGridPoints;
                Tesselator::Get().Tesselate(gridPoints, tessGridPoints);

                std::vector<Vector2> tessWorldPoints;
                for(const Vector2& gridPoint : tessGridPoints) {
                    Vector2 worldPoint = getGridToWorldPosition(gridPoint);
                    tessWorldPoints.push_back(worldPoint);
                }
                
                for(size_t i = 0; i < tessWorldPoints.size(); i += 3) {
                    DrawTriangle(
                        tessWorldPoints[i],
                        tessWorldPoints[i+1],
                        tessWorldPoints[i+2],
                        shape.color
                    );
                }
            }

            // Draw lines (connections)
            for(size_t i = 1; i < shape.GetVertexCount(); i++) {
                const Vector3& gridVertex = shape.GetVertex(i);
                const Vector3& prevGridVertex = shape.GetVertex(i - 1);
                Vector2 gridPoint = getVertexToPoint(gridVertex);
                Vector2 prevGridPoint = getVertexToPoint(prevGridVertex);
                Vector2 prevWorldPoint = getGridToWorldPosition(prevGridPoint);
                Vector2 worldPoint = getGridToWorldPosition(gridPoint);
                Color color = {50, 50, 50, 255};
                if(!shape.IsComplete())
                if(i == endIndex) {
                    if(!canAddVertex)
                        color = RED;
                    color.a /= 2;
                }
                DrawLineEx(prevWorldPoint, worldPoint, GRID_THICKNESS_PX, color);
            }

            // Draw circles
            for(size_t i = 0; i < shape.GetVertexCount(); i++) {
                Vector3 gridVertex = shape.GetVertex(i);
                Vector2 gridPoint = getVertexToPoint(gridVertex);
                Vector2 worldPoint = getGridToWorldPosition(gridPoint);
                Color color = GOLD;
                if(!shape.IsComplete())
                if(i == endIndex) {
                    if(!canAddVertex)
                        color = RED;
                    color.a /= 2;
                }
                DrawCircle(worldPoint.x, worldPoint.y, GRID_CELL_SIZE_PX / 4, color);
            }

            if(!shape.IsComplete())
                shape.PopVertex();
        }

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

    void EditWindow::resetCamera(bool redraw = true) {
        m_camera.offset = Vector2{0,0};
        m_camera.rotation = 0;
        m_camera.target = Vector2{0,0};
        m_camera.zoom = 1;
        if(redraw)
            redrawTexture();
    }

    void EditWindow::draw() {
        const char * name = NULL;
        switch(m_view) {
            case View::Left  : name = "Left View"; break;
            case View::Right : name = "Right View"; break;
            case View::Top   : name = "Top View"; break;
            case View::Bottom: name = "Bottom View"; break;
            case View::Front : name = "Front View"; break;
            case View::Back  : name = "Back View"; break;
        }
        if(ImGui::Begin(name, nullptr, ImGuiWindowFlags_MenuBar)) {
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
                    App::Get().PushShape(Shape());
                }
                ImGui::EndPopup();
            }
        }
        ImGui::End();
    }

    // 
    // Public
    // 

    EditWindow::EditWindow(View view):
        m_camera({}),
        m_renderTexture(LoadRenderTexture(GetScreenWidth(), GetScreenHeight())),
        m_state(State::NONE),
        m_windowPos(0, 0),
        m_windowSize(0, 0),
        m_view(view)
    {
        resetCamera(false);
    }

    EditWindow::~EditWindow() {
        UnloadRenderTexture(m_renderTexture);
    }

    void EditWindow::Update() {
        if(m_state == State::ADDING) {
            Shape& shape = App::Get().GetShape(App::Get().GetShapeCount() - 1);
            static Vector2 lastMousePos;
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                lastMousePos = GetMousePosition();
            } else if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                Vector2 mousePos = GetMousePosition();
                int dx = lastMousePos.x - mousePos.x;
                int dy = lastMousePos.y - mousePos.y;
                if(dx == 0 && dy == 0) {
                    Vector2 mouseGridPoint = getMouseGridPosition();
                    Vector3 mouseGridVertex = getPointToVertex(mouseGridPoint);
                    if(shape.CanAddVertex(mouseGridVertex)) {
                        shape.AddVertex(mouseGridVertex);
                        if(shape.GetVertexCount() > 1)
                        if(Vector3Equals(shape.GetVertex(0), mouseGridVertex)) {
                            shape.MarkAsComplete();
                            m_state = State::NONE;
                        }
                    }
                }
            } else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                App::Get().PopShape();
                m_state = State::NONE;
                // TODO: Make context menu not show
            }
        }

        draw();
    }
}
