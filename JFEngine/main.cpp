#include <iostream>
#include "jfengine.h"
#include "jfrendersystem.h"
#include "JFOpenGL.h"
#include "nba2k_offseason_tasks.h"

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main()
{
	//int data[] = { 5, 9, 1, 4, 3, 7, 2, 8, 6, 0 };
	//NBA2KOffseasonManager TaskManager(data, 10);
	JFENGINE::GetInstance()->Init();
    GLFWwindow* window = JFENGINE::GetInstance()->GetRenderSystem()->GetWindow();
	while (!JFENGINE::GetInstance()->ShouldExit())
	{
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //processInput(window, deltaTime);
		JFENGINE::GetInstance()->Update(deltaTime);
	}
	JFENGINE::GetInstance()->Deinit();
}