#ifndef MODEL_CALCULATABLE_H
#define MODEL_CALCULATABLE_H

#include "Model.h"

class QOpenGLFunctions;
class QOpenGLShaderProgram;

class Model_Calculatable : public Model
{
  public:
    // Construction
    Model_Calculatable(QOpenGLFunctions* gl, QOpenGLShaderProgram* shader);
    // Functionality
    void initialize(std::vector<QVector3D> vertices, std::vector<QVector3D> tris);
    QVector3D calcClosestSurfacePoint(QVector3D focusPoint);
  private:
    std::vector<QVector3D> vertices;
    std::vector<QVector3D> tris;
};

#endif // MODEL_CALCULATABLE_H
