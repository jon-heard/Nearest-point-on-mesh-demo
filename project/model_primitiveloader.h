#ifndef MODEL_PRIMITIVELOADER_H
#define MODEL_PRIMITIVELOADER_H

class Model;

class Model_PrimitiveLoader
{
  public:
    bool loadSphereIntoModel(Model* model, float radius, int subdivisionCount);
    bool loadBoxIntoModel(Model* model, float sizeX, float sizeY, float sizeZ);
};

#endif // MODEL_PRIMITIVELOADER_H
