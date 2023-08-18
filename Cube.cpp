#include "Cube.h"

Cube::Cube(float inputVertices[], int size, const char* texturePath, unsigned int sID)
{
	shaderID = sID;
	hasTexture = true;
	texture = new Texture("Assets/monito.png", false, GL_RGBA);
	//Generate VAO and VBO
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	//Assign vertices to object
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, size, inputVertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

Cube::Cube(float inputVertices[], int size, unsigned int sID)
{
	shaderID = sID;
	hasTexture = false;
	//Generate VAO and VBO
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	//Assign vertices to object
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, size, inputVertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Texture Attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
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

void Cube::initializeCube()
{

}