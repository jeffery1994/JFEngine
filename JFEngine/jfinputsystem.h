///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#pragma once

struct GLFWwindow;
class Camera;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class JFINPUT_SYSTEM
{
public:
	JFINPUT_SYSTEM();
	~JFINPUT_SYSTEM();
	bool Init(GLFWwindow* Window);
	void Deinit();
	void Update(float DeltaTime);
	void ProcessInput(float DeltaTime);
	bool GetInited() { return Inited;  }
	void UpdateCameraInput(float DeltaTime);

	void OnMouseMoved(GLFWwindow* window, float xpos, float ypos);
	void OnMouseScrolled(GLFWwindow* window, float xoffset, float yoffset);

	//Getter
	inline float GetLastX() { return this->lastX; }
	inline float GetLastY() { return this->lastY; }
private:
	GLFWwindow* Window = nullptr;
	Camera * ActiveCamera = nullptr;

	bool firstMouse = true;
	float lastX = 800.0f/2.0;
	float lastY = 800.0f/2.0;

	bool Inited = false;
};
