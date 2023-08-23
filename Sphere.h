#include "Object.h"
#define PI 3.141592653589793238462643383279502884197

class Sphere : public Object
{
    public:


	Sphere(unsigned int sId, const char* texturePath) : Object(sId, texturePath, texturePath)
	{      
        m_useEBO = true;
        generateSphere(30, 30, 1.0f);

        //Setup VAO and VBO
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);
        
        //Bind VAO
        glBindVertexArray(m_vao);

        //Bind and fill VBOs with data
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, m_InterleavedVertices.size() * sizeof(float), m_InterleavedVertices.data(), GL_STATIC_DRAW);

        // Bind and fill EBO with indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_Indices.size(), m_Indices.data(), GL_STATIC_DRAW);

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

    Sphere(unsigned int sId) : Object(sId)
    {
        m_useEBO = true;
        generateSphere(30, 30, 1.0f);

        //Setup VAO and VBO
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);

        //Bind VAO
        glBindVertexArray(m_vao);

        //Bind and fill VBOs with data
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, m_InterleavedVertices.size() * sizeof(float), m_InterleavedVertices.data(), GL_STATIC_DRAW);

        // Bind and fill EBO with indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_Indices.size(), m_Indices.data(), GL_STATIC_DRAW);

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
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);
        glDeleteVertexArrays(1, &m_vao);
    }
    
    //Teleport to specified location
    void setPosition(glm::vec3 newPosition)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, newPosition);
        glUseProgram(m_shaderID);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderID, "model"), 1, GL_FALSE, &model[0][0]);
    }

    //Translate by parameter (Used to move some direction from current position)
    void translatePosition(glm::vec3 newPosition)
    {
        m_Position[0] += newPosition[0];
        m_Position[1] += newPosition[1];
        m_Position[2] += newPosition[2];

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_Position);
        glUseProgram(m_shaderID);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderID, "model"), 1, GL_FALSE, &model[0][0]);

    }


    private:
        void generateSphere(int stackCount, int sectorCount, float radius)
        {
            //Algorithm provided by site given in assignment (http://www.songho.ca/opengl/gl_sphere.html)
            std::vector<float>().swap(m_Vertices);
            std::vector<float>().swap(m_Normals);
            std::vector<float>().swap(m_TexCoords);

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
                    m_Vertices.push_back(x);
                    m_Vertices.push_back(y);
                    m_Vertices.push_back(z);

                    // normalized vertex normal (nx, ny, nz)
                    nx = x * lengthInv;
                    ny = y * lengthInv;
                    nz = z * lengthInv;
                    m_Normals.push_back(nx);
                    m_Normals.push_back(ny);
                    m_Normals.push_back(nz);

                    // vertex tex coord (s, t) range between [0, 1]
                    s = (float)j / sectorCount;
                    t = (float)i / stackCount;
                    m_TexCoords.push_back(s);
                    m_TexCoords.push_back(t);
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
                        m_Indices.push_back(k1);
                        m_Indices.push_back(k2);
                        m_Indices.push_back(k1 + 1);
                    }

                    // k1+1 => k2 => k2+1
                    if (i != (stackCount - 1))
                    {
                        m_Indices.push_back(k1 + 1);
                        m_Indices.push_back(k2);
                        m_Indices.push_back(k2 + 1);
                    }
                }
            }

            // generate interleaved vertex array as well
            buildInterleavedVerticesWithTexCoords();

        }
};
