#ifndef MODEL_H
#define MODEL_H

// Model - An object to be rendered within a 3D scene.
//     Note: The transformation is non-standard. Use in other applications will require adjustment.

#include <vector>
#include <utility> // std::pair
#include <QString>
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
    Model(QOpenGLFunctions* gl, std::pair<QString, QString> shaderSource);
    virtual ~Model();
  // Functionality
    // Update the transform with the latest transformation info
    void refreshTransform();
    // Render this Model into the scene
    virtual void draw(QMatrix4x4 transform_projection_camera, QMatrix4x4 transform_camera);
    // Initialize this model with mesh data
    virtual void initialize(std::vector<QVector3D> vertices, std::vector<QVector3D> tris);
    // Get model's position in world space
    QVector3D calcWorldPosition();
  // Accessors
    // Visible - Determines whether this model is rendered or not
    bool getIsVisible() const;
    // Scale - Adjusts the size of this model
    float getScale() const;
    // Position - Adjusts the position of this model in world space
    QVector3D getPosition() const;
    // Rotation - Determines how the model is rotated in world space
    QQuaternion getRotation() const;
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
    unsigned int vertexCount;
  private:
  // Members
    bool isVisible;
    float scale;
    QVector3D position;
    QQuaternion rotation;
    std::pair<QString, QString> shaderSource;
    QOpenGLBuffer* vbo;
};

#endif // MODEL_H
