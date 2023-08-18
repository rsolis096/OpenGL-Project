#include "Cube.h"

Cube::Cube(float inputVertices[], int size)
{
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

	// Texture attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

Cube::~Cube()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}

void Cube::render(int quantity)
{
	//Bind texture and render to VAO
	glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture (2 texture in frag shader)
	glBindTexture(GL_TEXTURE_2D, texture->ID);

	//Render the cube
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Cube::bind()
{
	glBindVertexArray(m_vao);
}
