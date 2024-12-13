#include <GL/glew.h>
#include <iostream>
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
	parser.LoadDataFromFile("data/scenes/scene_A3.txt");
	
	//Convert from parser->obj to Model
	for (auto o : parser.objects)
	{
		allObjects.push_back(new Model(o));
	}
	//Convert from parser->obj to Light
	for (auto l : parser.lights)
	{
		allLights.push_back(new CS300Parser::Light(l));
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
	glEnable(GL_CULL_FACE);

	//glFrontFace(GL_CW);

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

		SetMatrix();
		
		auto currentTime = clock::now();
		std::chrono::duration<float> deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		time += deltaTime.count();

		glUseProgram(shader->handle);
		shader->setUniform("projMat", cam.ProjMat);
		shader->setUniform("viewMat", cam.ViewMat);
		
		if (!allLights.empty())
		{
			glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, cam.nearPlane, cam.farPlane);
			glm::mat4 lightView = allLights.front()->lightMat;
			glm::mat4 lightSpaceMatrix = lightProjection * lightView;
			glUseProgram(shader->handle);
			shader->setUniform("lightMat", lightView);
			glUseProgram(depth_shader->handle);
			depth_shader->setUniform("lightSpaceMatrix", lightSpaceMatrix);
		}

		glEnable(GL_DEPTH_TEST);
		glDepthRange(0.0, 1.0);
		glClearDepthf(1.0F);
		//first rendering for shadow
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glClear(GL_DEPTH_BUFFER_BIT);
		for (auto o : allObjects)
		{
			//use obj FBO
			glm::vec3 temp = o->transf.st_pos;

			//Animation Update
			for (auto anim : o->transf.anims)
			{
				temp = anim.Update(temp, time);
			}
			o->transf.pos = temp;

			//Shadow the object
			if (!Shadow(o))
				std::cout << "bad" << std::endl;
		}

		// Render graphics here
		glViewport(0, 0, W_WIDTH, W_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader->handle);
		shader->setUniform("shadowCalculate", false);
		shader->setUniform("lightView", false);
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
			for (auto o : allObjects)
			{
				//Render the object
				Normal(o);
			}
		}
		bool showLighting = true;
		if (showLighting)
		{
			Lighting(time);
		}

		//sub Screen
		glViewport(0, 0, 250, 250);
		glClear(GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader->handle);
		shader->setUniform("shadowCalculate", true);
		shader->setUniform("lightView", true);
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

		glDisable(GL_DEPTH_TEST);
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

void Level::SetMatrix()
{
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
	V[3][0] = -dot(r, cam.camPos);
	V[3][1] = -dot(up, cam.camPos);
	V[3][2] = -dot(dir, cam.camPos);

	//cam.ViewMat = glm::lookAt(cam.camPos, cam.camTarget, up);
	cam.ViewMat = V;

	//The image is mirrored on X
	cam.ProjMat = glm::perspective(glm::radians(cam.fovy), cam.width / cam.height, cam.nearPlane, cam.farPlane);

	//Calculate Light Matrix
	for (auto lgt : allLights)
	{
		glm::vec3 dir = lgt->dir;
		dir = -dir;
		glm::vec3 r = glm::normalize(glm::cross(glm::vec3(0, 1, 0), dir)); //cam.camUp = 0,1,0
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
		V[3][0] = -dot(r, lgt->pos);
		V[3][1] = -dot(up, lgt->pos);
		V[3][2] = -dot(dir, lgt->pos);
		lgt->lightMat = V;
	}
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
	if (lighting_shader)
		delete lighting_shader;
	if (normal_shader)
		delete normal_shader;
	if (depth_shader)
		delete depth_shader;
	std::stringstream v;
	std::stringstream f;
	std::stringstream dv;
	std::stringstream df;
	std::stringstream lv;
	std::stringstream lf;
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
	file.open("data/shaders/vert_l.vert");
	lv << file.rdbuf();
	file.close();
	file.open("data/shaders/frag_l.frag");
	lf << file.rdbuf();
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
	file.open("data/shaders/vert_d.vert");
	dv << file.rdbuf();
	file.close();
	file.open("data/shaders/frag_d.frag");
	df << file.rdbuf();
	file.close();

	shader = new cg::Program(v.str().c_str(), f.str().c_str());
	depth_shader = new cg::Program(dv.str().c_str(), df.str().c_str());
	lighting_shader = new cg::Program(lv.str().c_str(), lf.str().c_str());
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
	cam.camPos += glm::normalize(cam.camTarget - cam.camPos) * size;
}

void Level::AddObject(Model* model)
{
	allObjects.push_back(model);
}

void Level::AddLights(CS300Parser::Light* light)
{
	allLights.push_back(light);
}

bool Level::Shadow(Model* obj)
{
	glUseProgram(depth_shader->handle);
	depth_shader->setUniform("model", cam.ProjMat * cam.ViewMat * obj->ComputeMatrix());
	glBindFramebuffer(GL_FRAMEBUFFER, obj->depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, obj->depthMap, 0);
	glDrawBuffer(GL_NONE);

	bool res = false;
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		res = true;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	return res;
}

void Level::Render(Model* obj)
{
	glUseProgram(shader->handle);
	//use obj FBO
	glBindBuffer(GL_ARRAY_BUFFER, obj->depthMapFBO);
	//use obj VBO
	glBindBuffer(GL_ARRAY_BUFFER, obj->VBO);
	//use obj VAO
	glBindVertexArray(obj->VAO);

	shader->setUniform("normalMap", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, obj->normal_tex);

	shader->setUniform("tex", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, obj->texobj);

	shader->setUniform("depthMap", 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, obj->depthMap);

	//Send model matrix to the shader
	glm::mat4x4 m2w = obj->ComputeMatrix();

	shader->setUniform("m2w", m2w);
	shader->setUniform("render_mode", render_mode);
	shader->setUniform("cameraPos", cam.camPos);


	//Material
	// << type context here!! (p.s. ns means shiness)
	shader->setUniform("material.emission", obj->material.emission);
	shader->setUniform("material.ambient", obj->material.ambient);
	shader->setUniform("material.diffuse", obj->material.diffuse);
	shader->setUniform("material.specular", obj->material.specular);
	shader->setUniform("material.shininess", obj->material.shininess);


	//draw
	glDrawArrays(GL_TRIANGLES, 0, obj->points.size());
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void Level::Normal(Model* obj)
{
	glUseProgram(normal_shader->handle);
	//use obj VBO
	glBindBuffer(GL_ARRAY_BUFFER, obj->VBO);
	//use obj VAO
	glBindVertexArray(obj->VAO);

	//Send model matrix to the shader
	glm::mat4x4 m2w = obj->ComputeMatrix();

	//Send view matrix to the shader
	normal_shader->setUniform("model", cam.ProjMat * cam.ViewMat * m2w);
	normal_shader->setUniform("m2w", m2w);
	//draw
	glDrawArrays(GL_TRIANGLES, 0, obj->points.size());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Level::Lighting(float time)
{
	//lighting
	struct LightSourceParameters
	{
		int type					= 0;
		glm::vec4 ambient			= glm::vec4(0.0f);
		glm::vec4 diffuse			= glm::vec4(0.0f);
		glm::vec4 specular			= glm::vec4(0.0f);
		glm::vec4 position			= glm::vec4(0.0f);
		glm::vec3 direction			= glm::vec3(0.0f);
		float spotExponent			= 0;
		float spotInner				= 0;
		float spotOuter				= 0;
		float constantAttenuation	= 0;
		float linearAttenuation		= 0;
		float quadraticAttenuation	= 0;
	};

	glUseProgram(shader->handle);
	int i = 0;
	for (auto lgt : allLights)
	{
		glm::vec3 temp = lgt->st_pos;

		//Animation Update
		for (auto anim : lgt->anims)
		{
			temp = anim.Update(temp, time);
		}
		lgt->pos = temp;

		LightSourceParameters light
		{
			lgt->type == "POINT" ? 0 : lgt->type == "SPOT" ? 1 : 2,	//type
			glm::vec4(lgt->amb * lgt->col, 1),						//ambient
			glm::vec4(lgt->col, 1.0f),								//diffuse
			glm::vec4(lgt->col, 1.0f),								//specualr
			glm::vec4(lgt->pos,1.0f),								//position
			lgt->dir,												//Direction (or spotDirection)
			lgt->falloff,											//spotExponent
			lgt->inner,												//spotInner
			lgt->outer,												//spotOuter
			lgt->att.x,												//constantAttenuation
			lgt->att.y,												//linearAttenuation
			lgt->att.z												//quadraticAttenuation
		};
		std::string lightName = "light[" + std::to_string(i++) + "].";
		shader->setUniform(lightName + "type", light.type);
		shader->setUniform(lightName + "ambient", light.ambient);
		shader->setUniform(lightName + "diffuse", light.diffuse);
		shader->setUniform(lightName + "specular", light.specular);
		shader->setUniform(lightName + "position", light.position);
		shader->setUniform(lightName + "direction", light.direction);
		shader->setUniform(lightName + "spotExponent", light.spotExponent);
		shader->setUniform(lightName + "spotInner", light.spotInner);
		shader->setUniform(lightName + "spotOuter", light.spotOuter);
		shader->setUniform(lightName + "constantAttenuation", light.constantAttenuation);
		shader->setUniform(lightName + "linearAttenuation", light.linearAttenuation);
		shader->setUniform(lightName + "quadraticAttenuation", light.quadraticAttenuation);
	}
	shader->setUniform("activeCount", (int)allLights.size());

	//visualize light obj
	glUseProgram(lighting_shader->handle);
	for (auto lgt : allLights)
	{
		//use obj VBO
		glBindBuffer(GL_ARRAY_BUFFER, lgt->lightingVBO);
		//use obj VAO
		glBindVertexArray(lgt->lightingVAO);
		//Send model matrix to the shader
		glm::mat4x4 m2w = lgt->ComputeMatrix();

		//Send view matrix to the shader
		lighting_shader->setUniform("model", cam.ProjMat * cam.ViewMat * m2w);

		glDrawArrays(GL_TRIANGLES, 0, lgt->points.size());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
}


Level::Level() : window(nullptr), shader(nullptr), depth_shader(nullptr), lighting_shader(nullptr), normal_shader(nullptr), render_mode(0)
{

}

Level::~Level()
{
	for (auto m : allObjects)
		delete m;

	allObjects.clear();

	for (auto m : allLights)
		delete m;

	allLights.clear();
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
