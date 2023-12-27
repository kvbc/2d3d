#pragma once

#include "Tesselator.hpp"
#include "Shape.hpp"
#include "Window.hpp"

#include "raylib.h"
#include "imgui.h"

#include <vector>
#include <optional>
#include <string_view>
#include <string>

namespace App {

    class EditWindow : Window {
    public: 
        enum class View {
            Right, Left, // +X, -X
            Top, Bottom, // +Y, -Y
            Front, Back  // +Z, -Z
        };

        EditWindow(View view);
        void Update();

    private:
        const float ZOOM_SENSITIVITY = 0.125;
        const float MIN_ZOOM = 0.5;
        const float MAX_ZOOM = 3;
        const int GRID_CELL_SIZE_PX = 50;
        const int GRID_THICKNESS_PX = 5;
        const Color GRID_COLOR = {30, 30, 30, 255};
        const Color GRID_AXIS_COLOR = {60, 60, 60, 255};

        enum class State {
            NONE,
            ADDING
        };

        // Adding Shape
        bool addAddingShapePoint(Vector2 point);
        bool canAddAddingShapePoint(Vector2 point) const;

        // Coordinates
        Vector3 getViewDirection() const;
        Vector3 getPointToVertex(Vector2 v) const;
        Vector2 getVertexToPoint(Vector3 v) const;
        Vector2 getScreenToGridPosition(Vector2 screenPosition) const;
        Vector2 getWorldToGridPosition(Vector2 worldPosition) const;
        Vector2 getGridToWorldPosition(Vector2 gridPosition) const;
        Vector2 getMouseGridPosition() const;
        std::vector<Vector2> getVertexToPointArray(const std::vector<Vector3>& vertices) const;
        
        void resetCamera(bool redraw);

        // Draw
        void drawWindow();
        void drawScene();
        void drawTexture();
        void drawEdge(Vector2 p1, Vector2 p2, Color color);
        void drawPoint(Vector2 gridPoint, Color color);

        std::vector<Vector2> m_addingShapePoints;
        View m_view;
        State m_state;
        Camera2D m_camera;
        ImVec2 m_windowPos;
        ImVec2 m_windowSize;
    };

}