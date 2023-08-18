#pragma once

#include "imgui.h"
#include "raylib.h"
#include <vector>
#include <optional>

namespace App {

    class EditWindow {
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

        struct Shape {
            bool isComplete = false;
            std::vector<Vector2> points;

            Shape();
            bool canAddPoint(Vector2 point);
        };

        std::vector<Shape> m_shapes;
        State m_state;
        RenderTexture2D m_renderTexture;
        Camera2D m_camera;
        ImVec2 m_windowPos;
        ImVec2 m_windowSize;

        Vector2 getScreenToGridPosition(Vector2 screenPosition);
        Vector2 getWorldToGridPosition(Vector2 worldPosition);
        Vector2 getGridToWorldPosition(Vector2 gridPosition);
        Vector2 getMouseGridPosition();
        void resetCamera();
        void redrawTexture();
        void drawScene();
        void draw();
    public: 
        EditWindow();
        ~EditWindow();
        void update();
    };

}