#pragma once

#include "CS300Parser.h"
#include "Level.h"

struct Vec3Compare {
	bool operator()(const glm::vec3& a, const glm::vec3& b) const {
		if (a.x != b.x) return a.x < b.x;
		if (a.y != b.y) return a.y < b.y;
		return a.z < b.z;
	}
};

struct Model
{
	struct MaterialParameters
	{
		glm::vec4 emission = glm::vec4(0.0f);
		glm::vec4 ambient = glm::vec4(1.0f);
		glm::vec4 diffuse = glm::vec4(1.0f); //will be texture Color
		glm::vec4 specular = glm::vec4(1.0f);
		float shininess = 1.f; //defualt value is 10
	};
	CS300Parser::Transform transf;
	MaterialParameters material;
	
	glm::mat4x4 ComputeMatrix();

	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UV;
	std::vector<glm::vec3> tangent;

	std::vector<float> vertices;

	std::vector<int> pointIndeces;
	std::vector<int> normalIndeces;
	unsigned int VBO;
	unsigned int VAO;
	unsigned int normal_tex;

	unsigned char* texData;
	unsigned char* norTexData;

	unsigned int texobj;

	void Initialize();

	void LoadModel();
	Model(const CS300Parser::Transform& _transform);
	~Model();

	void SetMaterial();

	void InitVertexArray();
	void CreateTexobj();
	void CreateNorMap();
	void GetNormal(std::vector<glm::vec3>& v, std::vector<int>& vi);
	void GetTangent(std::vector<glm::vec3>& v, std::vector<int>& vi, std::vector<glm::vec2>& uv);
	bool FindVertex(const std::vector<std::vector<glm::vec3>>& vertexNormals, const int ind, const glm::vec3& normal);
	static int slices;

private:
	void InitModel();
	//TODO
	void CreateModelPlane();
	void CreateModelCube();
	void CreateModelCone(int slices);
	void CreateModelCylinder(int slices);
	void CreateModelSphere(int slices);

};