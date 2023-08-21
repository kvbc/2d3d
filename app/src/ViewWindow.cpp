#include "ViewWindow.hpp"
#include "imgui.h"
#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include "rlgl.h"
#include "Shape.hpp"
#include "App.hpp"
#include "Tesselator.hpp"

namespace App {

    // 
    // Public
    // 

    ViewWindow::ViewWindow() {
        resetCamera();
    }

    void ViewWindow::Update() {
        updateCamera();
        drawWindow();
    }

    // 
    // Private
    // 

    void ViewWindow::resetCamera() {
        m_camera.fovy = 90;
        m_camera.position = Vector3{5,-5,5};
        m_camera.projection = CAMERA_PERSPECTIVE;
        m_camera.target = Vector3{0,0,0};
        m_camera.up = Vector3{0,1,0};
    }

    void ViewWindow::updateCamera() {
        // move
        if(IsKeyDown(KEY_W)) CameraMoveForward(&m_camera,  CAMERA_MOVE_SPEED, false);
        if(IsKeyDown(KEY_A)) CameraMoveRight  (&m_camera, -CAMERA_MOVE_SPEED, false);
        if(IsKeyDown(KEY_S)) CameraMoveForward(&m_camera, -CAMERA_MOVE_SPEED, false);
        if(IsKeyDown(KEY_D)) CameraMoveRight  (&m_camera,  CAMERA_MOVE_SPEED, false);
        if(IsKeyDown(KEY_SPACE     )) CameraMoveUp(&m_camera, -CAMERA_MOVE_SPEED);
        if(IsKeyDown(KEY_LEFT_SHIFT)) CameraMoveUp(&m_camera,  CAMERA_MOVE_SPEED);

        // rotate (arrows)
        if(IsKeyDown(KEY_DOWN )) CameraPitch(&m_camera,  CAMERA_ROTATION_SPEED, false, false, false);
        if(IsKeyDown(KEY_UP   )) CameraPitch(&m_camera, -CAMERA_ROTATION_SPEED, false, false, false);
        if(IsKeyDown(KEY_RIGHT)) CameraYaw  (&m_camera, -CAMERA_ROTATION_SPEED, false);
        if(IsKeyDown(KEY_LEFT )) CameraYaw  (&m_camera,  CAMERA_ROTATION_SPEED, false);
        // rotate (mouse)
        if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            CameraYaw  (&m_camera, -GetMouseDelta().x * CAMERA_MOUSE_MOVE_SENSITIVITY, false);
            CameraPitch(&m_camera,  GetMouseDelta().y * CAMERA_MOUSE_MOVE_SENSITIVITY, false, false, false);
        }

        // zoom
        CameraMoveToTarget(&m_camera, -GetMouseWheelMove());

        // pan
        // TODO: this is terrible for some reason (?)
        if(IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            const Vector2 mouseDelta = GetMouseDelta();
            if(mouseDelta.x > 0) CameraMoveRight(&m_camera, -CAMERA_PAN_SPEED, false);
            if(mouseDelta.x < 0) CameraMoveRight(&m_camera,  CAMERA_PAN_SPEED, false);
            if(mouseDelta.y > 0) CameraMoveUp   (&m_camera, -CAMERA_PAN_SPEED);
            if(mouseDelta.y < 0) CameraMoveUp   (&m_camera,  CAMERA_PAN_SPEED);
        }
    }

    void ViewWindow::drawWindow() {
        if(ImGui::Begin("3D View", NULL, ImGuiWindowFlags_MenuBar)) {
            if(ImGui::BeginMenuBar()) {
                if(ImGui::BeginMenu("Camera")) {
                    if(ImGui::MenuItem("Reset"))
                        resetCamera();
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            drawScene();
        }
        ImGui::End();
    }

    void ViewWindow::drawScene() {
        redrawTexture();
        renderTexture();
    }

    void ViewWindow::drawTexture() {
        BeginMode3D(m_camera);
        ClearBackground(DARKGRAY);

        drawGrid(100, 1);
        // DrawCube(Vector3{0,0,0}, 3, 3, 3, GOLD);

        // for(const Shape& shape : App::Get().GetShapes()) {
        //     for(const Shape::Face& face : shape.GetFaces()) {
        //         const std::vector<Vector3>& vertices = shape.GetFaceVertices(face);
        //         std::vector<Vector3> tessVertices = Tesselator::Get().Tesselate3D(vertices);
        //         for(size_t i = 0; i < tessVertices.size(); i += 3) {
        //             DrawTriangle3D(
        //                 tessVertices[i],
        //                 tessVertices[i + 1],
        //                 tessVertices[i + 2],
        //                 BLUE
        //             );
        //         }
        //     }
        // }

        for(const Shape& shape : App::Get().GetShapes()) {
            DrawModel(
                shape.GetModel(),
                Vector3Zero(),
                1,
                WHITE
            );
        }        


        EndMode3D();
    }

    // modified version of raylib's DrawGrid()
    void ViewWindow::drawGrid(int slices, float spacing) {
        int halfSlices = slices / 2;

        rlBegin(RL_LINES);
            for (int i = -halfSlices; i <= halfSlices; i++)
            {
                Color color = (i == 0) ? GRID_AXIS_COLOR : GRID_COLOR;
                for(size_t j = 0; j < 4; j++)
                    rlColor3f(
                        color.r / 255.0,
                        color.g / 255.0,
                        color.b / 255.0
                    );

                rlVertex3f((float)i*spacing, 0.0f, (float)-halfSlices*spacing);
                rlVertex3f((float)i*spacing, 0.0f, (float)halfSlices*spacing);

                rlVertex3f((float)-halfSlices*spacing, 0.0f, (float)i*spacing);
                rlVertex3f((float)halfSlices*spacing, 0.0f, (float)i*spacing);
            }
        rlEnd();
    }

}