#ifndef SCENE_NEARESTPOINTDEMO_H
#define SCENE_NEARESTPOINTDEMO_H

#include "scene.h"
#include <string>

class Model;
class Model_WithCalculations;
class QOpenGLTexture;
class MainWindow;

class Scene_NearestPointDemo : public Scene
{
  public:
  // Construction
    Scene_NearestPointDemo(MainWindow* window);
    virtual ~Scene_NearestPointDemo();
  // Functionality
    virtual void initialize(QOpenGLFunctions* gl);
    virtual void update();
    void initiateMeshLoading(std::string filename);
  // Accessors
    virtual Model* getRightMouseRotatedModel();
    int getDecalType() const;
//    Model_WithCalculations* getModel_mesh();
//    Model* getModel_focus();
//    Model* getModel_nearestPoint();
//    QOpenGLTexture* getTargetTexture();
    bool getIsTargetSphereEnabled();
    void setDecalType(unsigned int value);
    void setIsTargetSphereEnabled(bool value);
  private:
  // Functionality
    bool loadMesh(std::string filename);
  // Fields - State
    bool isTargetSphereEnabled;
    std::string newMeshFilename;
    int decalType;
  // Fields - Resources
    MainWindow* window;
    Model_WithCalculations* model_mesh;
    Model* model_focus;
    Model* model_nearestPoint;
    QOpenGLTexture* targetTexture;
};

#endif // SCENE_NEARESTPOINTDEMO_H
