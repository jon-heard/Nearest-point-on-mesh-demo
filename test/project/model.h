#ifndef MODEL_H
#define MODEL_H

// Model - An object to be rendered within a 3D scene

#include <vector>
#include <string>
#include <utility> // std::pair
#include <QVector3D>
#include <QQuaternion>
#include <QMatrix4x4>

class QOpenGLFunctions;
class QOpenGLShaderProgram;
class QOpenGLBuffer;
class QOpenGLVertexArrayObject;

class Model
{
  public:
  // Construction
    Model(QOpenGLFunctions* gl, std::pair<std::string, std::string> shaderSource);
    virtual ~Model();
  // Functionality
    // Update the transform with the latest transformation info
    void refreshTransform();
    // Render this Model into the scene
    virtual void draw(QMatrix4x4 projectionCameraTransform, QMatrix4x4 cameraTransform);
    // Initialize this model with mesh data
    virtual void initialize(std::vector<QVector3D> vertices, std::vector<QVector3D> tris);
    // Get model's position in world space
    QVector3D calcWorldPosition();
  // Accessors
    bool getIsVisible() const;
    float getScale() const;
    QVector3D getPosition() const;
    QQuaternion getRotation() const;
    QOpenGLShaderProgram* getShader() const;
    std::vector<QVector3D> getVertices() const;
    void setIsVisible(bool value);
    void setScale(float value);
    void setPosition(QVector3D value);
    void setRotation(QQuaternion value);
  protected:
    QOpenGLFunctions* gl;
    bool isReady;
    QOpenGLShaderProgram* shader;
    QMatrix4x4 transform;
    QOpenGLVertexArrayObject* vao;
    int vertexCount;
  private:
  // Members
    bool isVisible;
    float scale;
    QVector3D position;
    QQuaternion rotation;
    std::pair<std::string, std::string> shaderSource;
    QOpenGLBuffer* vbo;
};

#endif // MODEL_H