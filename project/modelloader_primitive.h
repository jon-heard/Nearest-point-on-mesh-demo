#ifndef MODELLOADER_PRIMITIVE_H
#define MODELLOADER_PRIMITIVE_H

// Model_FileLoader_Off - Provides functionality to load primitive meshes into model objects

class Model;

class ModelLoader_Primitive
{
  public:
    // Creates mesh data for a sphere, and puts it into the given model
    bool loadSphereIntoModel(Model* model, float radius, unsigned int subdivisionCount);
    // Creates mesh data for a box, and puts it into the given model
    bool loadBoxIntoModel(Model* model, float sizeX, float sizeY, float sizeZ);
};

#endif // MODELLOADER_PRIMITIVE_H
