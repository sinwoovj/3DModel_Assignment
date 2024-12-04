#include <GL/glew.h>

#include "Level.h"
#include "CS300Parser.h"
#include "Controls.h"
#include "models.h"
#include <chrono>

#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


Level* Level::ptr = nullptr;

int Level::Initialize()
{
	// Initialize GLFW
	if (!glfwInit()) {
		return -1;
	}

	// Create a window
	window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Assignment 1", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return -1;
	}

	glViewport(0, 0, W_WIDTH, W_HEIGHT);

	//Set callbacks
	glfwSetKeyCallback(window, Controls::keyCallback);

	//Load Scene
	CS300Parser parser;
	parser.LoadDataFromFile("data/scenes/scene_A1.txt");

	//Convert from parser->obj to Model
	for (auto o : parser.objects)
	{
		allObjects.push_back(new Model(o));
	}

	//Save camera
	cam.fovy = parser.fovy;
	cam.width = parser.width;
	cam.height = parser.height;
	cam.nearPlane = parser.nearPlane;
	cam.farPlane = parser.farPlane;
	cam.camPos = parser.camPos;
	cam.camTarget = parser.camTarget;
	cam.camUp = parser.camUp;
	 
	
	//Shader program
	ReloadShaderProgram();
	//glEnable(GL_CULL_FACE);

	//glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);

	return 0;
}


void Level::Run()
{
	static float time = 0;
	using clock = std::chrono::high_resolution_clock; // 높은 해상도의 시계 사용
	auto lastTime = clock::now();
	glClearColor(0, 0, 0, 1);
	// Main loop
	while (!glfwWindowShouldClose(window)) 
	{	
		KeyCheck();

		// Render graphics here
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//use shader program
		glUseProgram(shader->handle);
		

		//Calculate Camera Matrix
		glm::vec3 dir = glm::normalize(cam.camTarget - cam.camPos);
		dir = -dir;
		glm::vec3 r = glm::normalize(glm::cross(cam.camUp, dir));
		glm::mat4 V = glm::mat4(1);
		glm::vec3 up = glm::normalize(glm::cross(dir, r));


		V[0][0] = r.x;
		V[1][0] = r.y;
		V[2][0] = r.z;
		V[0][1] = up.x;
		V[1][1] = up.y;
		V[2][1] = up.z;
		V[0][2] = dir.x;
		V[1][2] = dir.y;
		V[2][2] = dir.z;
		V[3][0] = -dot(r,cam.camPos );
		V[3][1] = -dot(up, cam.camPos);
		V[3][2] = -dot(dir, cam.camPos);


		//cam.ViewMat = glm::lookAt(cam.camPos, cam.camTarget, up);
		cam.ViewMat = V;

		//The image is mirrored on X
		cam.ProjMat = glm::perspective(glm::radians(cam.fovy), cam.width / cam.height, cam.nearPlane, cam.farPlane);

		auto currentTime = clock::now();
		std::chrono::duration<float> deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		time += deltaTime.count();
		//For each object in the level
		for (auto o : allObjects)
		{
			glm::vec3 temp = o->transf.st_pos;

			//Animation Update
			for (auto anim : o->transf.anims)
			{
				temp = anim.Update(temp, time);
			}
			o->transf.pos = temp;

			//Render the object
			Render(o);
		}
		if (showNormal)
		{
			glUseProgram(normal_shader->handle);
			for (auto o : allObjects)
			{
				//Render the object
				Render(o);
			}
		}
		glUseProgram(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return;
}

void Level::Cleanup()
{
	// Clean up
	glfwDestroyWindow(window);
	glfwTerminate();

	DeletePtr();
}

Level* Level::GetPtr()
{
	if (!ptr)
		ptr = new Level;
	return ptr;
}

void Level::DeletePtr()
{
	if (ptr)
	{
		delete ptr;
	}
	
	ptr = nullptr;
}

#include <fstream>
#include <sstream>

void Level::ReloadShaderProgram()
{
	glUseProgram(0);

	if (shader)
		delete shader;
	if (normal_shader)
		delete normal_shader;
	std::stringstream v;
	std::stringstream f;
	std::stringstream nv;
	std::stringstream nf;
	std::stringstream ng;

	std::ifstream file("data/shaders/vert.vert");

	if (file.is_open())
	{
		v << file.rdbuf();
	}

	file.close();
	file.open("data/shaders/frag.frag");
	f << file.rdbuf();
	file.close();
	file.open("data/shaders/vert_n.vert");
	nv << file.rdbuf();
	file.close();
	file.open("data/shaders/frag_n.frag");
	nf << file.rdbuf();
	file.close();
	file.open("data/shaders/geom_n.geo");
	ng << file.rdbuf();
	file.close();

	shader = new cg::Program(v.str().c_str(), f.str().c_str());
	normal_shader = new cg::Program(nv.str().c_str(), nf.str().c_str(), ng.str().c_str());
}

void Level::RotateCamX(float angle)
{
	glm::vec3 dir = glm::normalize(cam.camTarget - cam.camPos);
	glm::vec3 camRight = glm::normalize(glm::cross(cam.camUp, dir));

	cam.camPos = cam.camTarget - glm::vec3(
		glm::rotate(
			glm::identity<glm::mat4>(), 
			glm::radians(-angle), 
			camRight)
		* glm::vec4(cam.camTarget - cam.camPos,1));
}

void Level::RotateCamY(float angle)
{
	cam.camPos = cam.camTarget - glm::vec3(
		glm::rotate(
			glm::identity<glm::mat4>(), 
			glm::radians(-angle), 
			cam.camUp)
		* glm::vec4(cam.camTarget - cam.camPos,1));
}

void Level::ZoomCamZ(float size)
{
	cam.camPos.z += size;
}

void Level::AddObject(Model* model)
{
	allObjects.push_back(model);
}

void Level::Render(Model* obj)
{
	//use obj VBO
	glBindBuffer(GL_ARRAY_BUFFER, obj->VBO);
	//use obj VAO
	glBindVertexArray(obj->VAO);

	//Send model matrix to the shader
	glm::mat4x4 m2w = obj->ComputeMatrix();

	//Send view matrix to the shader
	shader->setUniform("model", cam.ProjMat * cam.ViewMat * m2w);
	normal_shader->setUniform("model", cam.ProjMat * cam.ViewMat * m2w);

	shader->setUniform("useTexture", texture);
	shader->setUniform("tex", 6);

	if (texture)
	{
		glBindTextureUnit(6, obj->texobj);
		glBindTexture(GL_TEXTURE_2D, obj->texobj);
	}

	//draw
	glDrawArrays(GL_TRIANGLES, 0, obj->points.size());
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
}


Level::Level() : window(nullptr), shader(nullptr), normal_shader(nullptr), texture(false)
{

}

Level::~Level()
{
	for (auto m : allObjects)
		delete m;

	allObjects.clear();
}

void Level::KeyCheck()
{
	float speed = 1.0f;

	//W,A,S,D,Q,E - CAMERA CONTROL
	if (key.A == GLFW_PRESS || key.A == GLFW_REPEAT) ptr->RotateCamY(speed);
	if (key.D == GLFW_PRESS || key.D == GLFW_REPEAT) ptr->RotateCamY(-speed);
	if (key.W == GLFW_PRESS || key.W == GLFW_REPEAT) ptr->RotateCamX(-speed);
	if (key.S == GLFW_PRESS || key.S == GLFW_REPEAT) ptr->RotateCamX(speed);
	if (key.Q == GLFW_PRESS || key.Q == GLFW_REPEAT) ptr->ZoomCamZ(-speed);
	if (key.E == GLFW_PRESS || key.E == GLFW_REPEAT) ptr->ZoomCamZ(speed);
}
