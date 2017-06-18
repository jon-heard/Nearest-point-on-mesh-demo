#ifndef MODEL_WITH_CALCULATIONS_H
#define MODEL_WITH_CALCULATIONS_H

// Model_Calculatable - A Model with extra functionality for various calculations

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
        QOpenGLFunctions* gl, std::vector<std::pair<QString, QString>> shaderSources);
    ~Model_WithCalculations();
  // Functionality
    // Calculates and sets the scale so that the mesh form-fits to the given size
    void scaleToFit(float size);
    // Overridden from "Model" to add storing mesh data for calculations
    void initialize(std::vector<QVector3D> vertices, std::vector<QVector3D> triangles);
    // Overridden from "Model" to add more involved shader functionality
    virtual void draw(QMatrix4x4 projectionCameraTransform, QMatrix4x4 cameraTransform);
    // Calculates the point on this model that is closest to the given focusPoint
    QVector3D calcClosestSurfacePoint(QVector3D focus);
  // Accessors
    // "Model_WidthCalculations" can have multiple shaders prepared for use.  This gets their count
    unsigned int getShaderCount() const;
    // "focus" is the point to get the nearest mesh point to
    QVector3D getFocus() const;
    // The "nearestPoint" is the point on the mesh nearest to the "focus"
    QVector3D getNearestPoint() const;
    // Sets which of the stored shaders to use for future rendering
    void setCurrentShader(unsigned int index);
    void setFocus(QVector3D value);
    void setNearestPoint(QVector3D value);
  private:
  // Functionality - helper
    static QVector3D calcClosestPointOnTriangle(
        QVector3D point, QVector3D tri1, QVector3D triangle2, QVector3D triangle3);
  // Members
    std::vector<QVector3D> vertices;
    std::vector<QVector3D> triangles;
    std::vector<std::pair<QString,QString>> shaderSources;
    std::vector<QOpenGLShaderProgram*> shaders;
    unsigned int currentShaderIndex;
    QMatrix4x4 decalAdjustAndProjectionTransform;
    QMatrix4x4 decalCameraTransform;
    QVector3D focus, nearestPoint;
};

#endif // MODEL_WITH_CALCULATIONS_H
