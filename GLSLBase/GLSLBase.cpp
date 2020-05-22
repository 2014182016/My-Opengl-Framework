/*
Copyright 2018 Lee Taek Hee (Korea Polytech University)

This program is free software: you can redistribute it and/or modify
it under the terms of the What The Hell License. Do it plz.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY.
*/

#include "stdafx.h"

#include "Dependencies\glew.h"
#include "Dependencies\freeglut.h"

#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
#include "GameTimer.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

Renderer* g_Renderer = nullptr;
Scene* g_Scene = nullptr;
GameTimer* g_GameTimer = nullptr;
POINT lastMousePos;

void RenderScene(void)
{
	g_GameTimer->Tick();
	float deltaTime = g_GameTimer->GetDeltaTime();

	g_Renderer->Update(deltaTime);
	g_Renderer->Render();

	glutSwapBuffers();
}

void Idle(void)
{
	RenderScene();
}

void MouseInput(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
	{
		lastMousePos.x = x;
		lastMousePos.y = y;
	}
}

void MouseMotion(int x, int y)
{
	float dx = static_cast<float>(x - lastMousePos.x);
	float dy = static_cast<float>(y - lastMousePos.y);
	g_Scene->GetCamera()->RotateY(-dx);
	g_Scene->GetCamera()->Pitch(-dy);

	lastMousePos.x = x;
	lastMousePos.y = y;
}

void KeyInput(unsigned char key, int x, int y)
{
	float deltaTime = g_GameTimer->GetDeltaTime();

	if (key == 'w' || key == 'W')
	{
		g_Scene->GetCamera()->Walk(-deltaTime);
	}
	else if (key == 's' || key == 'S')
	{
		g_Scene->GetCamera()->Walk(deltaTime);
	}
	else if (key == 'd' || key == 'D')
	{
		g_Scene->GetCamera()->Strafe(deltaTime);
	}
	else if (key == 'a' || key == 'A')
	{
		g_Scene->GetCamera()->Strafe(-deltaTime);
	}
}

void SpecialKeyInput(int key, int x, int y)
{
	RenderScene();
}

void Resize(int width, int height)
{
	glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
}

int main(int argc, char **argv)
{
	// Initialize GL things
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("My OpenGL");
	glewInit();

	// Initialize Renderer
	g_Scene = new Scene();
	g_Renderer = new Renderer();
	g_GameTimer = new GameTimer();

	g_Renderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT, g_Scene);

	glutReshapeFunc(Resize);
	glutDisplayFunc(RenderScene);
	glutIdleFunc(Idle);
	glutKeyboardFunc(KeyInput);
	glutMouseFunc(MouseInput);
	glutMotionFunc(MouseMotion);
	glutSpecialFunc(SpecialKeyInput);

	g_GameTimer->Reset();

	glutMainLoop();

	delete g_Renderer;
	delete g_Scene;
	delete g_GameTimer;

    return 0;
}

