#ifndef SCENE_H
#define SCENE_H

// Scene - Represents a 3d scene with models to render.

#include <QQuaternion>
#include <QMatrix4x4>
#include <vector>

class Model;
class QOpenGLFunctions;

class Scene
{
  public:
  // Construction
    Scene();
    virtual ~Scene();
  // Functionality
    // Setup the scene
    virtual void initialize(QOpenGLFunctions* gl);
    // Called each frame to update scene components
    virtual void update();
    // Called to render the scene to the SceneUi
    void draw(QMatrix4x4 projectionTransform);
  // Accessors
    // Set to true if Initialize has been called on this Scene
    bool getIsInitialized();
    // Returns a Model to be rotated with right mouse button, or NULL.
    virtual Model* getRightMouseRotatedModel();
    // Rotation of the camera
    QQuaternion getRotation() const;
    // Zoom of the camera
    float getZoom() const;
    void setZoom(float value);
    void setRotation(QQuaternion value);
  protected:
    QOpenGLFunctions* gl;
    std::vector<Model*> models;
  private:
    void refreshTransform();
    float zoom;
    QQuaternion rotation;
    QMatrix4x4 transform;
    bool isInitialized;
};

#endif // SCENE_H
