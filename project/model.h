#ifndef MESH_H
#define MESH_H

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
    Model(QOpenGLFunctions* gl, std::pair<std::string, std::string> shaderSources);
    virtual ~Model();
  // Functionality
    // Update the transform with the latest transformation info
    void refreshTransform();
    // Render this Model into the scene
    void draw();
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
    void setShader(QOpenGLShaderProgram* value);
  private:
  // Members
    bool isReady;
    bool isVisible;
    float scale;
    QVector3D position;
    QQuaternion rotation;
    QMatrix4x4 transform;
    int vertexCount;
    std::pair<std::string, std::string> shaderSources;
    QOpenGLShaderProgram* shader;
    QOpenGLBuffer* vbo;
    QOpenGLVertexArrayObject* vao;
    QOpenGLFunctions* gl;
};

#endif // MESH_H
