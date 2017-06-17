#include "model_withcalculations.h"
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

using namespace std;

Model_WithCalculations::Model_WithCalculations(
    QOpenGLFunctions* gl, vector<pair<string, string> > shaderSources) :
  Model(gl, {"",""}), shaderSources(shaderSources) {}

Model_WithCalculations::~Model_WithCalculations()
{
  while (shaders.size() > 0)
  {
    delete shaders.back();
    shaders.pop_back();
  }
  shader = NULL;
}

void Model_WithCalculations::draw(
    QMatrix4x4 projectionCameraTransform, QMatrix4x4 cameraTransform,
    QVector3D focus, QVector3D nearestPoint)
{
  QMatrix4x4 decalCameraTransform;
  decalCameraTransform.lookAt(focus, nearestPoint, QVector3D(0,1,0));
  QVector3D decalNormal = (nearestPoint - focus).normalized();


  this->gl->glDepthMask(false);
  this->gl->glFrontFace(GL_CW);

  this->shader->bind();
  // Texture / alpha settings
  this->shader->setUniformValue("alpha", 0.0f);
  this->shader->setUniformValue("mainTexture", 0);
  // Projected texture settings
  this->shader->setUniformValue(
      "decalAdjustAndProjectionTransform", decalAdjustAndProjectionTransform);
  this->shader->setUniformValue("decalCameraTransform", decalCameraTransform);
  this->shader->setUniformValue("decalNormal", decalNormal);
  // Distanced texture settings
  this->shader->setUniformValue("focus", focus);
  this->shader->setUniformValue("nearestPoint", nearestPoint);

  Model::draw(projectionCameraTransform, cameraTransform);

  this->gl->glFrontFace(GL_CCW);
  this->gl->glDepthMask(true);


  this->shader->bind();
  // Texture / alpha settings
  this->shader->setUniformValue("alpha", 0.6f);
  this->shader->setUniformValue("mainTexture", 0);
  // Projected texture settings
  this->shader->setUniformValue(
      "decalAdjustAndProjectionTransform", decalAdjustAndProjectionTransform);
  this->shader->setUniformValue("decalCameraTransform", decalCameraTransform);
  this->shader->setUniformValue("decalNormal", decalNormal);
  // Distanced texture settings
  this->shader->setUniformValue("focus", focus);
  this->shader->setUniformValue("nearestPoint", nearestPoint);

  Model::draw(projectionCameraTransform, cameraTransform);
}

void Model_WithCalculations::initialize(std::vector<QVector3D> vertices, std::vector<QVector3D> tris)
{
  // Base model initialize
  Model::initialize(vertices, tris);
  isReady = false;
  // mesh data store
  this->vertices = vertices;
  this->tris = tris;
  // Decal matrices
  this->decalAdjustAndProjectionTransform.translate(.5, .5, .5);
  this->decalAdjustAndProjectionTransform.scale(.5, .5, .5);
  this->decalAdjustAndProjectionTransform.perspective(45, 1, .1f, 1000);
  // shaders
  for (vector<pair<string,string>>::iterator i = shaderSources.begin(); i != shaderSources.end(); ++i)
  {
    QOpenGLShaderProgram* newShader = new QOpenGLShaderProgram();
    newShader->addShaderFromSourceFile(
      QOpenGLShader::Vertex, (*i).first.c_str());
    newShader->addShaderFromSourceFile(
      QOpenGLShader::Fragment, (*i).second.c_str());
    newShader->link();
    if(!newShader->isLinked())
    {
      qCritical() << "Shader error: <" <<
                     (*i).first.c_str() <<
                     ", " <<
                     (*i).second.c_str() <<
                     ">";
      return;
    }
    shaders.push_back(newShader);
  }
  currentShaderIndex = 0;
  shader = shaders[0];
  // done
  isReady = true;
}

QVector3D Model_WithCalculations::calcClosestSurfacePoint(QVector3D focusPoint)
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

QOpenGLShaderProgram* Model_WithCalculations::getShader(unsigned int index) const
{
  if (index > shaders.size()-1)
  {
    return NULL;
  }
  return shaders[index];
}
void Model_WithCalculations::setCurrentShader(unsigned int index)
{
  if (index > shaders.size()-1)
  {
    return;
  }
  shader = shaders[index];
}
