#pragma once

#include "imgui.h"
#include "raylib.h"
#include "Tesselator.hpp"
#include <vector>
#include <optional>
#include <string_view>
#include <string>

namespace App {

    class EditWindow {
    public: 
        enum class View {
            Right, Left, // +X, -X
            Up, Down, // +Y, -Y
            Front, Back // +Z, -Z
        };

        EditWindow(std::string_view name, View view);
        ~EditWindow();
        void update();

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

        View m_view;
        State m_state;
        RenderTexture2D m_renderTexture;
        Camera2D m_camera;
        ImVec2 m_windowPos;
        ImVec2 m_windowSize;
        std::string m_name;

        Vector3 getPointToVertex(Vector2 v) const;
        Vector2 getVertexToPoint(Vector3 v) const;
        Vector2 getScreenToGridPosition(Vector2 screenPosition) const;
        Vector2 getWorldToGridPosition(Vector2 worldPosition) const;
        Vector2 getGridToWorldPosition(Vector2 gridPosition) const;
        Vector2 getMouseGridPosition() const;
        void resetCamera(bool redraw);
        void redrawTexture();
        void drawScene();
        void draw();
    };

}