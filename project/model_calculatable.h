#ifndef MODEL_CALCULATABLE_H
#define MODEL_CALCULATABLE_H

// Model_Calculatable - A model with extra methods for various calculations

#include "Model.h"

class QOpenGLFunctions;
class QOpenGLShaderProgram;

class Model_Calculatable : public Model
{
  public:
  // Construction
    Model_Calculatable(QOpenGLFunctions* gl, QOpenGLShaderProgram* shader);
  // Functionality
    // Overridden initialize method adds storing mesh data for calculations
    void initialize(std::vector<QVector3D> vertices, std::vector<QVector3D> tris);
    // Calculates the point on this model that is closest to the given focusPoint
    QVector3D calcClosestSurfacePoint(QVector3D focusPoint);
  private:
  // Members
    std::vector<QVector3D> vertices;
    std::vector<QVector3D> tris;
};

#endif // MODEL_CALCULATABLE_H
