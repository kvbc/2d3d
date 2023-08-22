#pragma once

#include "Window.hpp"

#include "raylib.h"

#include <array>
#include <vector>

namespace App {

    class ViewWindow : Window {
    public:
        ViewWindow();
        ~ViewWindow();

        void Update();

    private:
        const Color GRID_COLOR = GRAY;
        const Color GRID_AXIS_COLOR = WHITE;

        const float CAMERA_MOVE_SPEED = 0.2;
        const float CAMERA_ROTATION_SPEED = 0.03;
        const float CAMERA_MOUSE_MOVE_SENSITIVITY = 0.01;
        const float CAMERA_PAN_SPEED = 0.3;
        const float CAMERA_ZOOM_SENSITIVITY = 2;

        const float ORIENT_CUBE_SIZE = 3;
        const float ORIENT_CUBE_CAMERA_OFFSET = 3;
        const int ORIENT_CUBE_WINDOW_SIZE = 200;
        const Color ORIENT_CUBE_COLOR = GRAY;

        void resetCamera();
        void updateCamera();

        void drawWindow();
        void drawScene();
        void drawTexture();
        void drawGrid(int slices, float spacing);

        void updateOrientationCubeCamera();
        void drawOrientationCubeTexture();
        void generateOrientationCubeModel();
        Mesh generateOrientationCubeMesh(const std::array<float, 4*3>& vertices);

        Camera3D m_camera;
        Model m_orientCubeModel;
        RenderTexture m_orientCubeTxt;
        Camera3D m_orientCubeCamera;
    };

}