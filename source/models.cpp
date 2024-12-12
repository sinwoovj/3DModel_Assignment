#include "models.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include <set>


//#define TINYOBJLOADER_IMPLEMENTATION
#include "../extern/tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <numeric>


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

void Model::Initialize()
{
	//load points
	InitModel();

	SetMaterial();

	CreateTexobj();

	CreateNorMap();

	CreateShadow();
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
			std::vector<int> tempIndexes;
			for (auto p : s.mesh.indices)
			{
				//Load vertexes
				points.push_back(temp[p.vertex_index]);
				tempIndexes.push_back(p.vertex_index);
				//Load Normals
				normals.push_back(tempN[p.normal_index]);
				//Load Indexes
				UV.push_back(tempUV[p.texcoord_index]);
			}
			
			GetTangent(temp, tempIndexes,UV);
		}

	}
}

void Model::InitModel()
{
	std::string meshtype = transf.mesh;
	
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteTextures(1, &normal_tex);
	glDeleteTextures(1, &texobj);


	points.clear();
	normals.clear();
	UV.clear();
	tangent.clear();
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
		//Tansent
		vertices.push_back(tangent[i].x);
		vertices.push_back(tangent[i].y);
		vertices.push_back(tangent[i].z);
	}

	//Sanity Check
	if (vertices.size() == 0)
		return;

	//Gen FBO
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	//Gen VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * (sizeof(float)), &vertices[0], GL_STATIC_DRAW);

	//Gen VAO

	//Assign Coordinates
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Assign Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Assign UV
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//Tangent
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

Model::Model(const CS300Parser::Transform& _transform) : transf(_transform), VBO(0), VAO(0), depthMapFBO(0), normal_tex(0), texData(nullptr), norTexData(nullptr)
{
	Initialize();
}

Model::~Model()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteFramebuffers(1, &depthMapFBO);
	glDeleteTextures(1, &normal_tex);
	glDeleteTextures(1, &depthMap);
	glDeleteTextures(1, &texobj);
	texData = nullptr;
	norTexData = nullptr;
}

void Model::SetMaterial()
{
	material.shininess = transf.ns;
}

void Model::InitVertexArray()
{
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

		//Tansent
		vertices.push_back(tangent[i].x);
		vertices.push_back(tangent[i].y);
		vertices.push_back(tangent[i].z);
	}

	//Sanity Check
	if (vertices.size() == 0)
		return;

	//Gen FBO
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	//Gen VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * (sizeof(float)), &vertices[0], GL_STATIC_DRAW);

	//Gen VAO

	//Assign Coordinates
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Assign Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Assign UV
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//Assign Tangent
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Model::CreateTexobj()
{
	const int width = 6;
	const int height = 6;

	glm::vec3 colors[] = {
		glm::vec3(0, 0, 1),   // Blue
		glm::vec3(0, 1, 1),   // Cyan
		glm::vec3(0, 1, 0),   // Green
		glm::vec3(1, 1, 0),   // Yellow
		glm::vec3(1, 0, 0),   // Red
		glm::vec3(1, 0, 1)    // Purple
	};

	texData = new unsigned char[width * height * 3 * 4];
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int colorIdx = (x + (height - 1 - y)) % 6;

			for (int py = 0; py < 2; py++)
			{
				for (int px = 0; px < 2; px++)
				{
					int idx = ((y * 2 + py) * width * 2 + (x * 2 + px)) * 3;
					texData[idx] = colors[colorIdx].r * 255;
					texData[idx + 1] = colors[colorIdx].g * 255;
					texData[idx + 2] = colors[colorIdx].b * 255;
				}
			}
		}
	}

	glGenTextures(1, &texobj);
	glBindTexture(GL_TEXTURE_2D, texobj);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width * 2, height * 2, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Model::CreateShadow()
{
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Model::CreateNorMap()
{
	// 텍스처 로드 및 생성
	int width, height, nrChannels;

	norTexData = stbi_load(transf.normalMap.c_str(), &width, &height, &nrChannels, 0);
	if (stbi_failure_reason())
		std::cout << stbi_failure_reason() << std::endl;
	
	GLuint format = GL_RGB;
	if(nrChannels > 3)
		format = GL_RGBA;

	//Normal Texture
	glGenTextures(1, &normal_tex);
	glBindTexture(GL_TEXTURE_2D, normal_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, nrChannels, width, height, 0, format, GL_UNSIGNED_BYTE, norTexData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Model::GetNormal(std::vector<glm::vec3>& v, std::vector<int>& vi)
{
	std::vector<glm::vec3> tempN;

	for (int i = 0; i < vi.size(); i += 3)
	{
		glm::vec3 v1 = v[vi[i]];
		glm::vec3 v2 = v[vi[i + 1]];
		glm::vec3 v3 = v[vi[i + 2]];

		glm::vec3 normal = glm::normalize(glm::cross(v1 - v2, v2 - v3));

		for (int j = 0; j < 3; j++)
		{
			tempN.push_back(normal);
		}
	}

	if (Level::GetPtr()->normalAvg)
	{
		std::vector<std::set<glm::vec3, Vec3Compare>> vertexNSet(v.size());

		for (int i = 0; i < vi.size(); i++)
		{
			vertexNSet[vi[i]].insert(tempN[i]);
		}

		for (int i = 0; i < vi.size(); i++)
		{
			normals.push_back(glm::normalize(std::accumulate(vertexNSet[vi[i]].begin(), vertexNSet[vi[i]].end(), glm::vec3(0.0f)) / (float)vertexNSet[vi[i]].size()));
		}
	}
	else
	{
		normals = tempN;
	}
}

void Model::GetTangent(std::vector<glm::vec3>& v, std::vector<int>& vi, std::vector<glm::vec2>& uv) //vertex, vertex indices, UV
{
	std::vector<glm::vec3> tempT;

	//Save Tangent
	for (int i = 0; i < vi.size(); i+=3)
	{
		//POINT
		glm::vec3 p1 = v[vi[i]];
		glm::vec3 p2 = v[vi[i + 1]];
		glm::vec3 p3 = v[vi[i + 2]];
		glm::vec2 A = uv[vi[i]];
		glm::vec2 B = uv[vi[i + 1]];
		glm::vec2 C = uv[vi[i + 2]];

		glm::vec3 V1 = p2 - p1;
		glm::vec3 V2 = p3 - p1;
		glm::vec2 Tc1 = B - A;
		glm::vec2 Tc2 = C - A;
		float d = Tc1.y * Tc2.x - Tc2.y * Tc1.x;
		glm::vec3 T = glm::vec3((Tc1.y * V2 - Tc2.y * V1) / (d == 0 ? 1 : d) );
		tempT.push_back(T);
		tempT.push_back(T);
		tempT.push_back(T);
	}
	if (Level::GetPtr()->normalAvg) //when make to average value
	{
		{
			std::vector<std::vector<glm::vec3>> vertexNormals(v.size());
			std::vector<glm::vec3> vertexNSum(v.size());
			std::vector<int> vertexNCount(v.size());

			for (int i = 0; i < vi.size(); i++)
			{
				if (!FindVertex(vertexNormals, vi[i], tempT[i]))
				{
					vertexNormals[vi[i]].push_back(tempT[i]);
					vertexNSum[vi[i]] += tempT[i];
					vertexNCount[vi[i]]++;
				}
			}

			for (int i = 0; i < vi.size(); i++)
			{
				if (glm::length(vertexNSum[vi[i]]) == 0.0f)
					tangent.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
				else
					tangent.push_back(glm::normalize(vertexNSum[vi[i]] / (float)vertexNCount[vi[i]]));
			}
		}
	}
	else
	{
		tangent = tempT;
	}
}
bool Model::FindVertex(const std::vector<std::vector<glm::vec3>>& vertexNormals, const int ind, const glm::vec3& normal)
{
	for (auto n : vertexNormals[ind])
	{
		if (n == normal)
			return true;
	}

	return false;
}
////TODO:
//void Model::CreateModelPlane()
//{
//	//TODO: Points
//	//버텍스
//	std::vector<float> Plane_Vertex = {
//		-0.5f, -0.5f, 0.0f,
//		-0.5f,  0.5f, 0.0f,
//		 0.5f,  0.5f, 0.0f,
//		 0.5f, -0.5f, 0.0f,
//	};
//	std::vector<float> Plane_Normal = {
//		0.0f, 0.0f, 1.0f,
//		0.0f, 0.0f, 1.0f,
//		0.0f, 0.0f, 1.0f,
//		0.0f, 0.0f, 1.0f,
//	};
//	std::vector<float> Plane_TexCoord = {
//		0.0f, 0.0f,
//		0.0f, 1.0f,
//		1.0f, 1.0f,
//		1.0f, 0.0f,
//	};
//	//인덱스
//	std::vector<int> Vertex_Indexs = {
//		2, 1, 0,
//		2, 0, 3
//	};
//	std::vector<int> Normal_Indexs = {
//		2, 1, 0,
//		2, 0, 3
//	};
//	std::vector<int> Texcoords_Indexs = {
//		2, 1, 0,
//		2, 0, 3
//	};
//	//TODO: UVs
//
//	//TODO: Normals
//
//	std::vector<glm::vec3> temp;
//	std::vector<glm::vec3> tempN;
//	std::vector<glm::vec2> tempUV;
//
//	//Save mesh points
//	for (int i = 0; i < Plane_Vertex.size(); i += 3)
//	{
//		temp.push_back({ Plane_Vertex[i], Plane_Vertex[i + 1], Plane_Vertex[i + 2] });
//	}
//
//	//Save mesh normals
//	for (int i = 0; i < Plane_Normal.size(); i += 3)
//	{
//		tempN.push_back({ Plane_Normal[i], Plane_Normal[i + 1], Plane_Normal[i + 2] });
//	}
//
//	//Save UV
//	for (int i = 0; i < Plane_TexCoord.size(); i += 2)
//	{
//		tempUV.push_back({ Plane_TexCoord[i], Plane_TexCoord[i + 1] });
//	}
//
//	//(vertex indexes)
//	for (auto p : Vertex_Indexs)
//	{
//		//Load vertexes
//		points.push_back(temp[p]);
//	}
//	for (auto p : Normal_Indexs)
//	{
//		//Load Normals
//		normals.push_back(tempN[p]);
//	}
//	for (auto p : Texcoords_Indexs)
//	{
//		//Load Indexes
//		UV.push_back(tempUV[p]);
//	}
//
//	//GetTangent(temp, Vertex_Indexs, UV);
//	GetTangent(points, Vertex_Indexs, UV);
//}
void Model::GetVertexAttr(const std::vector<glm::vec3>& vertices, const glm::vec2* vertexUVs, const int vertexIndices[3], const int uvIndices[3])
{
	glm::vec3 v0 = vertices[vertexIndices[0]];
	glm::vec3 v1 = vertices[vertexIndices[1]];
	glm::vec3 v2 = vertices[vertexIndices[2]];

	glm::vec2 uv0 = vertexUVs[uvIndices[0]];
	glm::vec2 uv1 = vertexUVs[uvIndices[1]];
	glm::vec2 uv2 = vertexUVs[uvIndices[2]];

	glm::vec3 normal = glm::normalize(glm::cross(v0 - v1, v1 - v2));

	glm::vec3 edge1 = v1 - v0;
	glm::vec3 edge2 = v2 - v0;
	glm::vec2 deltaUV1 = uv1 - uv0;
	glm::vec2 deltaUV2 = uv2 - uv0;
	glm::vec3 t = (deltaUV2.y * edge1 - deltaUV1.y * edge2) / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	for (int i = 0; i < 3; i++)
	{
		points.push_back(vertices[vertexIndices[i]]);
		UV.push_back(vertexUVs[uvIndices[i]]);
		normals.push_back(normal);
		tangent.push_back(t);

		//allVertexIndices.push_back(vertexIndices[i]);
	}
}

//TODO:
void Model::CreateModelPlane()
{
	std::vector<glm::vec3> vertices =
	{
		glm::vec3(-0.5f, -0.5f, 0.0f),
		glm::vec3(0.5f, -0.5f, 0.0f),
		glm::vec3(0.5f, 0.5f, 0.0f),
		glm::vec3(-0.5f, 0.5f, 0.0f)
	};

	glm::vec2 uv[4] =
	{
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
	};

	{
		int ind[3] = { 0, 1, 2 };
		GetVertexAttr(vertices, uv, ind, ind);
	}

	{
		int ind[3] = { 0, 2, 3 };
		GetVertexAttr(vertices, uv, ind, ind);
	}
}
void Model::CreateModelCube()
{
	Level* ptr = Level::GetPtr();
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
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),

		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),

		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
	};

	std::vector<int> Vertex_Indexs
	{
		1, 0, 3, 1, 3, 2,// 뒷면
		4, 5, 6, 4, 6, 7,// 앞면
		3, 0, 4, 3, 4, 7,// 왼쪽
		1, 2, 6, 1, 6, 5,// 오른쪽
		6, 2, 3, 6, 3, 7,// 위
		4, 0, 1, 4, 1, 5// 아래
	};
	 
	//(vertex indexes)
	int i = 0;
	for (auto p : Vertex_Indexs)
	{
		//Load vertexes
		points.push_back(Cube_Vertex[p]);
		// 각 면의 UV 좌표 설정
		int coord = i % 6;
		if (i >= 6 * 2 && i < 6 * 3)
			coord = i % 6 + 6;
		else if (i >= 6 * 3 && i < 6 * 4)
			coord = i % 6 + 6 * 2;
		UV.push_back(Cube_TexCoord[coord]);
		i++;
	}

	GetNormal(Cube_Vertex, Vertex_Indexs);
	GetTangent(Cube_Vertex, Vertex_Indexs, UV);
}

void Model::CreateModelCone(int slices)
{
	Level* ptr = Level::GetPtr();
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

		
		if (!int(i / slices))
		{
			points.push_back(pres);
			points.push_back(next);
			points.push_back(tb);
			UV.push_back({ float(prsc) / slices, 0 });
			UV.push_back({ float(ntsc) / slices, 0 });
			UV.push_back({ (float(prsc) + 0.5f) / slices, int(i / slices) });
			Vertex_Indexs.push_back(prsc);
			Vertex_Indexs.push_back(ntsc);
			Vertex_Indexs.push_back(int(i / slices) + slices);
		}
		else
		{
			points.push_back(pres);
			points.push_back(tb);
			points.push_back(next);
			UV.push_back({ float(prsc) / slices, 0 });
			UV.push_back({ (float(prsc) + 0.5f) / slices, int(i / slices) });
			UV.push_back({ float(ntsc) / slices, 0 });
			Vertex_Indexs.push_back(prsc);
			Vertex_Indexs.push_back(int(i / slices) + slices);
			Vertex_Indexs.push_back(ntsc);
		}
	}
	
	GetNormal(Cone_Vertex, Vertex_Indexs);
	GetTangent(Cone_Vertex, Vertex_Indexs, UV);
}

void Model::CreateModelCylinder(int slices)
{
	Level* ptr = Level::GetPtr();
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

		if (!int(i / slices))
		{
			points.push_back(pres);
			points.push_back(next);
			points.push_back(tb);
			points.push_back(pres);
			points.push_back(Cylinder_Vertex[int(i / slices) == 0 ? prsc + slices : (i - slices + 1) % slices]);
			points.push_back(next);
			UV.push_back({ float(prscuv) / slices, int(i / slices) });
			UV.push_back({ float(ntscuv) / slices, int(i / slices) });
			UV.push_back({ float(prscuv) / slices, int(i / slices) });
			UV.push_back({ float(prscuv) / slices, int(i / slices) });
			UV.push_back({ (int(i / slices) == 0 ? float(prscuv) : float(ntscuv)) / slices, int(i / slices) == 0 ? 1 : 0 });
			UV.push_back({ float(ntscuv) / slices, int(i / slices) });
			Vertex_Indexs.push_back(prsc);
			Vertex_Indexs.push_back(ntsc);
			Vertex_Indexs.push_back(int(i / slices) + slices * 2);
			Vertex_Indexs.push_back(prsc);
			Vertex_Indexs.push_back(int(i / slices) == 0 ? prsc + slices : (i - slices + 1) % slices);
			Vertex_Indexs.push_back(ntsc);
		}
		else
		{
			points.push_back(pres);
			points.push_back(tb);
			points.push_back(next);
			points.push_back(pres);
			points.push_back(next);
			points.push_back(Cylinder_Vertex[int(i / slices) == 0 ? prsc + slices : (i - slices + 1) % slices]);
			UV.push_back({ float(prscuv) / slices, int(i / slices) });
			UV.push_back({ float(prscuv) / slices, int(i / slices) });
			UV.push_back({ float(ntscuv) / slices, int(i / slices) });
			UV.push_back({ float(prscuv) / slices, int(i / slices) });
			UV.push_back({ float(ntscuv) / slices, int(i / slices) });
			UV.push_back({ (int(i / slices) == 0 ? float(prscuv) : float(ntscuv)) / slices, int(i / slices) == 0 ? 1 : 0 });
			Vertex_Indexs.push_back(prsc);
			Vertex_Indexs.push_back(int(i / slices) + slices * 2);
			Vertex_Indexs.push_back(ntsc);
			Vertex_Indexs.push_back(prsc);
			Vertex_Indexs.push_back(ntsc);
			Vertex_Indexs.push_back(int(i / slices) == 0 ? prsc + slices : (i - slices + 1) % slices);
		}
	}
	GetNormal(Cylinder_Vertex, Vertex_Indexs);

	GetTangent(Cylinder_Vertex, Vertex_Indexs, UV);
}

void Model::CreateModelSphere(int slices)
{
	float height = 1.0f;

	std::vector<glm::vec3> temp = {
		glm::vec3(0.0f, height / 2, 0.0f), // top
		glm::vec3(0.0f, -height / 2, 0.0f), // bottom
	};

	std::vector<int> indices;

	constexpr float pi = glm::pi<float>();
	float angleStep = 2 * pi / slices;
	float fslices = (float)slices;

	for (int i = 1; i < (slices / 2); i++)
	{
		float yPos = 0.5f * sinf((i * pi / (slices / 2)) - (pi / 2));
		float befYPos = 0.5f * sinf(((i - 1) * pi / (slices / 2)) - (pi / 2));
		float radius = sqrtf((0.5f * 0.5f) - (yPos * yPos));

		for (int j = 0; j <= slices; j++)
		{
			if (j != slices)
				temp.push_back(glm::vec3(radius * cosf(j * angleStep), yPos, radius * sinf(j * angleStep)));

			if (j != 0)
			{
				if (i == 1) // triangle
				{
					indices.push_back(1);
					indices.push_back(j + 1);
					indices.push_back(j != slices ? j + 2 : 2);

					//UV.push_back(glm::vec2((j - 1) / fslices, 0.0f));
					//UV.push_back(glm::vec2((j - 0.5) / fslices, yPos + 0.5f));
					UV.push_back(glm::vec2((j - 0.5f) / fslices, 0.0f));
					UV.push_back(glm::vec2((j - 1) / fslices, (i - 0.0f) / (slices / 2)));
					UV.push_back(glm::vec2(j / fslices, (i - 0.0f) / (slices / 2)));
				}

				else
				{
					indices.push_back((i - 1) * slices + (j != slices ? j + 2 : 2));
					indices.push_back((i - 2) * slices + j + 1);
					indices.push_back((i - 1) * slices + j + 1);

					UV.push_back(glm::vec2(j / fslices, (i - 0.0f) / (slices / 2)));
					UV.push_back(glm::vec2((j - 1) / fslices, (i - 1.0f) / (slices / 2)));
					UV.push_back(glm::vec2((j - 1) / fslices, (i - 0.0f) / (slices / 2)));

					indices.push_back((i - 2) * slices + j + 1);
					indices.push_back((i - 1) * slices + (j != slices ? j + 2 : 2));
					indices.push_back((i - 2) * slices + (j != slices ? j + 2 : 2));

					UV.push_back(glm::vec2((j - 1) / fslices, (i - 1.0f) / (slices / 2)));
					UV.push_back(glm::vec2(j / fslices, (i - 0.0f) / (slices / 2)));
					UV.push_back(glm::vec2(j / fslices, (i - 1.0f) / (slices / 2)));
				}

				if (i == (slices / 2) - 1)
				{
					indices.push_back(0);
					indices.push_back((i - 1) * slices + (j != slices ? j + 2 : 2));
					indices.push_back((i - 1) * slices + j + 1);

					UV.push_back(glm::vec2((j - 0.5f) / fslices, 1.f));
					UV.push_back(glm::vec2(j / fslices, (i - 0.0f) / (slices / 2)));
					UV.push_back(glm::vec2((j - 1) / fslices, (i - 0.0f) / (slices / 2)));
				}
			}
		}
	}


	for (int i = 0; i < indices.size(); i++)
	{
		points.push_back(temp[indices[i]]);
	}

	GetNormal(temp, indices);
	GetTangent(temp, indices, UV);
}
