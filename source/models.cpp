#include "models.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>


//#define TINYOBJLOADER_IMPLEMENTATION
#include "../extern/tiny_obj_loader.h"


int Model::slices = 4;

glm::mat4x4 Model::ComputeMatrix()
{
	glm::mat4 translate = glm::mat4(1.0f);
	glm::mat4 scale = glm::mat4(1.0f);

	glm::mat4 rotateYXZ = glm::mat4(1.0f);

	glm::mat4 temp = glm::mat4(1.0f);
	glm::mat4 a = glm::mat4(1.0f);
	glm::mat4 b = glm::mat4(1.0f);
	glm::mat4 c = glm::mat4(1.0f);

	translate = glm::translate(translate, transf.pos);
	scale = glm::scale(scale, transf.sca);

	a = glm::rotate(a, glm::radians(transf.rot.y), { 0,1,0 });
	b = glm::rotate(b, glm::radians(transf.rot.x), { 1,0,0 });
	c = glm::rotate(c, glm::radians(transf.rot.z), { 0,0,1 });

	rotateYXZ = a * b * c;

	temp = translate * rotateYXZ * scale;

	return temp;
}

void Model::LoadModel()
{
	//If exception. use one of our functions
	if (transf.mesh == "PLANE")
		CreateModelPlane();
	else if (transf.mesh == "CUBE")
		CreateModelCube();
	else if (transf.mesh == "SPHERE")
		CreateModelSphere(slices);
	else if (transf.mesh == "CYLINDER")
		CreateModelCylinder(slices);
	else if (transf.mesh == "CONE")
		CreateModelCone(slices);
	else
	{
		//Else, use the general
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string warn;
		std::string err;
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, transf.mesh.c_str());

		std::vector<glm::vec3> temp;
		std::vector<glm::vec3> tempN;
		std::vector<glm::vec2> tempUV;

		//Save mesh points
		for (int i = 0; i < attrib.vertices.size(); i += 3)
		{
			temp.push_back({ attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2] });
		}

		//Save mesh normals
		for (int i = 0; i < attrib.normals.size(); i += 3)
		{
			tempN.push_back({ attrib.normals[i], attrib.normals[i + 1], attrib.normals[i + 2] });
		}

		//Save UV
		for (int i = 0; i < attrib.texcoords.size(); i += 2)
		{
			tempUV.push_back({ attrib.texcoords[i], attrib.texcoords[i + 1]});
		}

		//Load Shapes (vertex indexes)
		for (auto s : shapes)
		{
			for (auto p : s.mesh.indices)
			{
				//Load vertexes
				points.push_back(temp[p.vertex_index]);
				//Load Normals
				normals.push_back(tempN[p.normal_index]);
				//Load Indexes
				UV.push_back(tempUV[p.texcoord_index]);
			}
		}
	}
}

void Model::InitModel()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	points.clear();
	normals.clear();
	UV.clear();
	vertices.clear();
	pointIndeces.clear();
	normalIndeces.clear();

	LoadModel();
	int s = points.size();
	//vertices
	for (int i = 0; i < s; i++)
	{
		//points
		vertices.push_back(points[i].x);
		vertices.push_back(points[i].y);
		vertices.push_back(points[i].z);
		//normals
		vertices.push_back(normals[i].x);
		vertices.push_back(normals[i].y);
		vertices.push_back(normals[i].z);
		//UV
		vertices.push_back(UV[i].x);
		vertices.push_back(UV[i].y);
	}

	//Sanity Check
	if (vertices.size() == 0)
		return;

	//Gen VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * (sizeof(float)), &vertices[0], GL_STATIC_DRAW);

	//Gen VAO

	//Assign Coordinates
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Assign Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Assign UV
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Model::Model(const CS300Parser::Transform& _transform) : transf(_transform), VBO(0), VAO(0)
{
	//load points
	LoadModel();
	
	int s = points.size();
	//vertices
	for (int i = 0; i < s; i++)
	{
		//points
		vertices.push_back(points[i].x);
		vertices.push_back(points[i].y);
		vertices.push_back(points[i].z);
		//normals
		vertices.push_back(normals[i].x);
		vertices.push_back(normals[i].y);
		vertices.push_back(normals[i].z);
		//UV
		vertices.push_back(UV[i].x);
		vertices.push_back(UV[i].y);
	}

	//Sanity Check
	if (vertices.size() == 0)
		return;

	//Gen VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * (sizeof(float)), &vertices[0], GL_STATIC_DRAW);

	//Gen VAO
	
	//Assign Coordinates
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Assign Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	//Assign UV
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

Model::~Model()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

//TODO:
void Model::CreateModelPlane()
{
	//TODO: Points
	//버텍스
	std::vector<float> Plane_Vertex = {
		 0.5f,  0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
	};
	std::vector<float> Plane_Normal = {
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
	};
	std::vector<float> Plane_TexCoord = {
		 1.f,  1.f,
		 1.f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.f
	};
	//인덱스
	std::vector<int> Vertex_Indexs = {
		2, 3, 0,
		2, 1, 0
	};
	std::vector<int> Normal_Indexs = {
		2, 3, 0,
		2, 1, 0
	};
	std::vector<int> Texcoords_Indexs = {
		2, 3, 0,
		2, 1, 0
	};
	//TODO: UVs

	//TODO: Normals

	std::vector<glm::vec3> temp;
	std::vector<glm::vec3> tempN;
	std::vector<glm::vec2> tempUV;

	//Save mesh points
	for (int i = 0; i < Plane_Vertex.size(); i += 3)
	{
		temp.push_back({ Plane_Vertex[i], Plane_Vertex[i + 1], Plane_Vertex[i + 2] });
	}

	//Save mesh normals
	for (int i = 0; i < Plane_Normal.size(); i += 3)
	{
		tempN.push_back({ Plane_Normal[i], Plane_Normal[i + 1], Plane_Normal[i + 2] });
	}

	//Save UV
	for (int i = 0; i < Plane_TexCoord.size(); i += 2)
	{
		tempUV.push_back({ Plane_TexCoord[i], Plane_TexCoord[i + 1] });
	}

	//(vertex indexes)
	for (auto p : Vertex_Indexs)
	{
		//Load vertexes
		points.push_back(temp[p]);
	}
	for (auto p : Normal_Indexs)
	{
		//Load Normals
		normals.push_back(tempN[p]);
	}
	for (auto p : Texcoords_Indexs)
	{
		//Load Indexes
		UV.push_back(tempUV[p]);
	}
}

void Model::CreateModelCube()
{
	//TODO: Points
	//버텍스
	float size = 0.5f;
	std::vector<glm::vec3> Cube_Vertex = {
		glm::vec3(-size, -size, -size),
		glm::vec3(size, -size, -size),
		glm::vec3(size, size, -size),
		glm::vec3(-size, size, -size),
		glm::vec3(-size, -size, size),
		glm::vec3(size, -size, size),
		glm::vec3(size, size, size),
		glm::vec3(-size, size, size),
	};
	std::vector<glm::vec3> Cube_Normal = {
		glm::vec3(0, 0, -1), // 앞
		glm::vec3(0, 0, 1),	 // 뒤
		glm::vec3(-1, 0, 0), // 왼
		glm::vec3(1, 0, 0),	 // 오른
		glm::vec3(0, 1, 0),	 // 위
		glm::vec3(0, -1, 0), // 아래
	};
	std::vector<glm::vec2> Cube_TexCoord = {
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f },
	};

	std::vector<int> Vertex_Indexs
	{
		1, 0, 3, 1, 3, 2,// 뒷면
		4, 5, 6, 4, 6, 7,// 앞면
		0, 4, 7, 0, 7, 3,// 왼쪽
		5, 1, 2, 5, 2, 6,// 오른쪽
		6, 2, 3, 6, 3, 7,// 위
		4, 0, 1, 4, 1, 5// 아래
	};

	//(vertex indexes)
	int i = 0;
	for (auto p : Vertex_Indexs)
	{
		//Load vertexes
		points.push_back(Cube_Vertex[p]);
		normals.push_back(Cube_Normal[int(i / 6)]);
		// 각 면의 UV 좌표 설정
		UV.push_back(Cube_TexCoord[i % 6]);
		i++;
	}
}

void Model::CreateModelCone(int slices)
{
	std::vector<glm::vec3> Cone_Vertex;
	float r = 0.5f;
	for (int i = 0; i < slices; i++) {
		float angle = 2.0f * glm::pi<float>() * i / slices;
		Cone_Vertex.push_back({ r * cos(angle),-0.5f,r * sin(angle) });
	}
	Cone_Vertex.push_back({ 0.0f, -0.5f, 0.0f }); //Bottom
	Cone_Vertex.push_back({ 0.0f,  0.5f, 0.0f }); //Top

	//인덱스
	std::vector<int> Vertex_Indexs;
	for (int i = 0; i < slices*2; i++)
	{
		int prsc = i % slices;
		int ntsc = (i + 1) % slices;
		glm::vec3 tb = Cone_Vertex[int(i / slices) + slices];
		glm::vec3 pres = Cone_Vertex[prsc];
		glm::vec3 next = Cone_Vertex[ntsc];

		points.push_back(pres);
		normals.push_back({glm::cross(glm::vec3(tb - pres),
			glm::vec3(next - pres)) });
		UV.push_back({ float(prsc) / slices, 0 });

		points.push_back(tb);
		normals.push_back({glm::cross(glm::vec3(pres - tb),
			glm::vec3(next - tb)) });
		UV.push_back({(float(prsc) + 0.5f) / slices, int(i / slices) });

		points.push_back(next);
		normals.push_back({ glm::cross(glm::vec3(tb - next),
			glm::vec3(pres - next)) });
		UV.push_back({ float(ntsc) / slices, 0 });
	}
}

void Model::CreateModelCylinder(int slices)
{
	std::vector<glm::vec3> Cylinder_Vertex;
	float r = 0.5f;
	for (int i = 0; i < slices * 2; i++) {
		float angle = 2.0f * glm::pi<float>() * (i % slices) / slices;
		Cylinder_Vertex.push_back({ r * cos(angle),	int(i /slices) - 0.5f	,r * sin(angle)});
	}
	Cylinder_Vertex.push_back({ 0.0f, -0.5f, 0.0f }); //Bottom
	Cylinder_Vertex.push_back({ 0.0f,  0.5f, 0.0f }); //Top

	//인덱스
	std::vector<int> Vertex_Indexs;
	for (int i = 0; i < slices * 2; i++)
	{
		int prscuv = i % slices;
		int ntscuv = (i + 1) % slices;
		int prsc = i % slices + int(i/slices) * slices;
		int ntsc = (i + 1) % slices + int(i / slices) * slices;
		glm::vec3 tb = Cylinder_Vertex[int(i / slices) + slices*2];
		glm::vec3 pres = Cylinder_Vertex[prsc];
		glm::vec3 next = Cylinder_Vertex[ntsc];

		points.push_back(pres);
		normals.push_back({ glm::cross(glm::vec3(tb - pres),
			glm::vec3(next - pres)) });
		UV.push_back({ float(prscuv) / slices, int(i / slices) });

		points.push_back(tb);
		normals.push_back({ glm::cross(glm::vec3(pres - tb),
			glm::vec3(next - tb)) });
		UV.push_back({ float(prscuv) / slices, int(i / slices) });

		points.push_back(next);
		normals.push_back({ glm::cross(glm::vec3(tb - next),
			glm::vec3(pres - next)) });
		UV.push_back({ float(ntscuv) / slices, int(i / slices) });

		points.push_back(pres);
		normals.push_back({ glm::cross(glm::vec3(tb - pres),
			glm::vec3(next - pres)) });
		UV.push_back({ float(prscuv) / slices, int(i / slices) });

		points.push_back(Cylinder_Vertex[int(i / slices) == 0 ? prsc + slices : (i - slices + 1) % slices]);
		normals.push_back({ glm::cross(glm::vec3(pres - tb),
			glm::vec3(next - tb)) });
		UV.push_back({ (int(i / slices) == 0 ? float(prscuv): float(ntscuv)) / slices, int(i / slices) == 0 ? 1 : 0 });

		points.push_back(next);
		normals.push_back({ glm::cross(glm::vec3(tb - next),
			glm::vec3(pres - next)) });
		UV.push_back({ float(ntscuv) / slices, int(i / slices) });
	}
}

void Model::CreateModelSphere(int slices)
{
	//TODO: Points

	//TODO: UVs

	//TODO: Normals
}
