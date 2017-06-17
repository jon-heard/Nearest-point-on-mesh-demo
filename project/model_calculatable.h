#ifndef MODEL_CALCULATABLE_H
#define MODEL_CALCULATABLE_H

// Model_Calculatable - A model with extra functionality for various calculations

#include "Model.h"
#include <vector>
#include <QVector3D>
#include <QMatrix4x4>

class QOpenGLFunctions;
class QOpenGLShaderProgram;

class Model_Calculatable : public Model
{
  public:
  // Construction
    Model_Calculatable(
        QOpenGLFunctions* gl, std::vector<std::pair<std::string, std::string>> shaderSources);
    ~Model_Calculatable();
  // Functionality
    // Overridden to add more involved shader functionality
    void draw(QMatrix4x4 projectionCameraTransform, QMatrix4x4 cameraTransform, QMatrix4x4 decalCameraTransform, QVector3D decalNormal);
    // Overridden initialize method adds storing mesh data for calculations
    void initialize(std::vector<QVector3D> vertices, std::vector<QVector3D> tris);
    // Calculates the point on this model that is closest to the given focusPoint
    QVector3D calcClosestSurfacePoint(QVector3D focusPoint);
  // Accessors
    QOpenGLShaderProgram* getShader(unsigned int index) const;
    void setCurrentShader(unsigned int index);
  private:
  // Members
    std::vector<QVector3D> vertices;
    std::vector<QVector3D> tris;
    std::vector<std::pair<std::string,std::string>> shaderSources;
    std::vector<QOpenGLShaderProgram*> shaders;
    unsigned int currentShaderIndex;
    QMatrix4x4 decalAdjustAndProjectionTransform;
    QMatrix4x4 decalCameraTransform;
};

#endif // MODEL_CALCULATABLE_H
