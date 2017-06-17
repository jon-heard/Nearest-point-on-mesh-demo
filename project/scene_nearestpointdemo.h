#ifndef SCENE_NEARESTPOINTDEMO_H
#define SCENE_NEARESTPOINTDEMO_H

#include "scene.h"
#include <string>

class Model;
class Model_WithCalculations;
class QOpenGLTexture;

class Scene_NearestPointDemo : public Scene
{
  public:
  // Construction
    Scene_NearestPointDemo();
    virtual ~Scene_NearestPointDemo();
  // Functionality
    void initialize(QOpenGLFunctions* gl);
    void update();
  // Accessors
    Model_WithCalculations* getModel_mesh();
    Model* getModel_focus();
    Model* getModel_nearestPoint();
    QOpenGLTexture* getTargetTexture();
  private:
  // Functionality
    bool loadMesh(std::string filename);
  // Fields
    Model_WithCalculations* model_mesh;
    Model* model_focus;
    Model* model_nearestPoint;
    QOpenGLTexture* targetTexture;
};

#endif // SCENE_NEARESTPOINTDEMO_H
