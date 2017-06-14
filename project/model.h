#ifndef MESH_H
#define MESH_H

// Model - An object to be rendered within a 3D scene

#include <vector>
#include <QVector3D>
#include <QMatrix4x4>

class QOpenGLFunctions;
class QOpenGLShaderProgram;
class QOpenGLBuffer;
class QOpenGLVertexArrayObject;

class Model
{
  public:
  // Construction
    Model(QOpenGLFunctions* gl, QOpenGLShaderProgram* shader);
    virtual ~Model();
  // Functionality
    // Render this Model into the scene
    void draw();
    // Initialize this model with mesh data
    virtual void initialize(std::vector<QVector3D> vertices, std::vector<QVector3D> tris);
    // Get model's position in world space
    QVector3D calcWorldPosition();
  // Accessors
    float getScale() const;
    QVector3D getPosition() const;
    QVector3D getRotation() const;
    QOpenGLShaderProgram* getShader() const;
    std::vector<QVector3D> getVertices() const;
    void setScale(float value);
    void setPosition(QVector3D value);
    void setRotation(QVector3D value);
    void setShader(QOpenGLShaderProgram* value);
  private:
  // Functionality - internal
    void refreshTransform();
  // Members
    bool isReady;
    float scale;
    QVector3D position;
    QVector3D rotation;
    QMatrix4x4 transform;
    int vertexCount;
    QOpenGLShaderProgram* shader;
    QOpenGLBuffer* vbo;
    QOpenGLVertexArrayObject* vao;
    QOpenGLFunctions* gl;
};

#endif // MESH_H
