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
	//���ؽ�
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
	//�ε���
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
	//���ؽ�
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
		glm::vec3(0, 0, -1), // ��
		glm::vec3(0, 0, 1),	 // ��
		glm::vec3(-1, 0, 0), // ��
		glm::vec3(1, 0, 0),	 // ����
		glm::vec3(0, 1, 0),	 // ��
		glm::vec3(0, -1, 0), // �Ʒ�
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
		1, 0, 3, 1, 3, 2,// �޸�
		4, 5, 6, 4, 6, 7,// �ո�
		0, 4, 7, 0, 7, 3,// ����
		5, 1, 2, 5, 2, 6,// ������
		6, 2, 3, 6, 3, 7,// ��
		4, 0, 1, 4, 1, 5// �Ʒ�
	};

	//(vertex indexes)
	int i = 0;
	for (auto p : Vertex_Indexs)
	{
		//Load vertexes
		points.push_back(Cube_Vertex[p]);
		normals.push_back(Cube_Normal[int(i / 6)]);
		// �� ���� UV ��ǥ ����
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

	//�ε���
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
	//TODO: Points
	//���ؽ�
	std::vector<float> Cylinder_Vertex =
	{
		0.0f, 0.5f, 0.0f
	};
	float r = 0.5f;
	for (int i = 0; i < slices; i++) {
		float angle = 2.0f * glm::pi<float>() * i / slices;
		Cylinder_Vertex.push_back(r * cos(angle));
		Cylinder_Vertex.push_back(0.5f);
		Cylinder_Vertex.push_back(r * sin(angle));
	}
	for (int i = 0; i < slices; i++) {
		float angle = 2.0f * glm::pi<float>() * i / slices;
		Cylinder_Vertex.push_back(r * cos(angle));
		Cylinder_Vertex.push_back(-0.5f);
		Cylinder_Vertex.push_back(r * sin(angle));
	}
	Cylinder_Vertex.push_back(0.0f);
	Cylinder_Vertex.push_back(-0.5f);
	Cylinder_Vertex.push_back(0.0f);

	std::vector<float> Cylinder_Normal;
	for (int i = 0; i < (slices + 1) * 2; i++) {
		Cylinder_Normal.push_back(0.0f);
		Cylinder_Normal.push_back(0.0f);
		Cylinder_Normal.push_back(1.0f);
	}
	std::vector<float> Cylinder_TexCoord;
	// ���� �߽��� �߰� (UV: �߽�)
	Cylinder_TexCoord.push_back(0.5f); // U
	Cylinder_TexCoord.push_back(0.5f); // V

	// ���� �ֺ� �� (UV: ���� �߽��� �������� ��ġ)
	for (int i = 0; i < slices; i++) {
		float angle = 2.0f * glm::pi<float>() * i / slices;
		float u = 0.5f + 0.5f * cos(angle); // U
		float v = 0.5f + 0.5f * sin(angle); // V
		Cylinder_TexCoord.push_back(u);
		Cylinder_TexCoord.push_back(v);
	}

	// ���� (UV: ������ �� ���簢�� ���·� ����)
	for (int i = 0; i < slices; i++) {
		float angle = 2.0f * glm::pi<float>() * i / slices;
		float u = static_cast<float>(i) / slices; // U: ������ �������� �յ� ��ġ

		// ���� ���� UV
		Cylinder_TexCoord.push_back(u);    // U
		Cylinder_TexCoord.push_back(1.0f); // V: ����

		// �Ʒ��� ���� UV
		Cylinder_TexCoord.push_back(u);    // U
		Cylinder_TexCoord.push_back(0.0f); // V: �Ʒ���
	}

	// �Ʒ��� �߽��� �߰� (UV: �߽�)
	Cylinder_TexCoord.push_back(0.5f); // U
	Cylinder_TexCoord.push_back(0.5f); // V

	// �Ʒ��� �ֺ� �� (UV: ���� �߽��� �������� ��ġ)
	for (int i = 0; i < slices; i++) {
		float angle = 2.0f * glm::pi<float>() * i / slices;
		float u = 0.5f + 0.5f * cos(angle); // U
		float v = 0.5f + 0.5f * sin(angle); // V
		Cylinder_TexCoord.push_back(u);
		Cylinder_TexCoord.push_back(v);
	}
	//�ε���
	std::vector<int> Vertex_Indexs;
	for (int i = 0; i < slices; i++) {
		int next = (i + 1) % slices; // ������ ���� ������ ù ��° ��������
		Vertex_Indexs.push_back(0);  // ������
		Vertex_Indexs.push_back(1 + i); // ���� �ظ� ����
		Vertex_Indexs.push_back(1 + next); // ���� �ظ� ����
	}
	for (int i = 0; i < slices; i++) {
		int next = (i + 1) % slices; // ������ ���� ������ ù ��° ��������
		Vertex_Indexs.push_back(i+1);  // ������
		Vertex_Indexs.push_back(i + 1 + slices); // ���� �ظ� ����
		Vertex_Indexs.push_back(next + 1 + slices); // ���� �ظ� ����
		Vertex_Indexs.push_back(i + 1);  // ������
		Vertex_Indexs.push_back(next + 1); // ���� �ظ� ����
		Vertex_Indexs.push_back(next + 1 + slices); // ���� �ظ� ����
	}
	for (int i = slices; i < slices*2; i++) {
		int next = (i + 1) % (slices)+slices; // ������ ���� ������ ù ��° ��������
		Vertex_Indexs.push_back(slices*2 + 1);      // �ظ� ù ����
		Vertex_Indexs.push_back(1 + i);  // ���� �ظ� ����
		Vertex_Indexs.push_back(1 + next); // ���� �ظ� ����
	}

	std::vector<int> Normal_Indexs;
	for (int i = 0; i < slices; i++) {
		int next = (i + 1) % slices; // ������ ���� ������ ù ��° ��������
		Normal_Indexs.push_back(0);  // ������
		Normal_Indexs.push_back(1 + i); // ���� �ظ� ����
		Normal_Indexs.push_back(1 + next); // ���� �ظ� ����
	}
	for (int i = 0; i < slices; i++) {
		int next = (i + 1) % slices; // ������ ���� ������ ù ��° ��������
		Normal_Indexs.push_back(i + 1);  // ������
		Normal_Indexs.push_back(i + 1 + slices); // ���� �ظ� ����
		Normal_Indexs.push_back(next + 1 + slices); // ���� �ظ� ����
		Normal_Indexs.push_back(i + 1);  // ������
		Normal_Indexs.push_back(next + 1); // ���� �ظ� ����
		Normal_Indexs.push_back(next + 1 + slices); // ���� �ظ� ����
	}
	for (int i = slices; i < slices * 2; i++) {
		int next = (i + 1) % (slices)+slices; // ������ ���� ������ ù ��° ��������
		Normal_Indexs.push_back(slices * 2 + 1);      // �ظ� ù ����
		Normal_Indexs.push_back(1 + i);  // ���� �ظ� ����
		Normal_Indexs.push_back(1 + next); // ���� �ظ� ����
	}
	std::vector<int> Texcoords_Indexs;
	for (int i = 0; i < slices; i++) {
		int next = (i + 1) % slices; // ������ ���� ������ ù ��° ��������
		Texcoords_Indexs.push_back(0);  // ������
		Texcoords_Indexs.push_back(1 + i); // ���� �ظ� ����
		Texcoords_Indexs.push_back(1 + next); // ���� �ظ� ����
	}
	for (int i = 0; i < slices; i++) {
		int next = (i + 1) % slices; // ������ ���� ������ ù ��° ��������
		Texcoords_Indexs.push_back(i + 1);  // ������
		Texcoords_Indexs.push_back(i + 1 + slices); // ���� �ظ� ����
		Texcoords_Indexs.push_back(next + 1 + slices); // ���� �ظ� ����
		Texcoords_Indexs.push_back(i + 1);  // ������
		Texcoords_Indexs.push_back(next + 1); // ���� �ظ� ����
		Texcoords_Indexs.push_back(next + 1 + slices); // ���� �ظ� ����
	}
	for (int i = slices; i < slices * 2; i++) {
		int next = (i + 1) % (slices) + slices; // ������ ���� ������ ù ��° ��������
		Texcoords_Indexs.push_back(slices * 2 + 1);      // �ظ� ù ����
		Texcoords_Indexs.push_back(1 + i);  // ���� �ظ� ����
		Texcoords_Indexs.push_back(1 + next); // ���� �ظ� ����
	}
	//TODO: UVs

	//TODO: Normals

	std::vector<glm::vec3> temp;
	std::vector<glm::vec3> tempN;
	std::vector<glm::vec2> tempUV;

	//Save mesh points
	for (int i = 0; i < Cylinder_Vertex.size(); i += 3)
	{
		temp.push_back({ Cylinder_Vertex[i], Cylinder_Vertex[i + 1], Cylinder_Vertex[i + 2] });
	}

	//Save mesh normals
	for (int i = 0; i < Cylinder_Normal.size(); i += 3)
	{
		tempN.push_back({ Cylinder_Normal[i], Cylinder_Normal[i + 1], Cylinder_Normal[i + 2] });
	}

	//Save UV
	for (int i = 0; i + 1 < Cylinder_TexCoord.size(); i += 2)
	{
		tempUV.push_back({ Cylinder_TexCoord[i], Cylinder_TexCoord[i + 1] });
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

void Model::CreateModelSphere(int slices)
{
	//TODO: Points

	//TODO: UVs

	//TODO: Normals
}
