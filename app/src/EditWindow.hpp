#pragma once

#include "raylib.h"
#include <vector>

namespace App {

    class EditWindow {
    private:
        const float ZOOM_SENSITIVITY = 0.125;
        const float MIN_ZOOM = 0.5;
        const float MAX_ZOOM = 3;
        const int GRID_CELL_SIZE_PX = 50;
        const int GRID_THICKNESS_PX = 5;
        const Color GRID_COLOR = {30, 30, 30, 255};

        enum class State {
            NONE,
            ADDING
        };

        std::vector<Vector2> m_addedGridPoints;
        State m_state;
        RenderTexture2D m_renderTexture;
        Camera2D m_camera;

        Vector2 getScreenToGridPosition(Vector2 screenPosition);
        Vector2 getGridToScreenPosition(Vector2 gridPosition);
        Vector2 getWorldToGridPosition(Vector2 worldPosition);
        Vector2 getGridToWorldPosition(Vector2 gridPosition);
        void resetCamera();
        void redrawTexture();
        void drawScene();
    public: 
        EditWindow();
        ~EditWindow();
        void draw();
    };

}