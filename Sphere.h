
#pragma once

#include "Object.h"

class Sphere : public Object
{

public:
    Sphere(Shader& shader, const char* texturePath);
    Sphere(Shader& shader);
    ~Sphere();

private:
    void generateSphere(int stackCount, int sectorCount, float radius);

};