#include "Cube.h"

Cube::Cube(std::vector<float> inputVertices, std::vector<float> inputNormals, std::vector<float> inputTexCoords, const char* texturePathDiffuse, const char* texturePathSpecular
	, Shader& shader) : Object()
{
	m_shader = shader;
	m_hasTexture = true;
	m_useEBO = false;

	//Open and load diffuse map
	if (std::strstr(texturePathDiffuse, ".jpg"))
		diffuseMap = new Texture(texturePathDiffuse, false, GL_RGB);
	else
		diffuseMap = new Texture(texturePathDiffuse, false, GL_RGBA);

	//Open and load specular map
	if (std::strstr(texturePathSpecular, ".jpg"))
		specularMap = new Texture(texturePathSpecular, false, GL_RGB);
	else
		specularMap = new Texture(texturePathSpecular, false, GL_RGBA);

	model = glm::mat4(1.0f);
	model = glm::translate(model, m_Position);

	//Generate VAO and VBO
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	for (int i = 0; i < inputVertices.size(); i++)
	{
		m_Vertices.push_back(inputVertices[i]);
		m_Normals.push_back(inputNormals[i]);
	}

	for (int i = 0; i < inputTexCoords.size(); i++)
	{
		m_TexCoords.push_back(inputTexCoords[i]);
	}

	//Build interleaved Vertices
	buildInterleavedVerticesWithTexCoords();

	//Assign vertices to object
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_InterleavedVertices.size(), m_InterleavedVertices.data(), GL_STATIC_DRAW);

	// Set up vertex attribute pointers (vertices, normals, texcoords)
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(float), (void*)(sizeof(float) * 6));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// unbind VAO and VBOs
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

//Cube with no Textures
Cube::Cube(std::vector<float> inputVertices, std::vector<float> inputNormals, Shader& shader) : Object()
{
	m_shader = shader;
	m_hasTexture = false;
	m_useEBO = false;
	//Generate VAO and VBO
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	for (int i = 0; i < inputVertices.size(); i++)
	{	
		m_Vertices.push_back(inputVertices[i]);
		m_Normals.push_back(inputNormals[i]);
	}

	//Build interleaved Vertices
	buildInterleavedVertices();

	//Generate VAO and VBO
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glBindVertexArray(m_vao);

	//Assign vertices to object
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_InterleavedVertices.size() * sizeof(float), m_InterleavedVertices.data(), GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Normal Attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

}

Cube::~Cube()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}
/*

void Cube::render()
{
	//Bind texture and render to VAO
	if (hasTexture)
	{
		glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture (2 texture in frag shader)
		glBindTexture(GL_TEXTURE_2D, diffuseMap->ID);
		glActiveTexture(GL_TEXTURE1); // activate the texture unit first before binding texture (2 texture in frag shader)
		glBindTexture(GL_TEXTURE_2D, specularMap->ID);

		glUseProgram(shaderID);
		glUniform1i(glGetUniformLocation(shaderID, "hasTexture"), true);
	}
	else
	{
		glUseProgram(shaderID);
		glUniform1i(glGetUniformLocation(shaderID, "hasTexture"), false);
	}

	//Bind
	glBindVertexArray(m_vao);

	//Render the cube
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Unbind buffers and reset state
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

*/