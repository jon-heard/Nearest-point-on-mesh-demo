#ifndef SCENE_NEARESTPOINTDEMO_H
#define SCENE_NEARESTPOINTDEMO_H

// Scene_NearestPointDemo - A subclass of Scene designed specifically for this application.

#include "scene.h"
#include <QString>

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
    // Setup the scene
    virtual void initialize(QOpenGLFunctions* gl);
    // Called each frame to update scene components
    virtual bool update();
    // Called externally to tell the OpenGL thread to load a new Mesh Model
    void initiateMeshLoading(QString filename);
  // Accessors
    // Returns the focus model to be rotated with right mouse button.
    virtual Model* getRightMouseRotatedModel();
    // DecalType - The current decal type being rendered
    unsigned int getDecalType() const;
    // If enabled, a blue sphere will be rendered at the nearest point
    bool getIsTargetSphereEnabled();
    void setDecalType(unsigned int value);
    void setIsTargetSphereEnabled(bool value);
  private:
  // Functionality
    bool loadMesh(QString filename);
  // Fields - State
    bool isTargetSphereEnabled;
    QString newMeshFilename;
    unsigned int decalType;
  // Fields - Resources
    MainWindow* window;
    Model_WithCalculations* model_mesh;
    Model* model_focus;
    Model* model_nearestPoint;
    QOpenGLTexture* targetTexture;
};

#endif // SCENE_NEARESTPOINTDEMO_H
