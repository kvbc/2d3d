#include "Tesselator.h"
#include "EditWindow.hpp"
#include "App.hpp"

#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"
#include "raymath.h"
#include "rlgl.h"

#include <iostream>
#include <algorithm>

namespace App {
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //
    // [Public]
    //
    //
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditWindow::EditWindow(View view):
        m_state(State::NONE),
        m_windowPos(0, 0),
        m_windowSize(0, 0),
        m_view(view)
    {
        resetCamera(false);
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
                    if(canAddShapePoint(shape, mouseGridPoint)) {
                        addShapePoint(shape, mouseGridPoint);
                        if(isShapeComplete(shape))
                            m_state = State::NONE;
                    }
                }
            } else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                App::Get().PopShape();
                m_state = State::NONE;
                // TODO: Make context menu not show
            }
        }

        drawWindow();
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //
    // [Private] Coordinates
    //
    //
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

    Vector3 EditWindow::getViewDirection() const {
        switch(m_view) {
            case View::Right : return {+1,0,0};
            case View::Left  : return {-1,0,0};
            case View::Top   : return {0,+1,0};
            case View::Bottom: return {0,-1,0};
            case View::Front : return {0,0,+1};
            case View::Back  : return {0,0,-1};
        }
        assert(false);
        return {};
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //
    // [Private]
    //
    //
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditWindow::resetCamera(bool redraw = true) {
        m_camera.offset = Vector2{0,0};
        m_camera.rotation = 0;
        m_camera.target = Vector2{0,0};
        m_camera.zoom = 1;
        if(redraw)
            redrawTexture();
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //
    // [Private] Draw
    //
    //
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditWindow::drawWindow() {
        const char * name = NULL;
        switch(m_view) {
        case View::Left  : name = "Left View###X"; break;
        case View::Right : name = "Right View###X"; break;
        case View::Top   : name = "Top View###Y"; break;
        case View::Bottom: name = "Bottom View###Y"; break;
        case View::Front : name = "Front View###Z"; break;
        case View::Back  : name = "Back View###Z"; break;
        }
        if(ImGui::Begin(name, nullptr, ImGuiWindowFlags_MenuBar)) {
            m_windowPos = ImGui::GetWindowPos();
            m_windowPos.x += ImGui::GetWindowContentRegionMin().x;
            m_windowPos.y += ImGui::GetWindowContentRegionMin().y;
            m_windowSize = ImGui::GetContentRegionAvail();

            drawScene();

            if(ImGui::BeginMenuBar()) {
                if(ImGui::BeginMenu("Camera")) {
                    if(ImGui::MenuItem("Reset"))
                        resetCamera();
                    if(ImGui::MenuItem("Flip"))
                        switch(m_view) {
                        case View::Left  : m_view = View::Right; break;
                        case View::Right : m_view = View::Left; break;
                        case View::Top   : m_view = View::Bottom; break;
                        case View::Bottom: m_view = View::Top; break;
                        case View::Front : m_view = View::Back; break;
                        case View::Back  : m_view = View::Front; break;
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

    void EditWindow::drawScene() {
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
                // redraw = true;
            }

            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                Vector2 mouseDelta = GetMouseDelta();
                mouseDelta = Vector2Scale(mouseDelta, 1.0 / m_camera.zoom);
                mouseDelta.y *= -1;
                m_camera.target = Vector2Subtract(m_camera.target, mouseDelta);

                dragging = true;
                // redraw = true;
            } else {
                dragging = false;
            }
        }

        // if(redraw) {
        //     redraw = false;
        //     redrawTexture();
        // }
        redrawTexture();
        renderTexture();
    }

    void EditWindow::drawTexture() {
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
            bool canAddPoint = canAddShapePoint(shape, mouseGridPoint);
            std::vector<Vector2> points = getShapePoints(shape);
            size_t mousePointIndex = points.size();
            if(!isShapeComplete(shape))
                points.push_back(mouseGridPoint);

            // Draw fill
            if(isShapeComplete(shape)) {
                for(const Shape::Face& face : shape.GetFaces()) {
                    std::vector<Vector2> gridPoints = getShapeFacePoints(shape, face);
                    gridPoints.pop_back();
                    std::vector<Vector2> tessGridPoints;
                    Tesselator::Get().Tesselate2D(gridPoints, tessGridPoints);

                    std::vector<Vector2> tessWorldPoints;
                    for(const Vector2& gridPoint : tessGridPoints) {
                        Vector2 worldPoint = getGridToWorldPosition(gridPoint);
                        tessWorldPoints.push_back(worldPoint);
                    }

                    Color color;
                    switch(m_view) {
                    case View::Left  : color = MAROON; break;
                    case View::Right : color = RED; break;
                    case View::Top   : color = GREEN; break;
                    case View::Bottom: color = DARKGREEN; break;
                    case View::Front : color = BLUE; break;
                    case View::Back  : color = DARKBLUE; break;
                    }

                    for(size_t i = 0; i < tessWorldPoints.size(); i += 3) {
                        DrawTriangle(
                            tessWorldPoints[i],
                            tessWorldPoints[i+1],
                            tessWorldPoints[i+2],
                            color
                        );
                    }
                }
            }

            // Draw lines (connections)
            for(size_t i = 1; i < points.size(); i++) {
                Vector2 gridPoint = points[i];
                Vector2 prevGridPoint = points[i-1];
                Vector2 prevWorldPoint = getGridToWorldPosition(prevGridPoint);
                Vector2 worldPoint = getGridToWorldPosition(gridPoint);
                Color color = {50, 50, 50, 255};
                if(i == mousePointIndex) {
                    if(!canAddPoint)
                        color = RED;
                    color.a /= 2;
                }
                DrawLineEx(prevWorldPoint, worldPoint, GRID_THICKNESS_PX, color);
            }

            // Draw circles
            for(size_t i = 0; i < points.size(); i++) {
                Vector2 gridPoint = points[i];
                Vector2 worldPoint = getGridToWorldPosition(gridPoint);
                Color color = GOLD;
                if(i == mousePointIndex) {
                    if(!canAddPoint)
                        color = RED;
                    color.a /= 2;
                }
                DrawCircle(worldPoint.x, worldPoint.y, GRID_CELL_SIZE_PX / 4, color);
            }
        }

        EndMode2D();
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //
    // [Private] Shape
    //
    //
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditWindow::canAddShapePoint(const Shape& shape, Vector2 point) const {
        std::vector<Vector2> points = getShapePoints(shape);
        for(size_t i = 1; i < points.size(); i++) // skip 1st check
            if(Vector2Equals(points[i], point))
                return false;
        return true;
    }

    void EditWindow::addShapePoint(Shape& shape, Vector2 point) {
        Vector3 vertex = getPointToVertex(point);
        Vector3 vertex1 = Vector3Subtract(vertex, getViewDirection());
        Vector3 vertex2 = Vector3Add(vertex1, getViewDirection());
        shape.AddVertex(vertex1);
        shape.AddVertex(vertex2);

        // Add faces
        if(isShapeComplete(shape)) {
            std::vector<Vector2> points = getShapePoints(shape);

            Shape::Face frontFace, backFace;
            for(size_t i = 0; i < points.size(); i++) { // CCW
                frontFace.push_back(i * 2);
                backFace.push_back(i * 2 + 1);
            }

            App::WindingOrder placeWindingOrder = App::DetermineWindingOrder2D(points); // order the points have been placed in
            if(placeWindingOrder == App::WindingOrder::CLOCKWISE) {
                std::reverse(frontFace.begin(), frontFace.end());
            }
            else {
                std::reverse(backFace.begin(), backFace.end());
            }

            shape.AddFace(frontFace, Vector3Negate(getViewDirection()));
            shape.AddFace(backFace, getViewDirection());

            for(size_t i = 1; i < points.size(); i++) {
                Shape::Face edgeFace;
                edgeFace.push_back((i - 1) * 2);
                edgeFace.push_back(i * 2);
                edgeFace.push_back(i * 2 + 1);
                edgeFace.push_back((i - 1) * 2 + 1);
                shape.AddFace(edgeFace, getViewDirection());
            }
        }
    }

    std::vector<Vector2> EditWindow::getShapeFacePoints(const Shape& shape, const Shape::Face& face) const {
        std::vector<Vector2> points;
        for(Vector3 vertex : shape.GetFaceVertices(face)) {
            Vector2 point = getVertexToPoint(vertex);
            points.push_back(point);
        }
        return points;
    }

    bool EditWindow::isShapeComplete(const Shape& shape) const {
        const std::vector<Vector2>& points = getShapePoints(shape);
        return
            (points.size() > 2) &&
            Vector2Equals(points.front(), points.back());
    }

    std::vector<Vector2> EditWindow::getShapePoints(const Shape& shape) const {
        std::vector<Vector2> points;
        const std::vector<Vector3>& vertices = shape.GetVertices();

        for(Vector3 vertex : vertices) {
            Vector2 point = getVertexToPoint(vertex);
            if(std::any_of(points.begin(), points.end(), [=](Vector2 alrPoint) {
                return Vector2Equals(alrPoint, point);
                }))
                continue;
                points.push_back(point);
        }

        if(!points.empty()) {
            Vector3 lastVertex = vertices.back();
            Vector2 lastVertexPoint = getVertexToPoint(lastVertex);
            if(Vector2Equals(lastVertexPoint, points[0]))
                points.push_back(lastVertexPoint);
        }

        return points;
    }

    // void EditWindow::removeShapePoint(Shape& shape, Vector2 point) {
    //     for(Vector3 vertex : shape.GetVertices()) {
    //         Vector2 vertexPoint = getVertexToPoint(vertex);
    //         if(Vector2Equals(vertexPoint, point))
    //             shape.RemoveVertex(vertex);
    //     }
    // }

}