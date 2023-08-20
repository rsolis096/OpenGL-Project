#include "Cube.h"

Cube::Cube(float inputVertices[], float inputNormals[], float inputTexCoords[], const char* texturePath, unsigned int sID)
{

	for (int i = 0; i < 108; i++)
	{
		vertices.push_back(inputVertices[i]);
		normals.push_back(inputNormals[i]);
	}

	for (int i = 0; i < 72 ; i++)
	{
		texCoords.push_back(inputTexCoords[i]);
	}
	for (int i = 0; i < texCoords.size(); i += 2)
	{
		std::cout << texCoords[i] << ", " << 
		texCoords[i + 1] << std::endl;
	}

	//Build interleaved Vertices
	buildInterleavedVerticesWithTexCoords();

	shaderID = sID;
	hasTexture = true;
	texture = new Texture(texturePath, false, GL_RGB);
	//Generate VAO and VBO
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	//Assign vertices to object
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * interleavedVertices.size(), interleavedVertices.data(), GL_STATIC_DRAW);

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

Cube::Cube(float inputVertices[], float inputNormals[], unsigned int sID)
{
	for (int i = 0; i < 108; i++)
	{	
		vertices.push_back(inputVertices[i]);
		normals.push_back(inputNormals[i]);
	}

	//Build interleaved Vertices
	buildInterleavedVertices();

	shaderID = sID;
	hasTexture = false;
	//Generate VAO and VBO
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glBindVertexArray(m_vao);

	//Assign vertices to object
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, interleavedVertices.size() * sizeof(float), interleavedVertices.data(), GL_STATIC_DRAW);

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

void Cube::render()
{
	//Bind texture and render to VAO
	if (hasTexture)
	{
		glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture (2 texture in frag shader)
		glBindTexture(GL_TEXTURE_2D, texture->ID);

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

void Cube::bind()
{
	glBindVertexArray(m_vao);
}

void Cube::buildInterleavedVertices()
{
	std::vector<float>().swap(interleavedVertices);

	std::size_t i, j;
	std::size_t count = vertices.size();
	for (i = 0, j = 0; i < count; i += 3)
	{
		interleavedVertices.push_back(vertices[i]);
		interleavedVertices.push_back(vertices[i + 1]);
		interleavedVertices.push_back(vertices[i + 2]);


		interleavedVertices.push_back(normals[i]);
		interleavedVertices.push_back(normals[i + 1]);
		interleavedVertices.push_back(normals[i + 2]);

	}
}

void Cube::buildInterleavedVerticesWithTexCoords()
{
	std::vector<float>().swap(interleavedVertices);

	std::size_t i, j;
	std::size_t count = vertices.size();
	for (i = 0, j = 0; i < count; i += 3, j+=2)
	{
		interleavedVertices.push_back(vertices[i]);
		interleavedVertices.push_back(vertices[i + 1]);
		interleavedVertices.push_back(vertices[i + 2]);

		interleavedVertices.push_back(normals[i]);
		interleavedVertices.push_back(normals[i + 1]);
		interleavedVertices.push_back(normals[i + 2]);

		interleavedVertices.push_back(texCoords[j]);
		interleavedVertices.push_back(texCoords[j + 1]);
	}
}