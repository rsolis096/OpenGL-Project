#include <iostream>
#include <vector>
#include "Shader.h"

#define PI 3.141592653589793238462643383279502884197

class Sphere
{
    public:
        std::vector<float>vertices;
        std::vector<float>texCoords;
        std::vector<float>normals;
        std::vector<unsigned int>indices;
        std::vector<float>interleavedVertices;

        //Object color properties
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

        Texture* texture;
        unsigned int m_vao, m_vboVertices, m_ebo;
        unsigned int shaderID;
        bool hasTexture;


	Sphere(unsigned int sId, const char* texturePath)
	{      
        ambient = glm::vec3(1.0f);
        diffuse = glm::vec3(1.0f);
        specular = glm::vec3(1.0f);


        shaderID = sId;
        hasTexture = true;
        texture = new Texture(texturePath, false, GL_RGBA);
        generateSphere(30, 30, 1.0f);

        //Setup VAO and VBO
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vboVertices);
        glGenBuffers(1, &m_ebo);
        
        //Bind VAO
        glBindVertexArray(m_vao);

        //Bind and fill VBOs with data
        glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
        glBufferData(GL_ARRAY_BUFFER, interleavedVertices.size() * sizeof(float), interleavedVertices.data(), GL_STATIC_DRAW);

        // Bind and fill EBO with indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

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
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

    Sphere(unsigned int sId)
    {
        ambient = glm::vec3(0.2f);
        diffuse = glm::vec3(0.2f);
        specular = glm::vec3(0.2f);

        shaderID = sId;
        hasTexture = false;
        generateSphere(30, 30, 1.0f);

        //Setup VAO and VBO
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vboVertices);
        glGenBuffers(1, &m_ebo);

        //Bind VAO
        glBindVertexArray(m_vao);

        //Bind and fill VBOs with data
        glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
        glBufferData(GL_ARRAY_BUFFER, interleavedVertices.size() * sizeof(float), interleavedVertices.data(), GL_STATIC_DRAW);

        // Bind and fill EBO with indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

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
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    ~Sphere()
    {
        // Clean up
        glDeleteBuffers(1, &m_vboVertices);
        glDeleteBuffers(1, &m_ebo);
        glDeleteVertexArrays(1, &m_vao);
    }

    void render(float time)
    {

        glUseProgram(shaderID);
        glUniform3fv(glGetUniformLocation(shaderID, "object.ambient"), 1, &ambient[0]);
        glUniform3fv(glGetUniformLocation(shaderID, "object.diffuse"), 1, &diffuse[0]);
        glUniform3fv(glGetUniformLocation(shaderID, "object.specular"), 1, &specular[0]);

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

        //Bind object to render (sphere indices)
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

        //Render the object
        glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, 0);

        // Unbind buffers and reset state
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

    }


    private:
        void generateSphere(int stackCount, int sectorCount, float radius)
        {
            //Algorithm provided by site given in assignment (http://www.songho.ca/opengl/gl_sphere.html)
            std::vector<float>().swap(vertices);
            std::vector<float>().swap(normals);
            std::vector<float>().swap(texCoords);

            float x, y, z, xy;                              // vertex position
            float s, t;                                     // vertex texCoord
            float nx, ny, nz;

            float sectorStep = 2 * PI / sectorCount;
            float stackStep = PI / stackCount;
            float sectorAngle, stackAngle;
            float lengthInv = 1.0f / radius;

            for (int i = 0; i <= stackCount; ++i)
            {
                stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
                z = radius * sinf(stackAngle);              // r * sin(u)
                // add (sectorCount+1) vertices per stack
                // first and last vertices have same position and normal, but different tex coords
                for (int j = 0; j <= sectorCount; ++j)
                {
                    sectorAngle = j * sectorStep;          // starting from 0 to 2pi

                    // vertex position (x, y, z)
                    x = radius * cosf(stackAngle) * cosf(sectorAngle);
                    y = radius * cosf(stackAngle) * sinf(sectorAngle);
                    vertices.push_back(x);
                    vertices.push_back(y);
                    vertices.push_back(z);

                    // normalized vertex normal (nx, ny, nz)
                    nx = x * lengthInv;
                    ny = y * lengthInv;
                    nz = z * lengthInv;
                    normals.push_back(nx);
                    normals.push_back(ny);
                    normals.push_back(nz);

                    // vertex tex coord (s, t) range between [0, 1]
                    s = (float)j / sectorCount;
                    t = (float)i / stackCount;
                    texCoords.push_back(s);
                    texCoords.push_back(t);
                }
            }

            int k1, k2;
            for (int i = 0; i < stackCount; ++i)
            {
                k1 = i * (stackCount + 1);     // beginning of current stack
                k2 = k1 + sectorCount + 1;      // beginning of next stack

                for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
                {
                    // 2 triangles per sector excluding first and last stacks
                    // k1 => k2 => k1+1
                    if (i != 0)
                    {
                        indices.push_back(k1);
                        indices.push_back(k2);
                        indices.push_back(k1 + 1);
                    }

                    // k1+1 => k2 => k2+1
                    if (i != (stackCount - 1))
                    {
                        indices.push_back(k1 + 1);
                        indices.push_back(k2);
                        indices.push_back(k2 + 1);
                    }
                }
            }

            // generate interleaved vertex array as well
            buildInterleavedVertices();

        }

        void buildInterleavedVertices()
        {
            std::vector<float>().swap(interleavedVertices);

            std::size_t i, j;
            std::size_t count = vertices.size();
            for (i = 0, j = 0; i < count; i += 3, j += 2)
            {
                interleavedVertices.push_back(vertices[i]);
                interleavedVertices.push_back(vertices[i + 1]);
                interleavedVertices.push_back(vertices[i + 2]);

                interleavedVertices.push_back(normals[i]);
                interleavedVertices.push_back(normals[i+1]);
                interleavedVertices.push_back(normals[i+2]);

                interleavedVertices.push_back(texCoords[j]);
                interleavedVertices.push_back(texCoords[j + 1]);
            }

        }

};
