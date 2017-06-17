#ifndef SCENE_H
#define SCENE_H

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
    virtual void initialize(QOpenGLFunctions* gl);
    virtual void update();
  // Accessors
    float getZoom() const;
    QQuaternion getRotation() const;
    QMatrix4x4 getTransform() const;
    std::vector<Model*>* getModels();
    void setZoom(float value);
    void setRotation(QQuaternion value);
  protected:
  // Helper
    QOpenGLFunctions* gl;
  private:
  // Functionality
    void refreshTransform();
  // Fields
    // Transform
    float zoom;
    QQuaternion rotation;
    QMatrix4x4 transform;
    // To render
    std::vector<Model*> models;
};

#endif // SCENE_H
