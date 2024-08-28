#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform int lightType; //0 = Point Light, 1 = Spot Light, 2-Directional Light

//For point lights
uniform mat4 shadowMatricesPoint[6];

out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main()
{
    if(lightType == 0)
    {
        for(int face = 0; face < 6; ++face)
        {
            gl_Layer = face; // built-in variable that specifies to which face we render.
            for(int i = 0; i < 3; ++i) // for each triangle's vertices
            {
                FragPos = gl_in[i].gl_Position;
                gl_Position = shadowMatricesPoint[face] * FragPos;
                EmitVertex();
            }    
            EndPrimitive();
        }
    }    

    //Immitate default behaviour of the depth shader (spot light)
    else if (lightType == 1 || lightType == 2)
    {
        for (int i = 0; i < 3; ++i) // for each triangle's vertices
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }

} 