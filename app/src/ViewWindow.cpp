#include "imgui.h"
#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include "rlgl.h"

#include "ViewWindow.hpp"
#include "Shape.hpp"
#include "App.hpp"
#include "Tesselator.hpp"

namespace App {

    // 
    // Public
    // 

    ViewWindow::ViewWindow():
        m_orientModelRTxt(LoadRenderTexture(ORIENT_MODEL_WINDOW_SIZE, ORIENT_MODEL_WINDOW_SIZE))
    {
        resetCamera();

        m_orientModelCamera.fovy = 90;
        m_orientModelCamera.position = Vector3Zero();
        m_orientModelCamera.projection = CAMERA_PERSPECTIVE;
        m_orientModelCamera.target = Vector3{ 0,0,0 };
        m_orientModelCamera.up = Vector3{ 0,1,0 };

        loadOrientationModel();
    }

    ViewWindow::~ViewWindow() {
        UnloadRenderTexture(m_orientModelRTxt);
        UnloadModel(m_orientModel);
    }

    void ViewWindow::Update() {
        updateCamera();
        updateOrientationModelCamera();
        drawWindow();
    }

    // 
    // Private
    // 

    void ViewWindow::resetCamera() {
        m_camera.fovy = 90;
        m_camera.position = Vector3{5,5,5};
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
        if(IsKeyDown(KEY_SPACE     )) CameraMoveUp(&m_camera,  CAMERA_MOVE_SPEED);
        if(IsKeyDown(KEY_LEFT_SHIFT)) CameraMoveUp(&m_camera, -CAMERA_MOVE_SPEED);

        // rotate (arrows)
        if(IsKeyDown(KEY_DOWN )) CameraPitch(&m_camera, -CAMERA_ROTATION_SPEED, false, false, false);
        if(IsKeyDown(KEY_UP   )) CameraPitch(&m_camera,  CAMERA_ROTATION_SPEED, false, false, false);
        if(IsKeyDown(KEY_RIGHT)) CameraYaw  (&m_camera, -CAMERA_ROTATION_SPEED, false);
        if(IsKeyDown(KEY_LEFT )) CameraYaw  (&m_camera,  CAMERA_ROTATION_SPEED, false);
        // rotate (mouse)
        if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            CameraYaw  (&m_camera, -1 * GetMouseDelta().x * CAMERA_MOUSE_MOVE_SENSITIVITY, false);
            CameraPitch(&m_camera, -1 * GetMouseDelta().y * CAMERA_MOUSE_MOVE_SENSITIVITY, false, false, false);
        }

        // zoom
        CameraMoveToTarget(&m_camera, -GetMouseWheelMove());

        // pan
        // TODO: this is terrible for some reason (?)
        if(IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            const Vector2 mouseDelta = GetMouseDelta();
            if(mouseDelta.x > 0) CameraMoveRight(&m_camera, -CAMERA_PAN_SPEED, false);
            if(mouseDelta.x < 0) CameraMoveRight(&m_camera,  CAMERA_PAN_SPEED, false);
            if(mouseDelta.y > 0) CameraMoveUp   (&m_camera,  CAMERA_PAN_SPEED);
            if(mouseDelta.y < 0) CameraMoveUp   (&m_camera, -CAMERA_PAN_SPEED);
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
        drawOrientationModelTexture();

        redrawTexture();
        renderTexture();
    }

    void ViewWindow::drawTexture() {
        ClearBackground(DARKGRAY);

        BeginMode3D(m_camera);

        drawGrid(100, 1);

        for(const Shape& shape : App::Get().GetShapes()) {
            const Model& model = shape.GetModel();

            for(size_t meshIdx = 0; meshIdx < model.meshCount; meshIdx++) {
                Mesh * mesh = &model.meshes[meshIdx];
                for(size_t vxIdx = 0; vxIdx < mesh->vertexCount; vxIdx++) {
                    float* fVertex = &mesh->vertices[vxIdx * 3];
                    float* fNormal = &mesh->normals[vxIdx * 3];
                    Vector3 vertex = { fVertex[0], fVertex[1], fVertex[2] };
                    Vector3 normal = { fNormal[0], fNormal[1], fNormal[2] };

                    Vector3 endPos = Vector3Add(vertex, normal);
                    float radius = 0.1;
                    int sides = 5;
                    Color color = WHITE;
                    DrawCylinderEx(vertex, endPos, radius, radius, sides, color);
                }
            }

            DrawModel(
                model,
                Vector3Zero(),
                1,
                WHITE
            );
        }        

        EndMode3D();

        // DrawTexture(m_orientCubeTxt.texture, 0, 0, WHITE);
        // we gotta flip the texture on the Y axis for some reason (?)
        DrawTexturePro(
            m_orientModelRTxt.texture,
            {0, 0, (float)m_orientModelRTxt.texture.width, (float)m_orientModelRTxt.texture.height * -1},
            {0, 0, (float)m_orientModelRTxt.texture.width, (float)m_orientModelRTxt.texture.height},
            {0, 0},
            0,
            WHITE
        );
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

    void ViewWindow::drawOrientationModelTexture() {
        BeginTextureMode(m_orientModelRTxt);
        ClearBackground(BLANK);
        // DrawCircle(10, 10, 10, RED);
        BeginMode3D(m_orientModelCamera);
        
        // DrawCube(Vector3Zero(), 3, 3, 3, GRAY);
        DrawModel(m_orientModel, Vector3Zero(), 1, WHITE);

        EndMode3D();
        EndTextureMode();
    }

    void ViewWindow::updateOrientationModelCamera() {
        Vector3 lookVector = GetCameraForward(&m_camera);
        Vector3 camOffset = { ORIENT_MODEL_CAMERA_OFFSET, ORIENT_MODEL_CAMERA_OFFSET, ORIENT_MODEL_CAMERA_OFFSET };
        m_orientModelCamera.position = Vector3Negate(Vector3Multiply(lookVector, camOffset));
    }

    void ViewWindow::loadOrientationModel() {
        m_orientModel = LoadModel("../app/assets/car.glb");
    }
}