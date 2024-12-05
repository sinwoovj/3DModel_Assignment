#pragma once

#define W_WIDTH 1280.0
#define W_HEIGHT 720.0

class GLFWwindow;
class Model;

#include <vector>
#include "program.h"
#include <GLFW/glfw3.h>
#include "CS300Parser.h"

//Singleton level
struct Level
{
	int Initialize();
	void Run();
	void Cleanup();

	static Level* GetPtr();
	static void DeletePtr();
	void ReloadShaderProgram();
	void RotateCamX(float angle);
	void RotateCamY(float angle);
	void ZoomCamZ(float size);
	void AddObject(Model* model);
	void AddLights(CS300Parser::Light* light);
	std::vector<Model*> allObjects;
	std::vector<CS300Parser::Light*> allLights;
private:
	void Render(Model*);
	void Lighting();
	Level();
	~Level();
	Level(const Level&) = delete;
	const Level& operator=(const Level&) = delete;

	static Level* ptr;

	GLFWwindow* window;


	//keyState
	struct KeyState
	{
		//W,A,S,D,Q,E - CAMERA CONTROL
		int W = GLFW_RELEASE;
		int A = GLFW_RELEASE;
		int S = GLFW_RELEASE;
		int D = GLFW_RELEASE;
		int Q = GLFW_RELEASE;
		int E = GLFW_RELEASE;
		
		/*
		TODO: ADD/DECRESE SLICES
		 +/z - Increase Slices. Reloads Models
		 -/x - Decrease Slices. Reloads Models
		TODO: TRIGGER WIREFRAME
		 m - Wireframe On/Off
		TODO: TRIGGER TEXTURE
		 T - Texture Mapping On/Off
		TODO: TRIGGER NORMALS RENDER
		 N - Rendering Normals On/Off
		TODO: TRIGGER NORMALS AVERAGE
		 F - Average Normals On/Off
		*/
	};
	void KeyCheck();
public:
	KeyState key;
	bool showNormal = false;
	bool normalAvg = false; // false : face, true : averaged
	bool texture;
	//shaders
	cg::Program* shader;
	cg::Program* lighting_shader;
	cg::Program* normal_shader;
private:
	//camera 
	struct Camera
	{
		float     fovy = 60.0f;
		float     width = 16.0f;
		float     height = 9.0f;
		float     nearPlane = 1.0f;
		float     farPlane = 500.0f;
		glm::vec3 camPos = glm::vec3(0.0f);
		glm::vec3 camTarget = glm::vec3(0.0f);
		glm::vec3 camUp = glm::vec3(0.0f);
		glm::mat4 ViewMat;
		glm::mat4 ProjMat;
	};

	Camera cam;

};

