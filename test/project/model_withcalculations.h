#ifndef MODEL_WITH_CALCULATIONS_H
#define MODEL_WITH_CALCULATIONS_H

// Model_Calculatable - A model with extra functionality for various calculations

#include "Model.h"
#include <vector>
#include <QVector3D>
#include <QMatrix4x4>

class QOpenGLFunctions;
class QOpenGLShaderProgram;

class Model_WithCalculations : public Model
{
  public:
  // Construction
    Model_WithCalculations(
        QOpenGLFunctions* gl, std::vector<std::pair<std::string, std::string>> shaderSources);
    ~Model_WithCalculations();
  // Functionality
    // Calculates and sets the scale so that the mesh form-fits to the given size
    void scaleToFit(float size);
    // Overridden to add more involved shader functionality
    void draw(QMatrix4x4 projectionCameraTransform, QMatrix4x4 cameraTransform,
        QVector3D focus, QVector3D nearestPoint);
    // Overridden initialize method adds storing mesh data for calculations
    void initialize(std::vector<QVector3D> vertices, std::vector<QVector3D> triangles);
    // Calculates the point on this model that is closest to the given focusPoint
    QVector3D calcClosestSurfacePoint(QVector3D focus);
  // Accessors
    QOpenGLShaderProgram* getShader(unsigned int index) const;
    void setCurrentShader(unsigned int index);
  private:
  // Functionality - helper
    static QVector3D calcClosestPointOnTriangle(
        QVector3D point, QVector3D tri1, QVector3D triangle2, QVector3D triangle3);
  // Members
    std::vector<QVector3D> vertices;
    std::vector<QVector3D> triangles;
    std::vector<std::pair<std::string,std::string>> shaderSources;
    std::vector<QOpenGLShaderProgram*> shaders;
    unsigned int currentShaderIndex;
    QMatrix4x4 decalAdjustAndProjectionTransform;
    QMatrix4x4 decalCameraTransform;
};

#endif // MODEL_WITH_CALCULATIONS_H
