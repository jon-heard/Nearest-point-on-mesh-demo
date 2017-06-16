#include "model_calculatable.h"
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

using namespace std;

Model_Calculatable::Model_Calculatable(QOpenGLFunctions* gl, vector<std::pair<string, string> > shaderSources) :
  Model(gl, shaderSources[0]), currentShaderIndex() {}

void Model_Calculatable::initialize(std::vector<QVector3D> vertices, std::vector<QVector3D> tris)
{
  Model::initialize(vertices, tris);
  this->vertices = vertices;
  this->tris = tris;
}

QVector3D Model_Calculatable::calcClosestSurfacePoint(QVector3D focusPoint)
{
  QVector3D result;
  int resultDistance = INT_MAX;
  for (vector<QVector3D>::iterator i = this->vertices.begin(); i != this->vertices.end(); ++i)
  {
    int length = (focusPoint - *i).length();
    if (length < resultDistance)
    {
      resultDistance = length;
      result = *i;
    }
  }
  return result;
}
