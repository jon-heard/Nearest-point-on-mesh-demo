#include "model.h"
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QDebug>

using namespace std;

Model::Model(QOpenGLFunctions* gl, QOpenGLShaderProgram* shader) :
  isReady(false), isVisible(true), scale(1), shader(shader),
  vbo(NULL), vao(NULL), gl(gl) {}

Model::~Model()
{
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

void Model::draw()
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
  this->shader->setUniformValue("modelTransform", this->transform);
  this->vao->bind();
  this->gl->glDrawArrays(GL_TRIANGLES, 0, this->vertexCount);
  this->vao->release();
}

void Model::initialize(std::vector<QVector3D> vertices, std::vector<QVector3D> tris)
{
  // only initialize once
  if (this->isReady)
  {
    qCritical() << "Model error: initialized twice";
    return;
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
  this->gl->glEnableVertexAttribArray(1);
  this->gl->glEnableVertexAttribArray(2);
  this->gl->glVertexAttribPointer(1, 3, GL_FLOAT, false, 6*4, 0);
  this->gl->glVertexAttribPointer(2, 3, GL_FLOAT, false, 6*4, (void*)(3*4));
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
QVector3D Model::getRotation() const { return this->rotation; }
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

void Model::setRotation(QVector3D value)
{
  this->rotation = value;
  refreshTransform();
}

void Model::setShader(QOpenGLShaderProgram* value)
{
  this->shader = value;
}

void Model::refreshTransform()
{
  transform.setToIdentity();
  transform.rotate(rotation.x(), QVector3D(1, 0, 0));
  transform.rotate(rotation.y(), QVector3D(0, 1, 0));
  transform.rotate(rotation.z(), QVector3D(0, 0, 1));
  transform.translate(position);
  transform.scale(scale, scale, scale);
}
