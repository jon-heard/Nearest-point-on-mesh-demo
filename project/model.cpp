#include "model.h"
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QDebug>

using namespace std;

Model::Model(QOpenGLFunctions* gl, std::pair<QString, QString> shaderSource) :
  isReady(false), gl(gl), shader(NULL), vao(NULL), isVisible(true), scale(1),
  shaderSource(shaderSource), vbo(NULL) {}

Model::~Model()
{
  if (this->shader != NULL)
  {
    delete this->shader;
    this->shader = NULL;
  }
  if (this->vao != NULL)
  {
    this->vao->destroy();
    delete this->vao;
    this->vao = NULL;
  }
  if (this->vbo != NULL)
  {
    this->vbo->destroy();
    delete this->vbo;
    this->vbo = NULL;
  }
}

void Model::refreshTransform()
{
  this->transform.setToIdentity();
  this->transform.rotate(rotation);
  this->transform.translate(this->position);
  this->transform.scale(this->scale, this->scale, this->scale);
}

void Model::draw(QMatrix4x4 transform_projection_camera, QMatrix4x4 transform_camera)
{
  if (!this->isReady)
  {
    qCritical() << "Model error: drawn before initialized";
    return;
  }
  if (!this->isVisible)
  {
    return;
  }
  QMatrix4x4 transform_projection_camera_model = transform_projection_camera * this->transform;
  QMatrix4x4 transform_camera_model = transform_camera * this->transform;
  this->shader->bind();
  this->shader->setUniformValue(
      "transform_projection_camera_model", transform_projection_camera_model);
  this->shader->setUniformValue("transform_camera_model", transform_camera_model);
  this->vao->bind();

  this->gl->glDrawArrays(GL_TRIANGLES, 0, this->vertexCount);
  this->vao->release();
  this->shader->release();
}

void Model::initialize(std::vector<QVector3D> vertices, std::vector<QVector3D> tris)
{
  // Only initialize once
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
          QOpenGLShader::Vertex, this->shaderSource.first);
    this->shader->addShaderFromSourceFile(
          QOpenGLShader::Fragment, this->shaderSource.second);
    this->shader->bindAttributeLocation("position", 1);
    this->shader->bindAttributeLocation("normal", 2);
    this->shader->link();
    if (!this->shader->isLinked())
    {
      qCritical() << "Shader error: <" << this->shaderSource.first << ", " <<
                     this->shaderSource.second << ">";
      return;
    }
  }
  // Allocate data buffer
  this->vertexCount = (unsigned int)tris.size() * 3;
  float* dataBuffer = new float[this->vertexCount * 6];
  unsigned int currentDataBufferIndex = 0;
  // Fill data buffer
  for (vector<QVector3D>::iterator i = tris.begin(); i != tris.end(); ++i)
  {
    // Normal calculation
    QVector3D normalCalc1 = (vertices[(*i)[1]] - vertices[(*i)[0]]).normalized();
    QVector3D normalCalc2 = (vertices[(*i)[2]] - vertices[(*i)[0]]).normalized();
    QVector3D normal = QVector3D::crossProduct(normalCalc1, normalCalc2).normalized();
    for (unsigned int j = 0; j < 3; ++j)
    {
      unsigned int currentVectorIndex = (*i)[j];
      // Vector data
      for (unsigned int k = 0; k < 3; ++k)
      {
        dataBuffer[currentDataBufferIndex] = vertices[currentVectorIndex][k];
        ++currentDataBufferIndex;
      }
      // Normal data
      for (unsigned int k = 0; k < 3; ++k)
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
  // Using native opengl commands (instead of QOpenGLShader) as shader may not exist at this point
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
  return this->transform * QVector3D();
}

bool Model::getIsVisible() const { return this->isVisible; }
float Model::getScale() const { return this->scale; }
QVector3D Model::getPosition() const { return this->position; }
QQuaternion Model::getRotation() const { return this->rotation; }

void Model::setIsVisible(bool value)
{
  this->isVisible = value;
}

void Model::setScale(float value)
{
  this->scale = value;
  this->refreshTransform();
}

void Model::setPosition(QVector3D value)
{
  this->position = value;
  this->refreshTransform();
}

void Model::setRotation(QQuaternion value)
{
  this->rotation = value;
  this->refreshTransform();
}
