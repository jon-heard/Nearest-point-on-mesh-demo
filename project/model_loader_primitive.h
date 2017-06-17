#ifndef MODEL_LOADER_PRIMITIVE_H
#define MODEL_LOADER_PRIMITIVE_H

// Model_FileLoader_Off - Provides functionality to load primitive meshes into model objects

class Model;

class Model_Loader_Primitive
{
  public:
    // Creates mesh data for a sphere, and puts it into the given model
    bool loadSphereIntoModel(Model* model, float radius, int subdivisionCount);
    // Creates mesh data for a box, and puts it into the given model
    bool loadBoxIntoModel(Model* model, float sizeX, float sizeY, float sizeZ);
};

#endif // MODEL_LOADER_PRIMITIVE_H
