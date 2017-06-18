#include "model.h"
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QDebug>

using namespace std;

Model::Model(QOpenGLFunctions* gl, pair<string, string> shaderSource) :
  isReady(false), isVisible(true), scale(1), shaderSource(shaderSource),
  shader(NULL), vbo(NULL), vao(NULL), gl(gl) {}

Model::~Model()
{
  if (shader != NULL)
  {
    delete this->shader;
    this->shader = NULL;
  }
  if (vao != NULL)
  {
    this->vao->destroy();
    delete this->vao;
    this->vao = NULL;
  }
  if (vbo != NULL)
  {
    this->vbo->destroy();
    delete this->vbo;
    this->vbo = NULL;
  }
}

void Model::refreshTransform()
{
  transform.setToIdentity();
  transform.rotate(rotation);
  transform.translate(position);
  transform.scale(scale, scale, scale);
}

void Model::draw(QMatrix4x4 projectionCameraTransform, QMatrix4x4 cameraTransform)
{
  if (!isReady)
  {
    qCritical() << "Model error: drawn before initialized";
    return;
  }
  if (!isVisible)
  {
    return;
  }
  this->shader->bind();
  this->shader->setUniformValue("projectionCameraTransform", projectionCameraTransform);
  this->shader->setUniformValue("cameraTransform", cameraTransform);
  this->shader->setUniformValue("modelTransform", this->transform);
  this->vao->bind();
  this->gl->glDrawArrays(GL_TRIANGLES, 0, this->vertexCount);
  this->vao->release();
  this->shader->release();
}

void Model::initialize(std::vector<QVector3D> vertices, std::vector<QVector3D> tris)
{
  // only initialize once
  if (this->isReady)
  {
    qCritical() << "Model error: initialized twice";
    return;
  }
  // Setup shader (if given)
  if (this->shaderSource.first != "")
  {
    this->shader = new QOpenGLShaderProgram();
    this->shader->addShaderFromSourceFile(
      QOpenGLShader::Vertex, shaderSource.first.c_str());
    this->shader->addShaderFromSourceFile(
      QOpenGLShader::Fragment, shaderSource.second.c_str());
    this->shader->link();
    if(!this->shader->isLinked())
    {
      qCritical() << "Shader error: <" <<
                     shaderSource.first.c_str() <<
                     ", " <<
                     shaderSource.second.c_str() <<
                     ">";
      return;
    }
  }
  // Allocate data buffer
  this->vertexCount = (int)tris.size() * 3;
  float* dataBuffer = new float[this->vertexCount * 6];
  int currentDataBufferIndex = 0;
  // fill data buffer
  for (vector<QVector3D>::iterator i = tris.begin(); i != tris.end(); ++i)
  {
      // normal calculation
      QVector3D normalCalc1 = (vertices[(*i)[1]] - vertices[(*i)[0]]).normalized();
      QVector3D normalCalc2 = (vertices[(*i)[2]] - vertices[(*i)[0]]).normalized();
      QVector3D normal = QVector3D::crossProduct(normalCalc1, normalCalc2).normalized();
      for (int j = 0; j < 3; ++j)
      {
          int currentVectorIndex = (*i)[j];
          // vector data
          for (int k = 0; k < 3; ++k)
          {
              dataBuffer[currentDataBufferIndex] = vertices[currentVectorIndex][k];
              ++currentDataBufferIndex;
          }
          // normal data
          for (int k = 0; k < 3; ++k)
          {
              dataBuffer[currentDataBufferIndex] = normal[k];
              ++currentDataBufferIndex;
          }
      }
  }
  // Allocate OpenGL buffers
  this->vao = new QOpenGLVertexArrayObject();
  this->vao->create();
  this->vao->bind();
  this->vbo = new QOpenGLBuffer();
  this->vbo->create();
  this->vbo->bind();
  this->vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
  this->vbo->allocate(dataBuffer, vertexCount * 6 * 4);
  //  this->gl->glEnableVertexAttribArray(1);
  //  this->gl->glEnableVertexAttribArray(2);
  //  this->gl->glVertexAttribPointer(1, 3, GL_FLOAT, false, 6*4, 0);
  //  this->gl->glVertexAttribPointer(2, 3, GL_FLOAT, false, 6*4, (void*)(3*4));
  this->shader->enableAttributeArray(1);
  this->shader->enableAttributeArray(2);
  this->shader->setAttributeBuffer(1, GL_FLOAT, 0, 3, 6*4);
  this->shader->setAttributeBuffer(2, GL_FLOAT, 3*4, 3, 6*4);

  this->vbo->release();
  this->vao->release();
  // Wrap up
  delete dataBuffer;
  this->isReady = true;
}

QVector3D Model::calcWorldPosition()
{
  return transform * QVector3D();
}

bool Model::getIsVisible() const { return isVisible; }
float Model::getScale() const { return this->scale; }
QVector3D Model::getPosition() const { return this->position; }
QQuaternion Model::getRotation() const { return this->rotation; }
QOpenGLShaderProgram* Model::getShader() const { return this->shader; }

void Model::setIsVisible(bool value)
{
  this->isVisible = value;
}

void Model::setScale(float value)
{
  this->scale = value;
  refreshTransform();
}

void Model::setPosition(QVector3D value)
{
  this->position = value;
  refreshTransform();
}

void Model::setRotation(QQuaternion value)
{
  this->rotation = value;
  refreshTransform();
}

