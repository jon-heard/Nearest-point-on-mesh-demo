#include "model_calculatable.h"
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

using namespace std;

Model_Calculatable::Model_Calculatable(
    QOpenGLFunctions* gl, vector<pair<string, string> > shaderSources) :
  Model(gl, {"",""}), shaderSources(shaderSources) {}

Model_Calculatable::~Model_Calculatable()
{
  while (shaders.size() > 0)
  {
    delete shaders.back();
    shaders.pop_back();
  }
  shader = NULL;
}

void Model_Calculatable::draw(QMatrix4x4 decalCameraTransform, QVector3D decalNormal)
{
  this->shader->setUniformValue("mainTexture", 0);
  this->shader->setUniformValue("decalProjection", decalProjectionTransform);
  this->shader->setUniformValue("decalCamera", decalCameraTransform);
  this->shader->setUniformValue("decalAdjust", decalAdjustTransform);
  this->shader->setUniformValue("decalNormal", decalNormal);

  this->gl->glDepthMask(false);
  this->gl->glFrontFace(GL_CW);
  this->shader->setUniformValue("alpha", 0.0f);
  Model::draw();
  this->gl->glFrontFace(GL_CCW);
  this->gl->glDepthMask(true);

  this->shader->setUniformValue("alpha", .6f);
  Model::draw();
}

void Model_Calculatable::initialize(std::vector<QVector3D> vertices, std::vector<QVector3D> tris)
{
  // Base model initialize
  Model::initialize(vertices, tris);
  isReady = false;
  // mesh data store
  this->vertices = vertices;
  this->tris = tris;
  // Decal matrices
  this->decalProjectionTransform.perspective(45, 1, .1f, 1000);
  this->decalAdjustTransform.translate(.5,.5,.5);
  this->decalAdjustTransform.scale(.5,.5,.5);
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

QOpenGLShaderProgram* Model_Calculatable::getShader(unsigned int index) const
{
  if (index > shaders.size()-1)
  {
    return NULL;
  }
  return shaders[index];
}
void Model_Calculatable::setCurrentShader(unsigned int index)
{
  if (index > shaders.size()-1)
  {
    return;
  }
  shader = shaders[index];
}
