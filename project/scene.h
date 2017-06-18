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
    void draw(QMatrix4x4 projectionTransform);
  // Accessors
    bool getIsInitialized();
    virtual Model* getRightMouseRotatedModel();
    QQuaternion getRotation() const;
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
