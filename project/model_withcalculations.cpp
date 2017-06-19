#include "model_withcalculations.h"
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

using namespace std;

Model_WithCalculations::Model_WithCalculations(
    QOpenGLFunctions* gl, vector<pair<QString, QString> > shaderSources) :
  Model(gl, {"",""}), shaderSources(shaderSources) {}

Model_WithCalculations::~Model_WithCalculations()
{
  this->shader = NULL; // Null out 'current' shader so it's not deleted by base 'Model' destructor
  while (this->shaders.size() > 0)
  {
    delete this->shaders.back();
    this->shaders.pop_back();
  }
}

void Model_WithCalculations::scaleToFit(float size)
{
  // Get the min and max vertex locations on each axis
  QVector3D min((float)INT_MAX, (float)INT_MAX, (float)INT_MAX);
  QVector3D max((float)INT_MIN, (float)INT_MIN, (float)INT_MIN);
  for (vector<QVector3D>::iterator i = this->vertices.begin(); i != this->vertices.end(); ++i)
  {
    for (unsigned int j = 0; j < 3; ++j)
    {
      if ((*i)[0] < min[0]) { min[0] = (*i)[0]; }
      if ((*i)[1] < min[1]) { min[1] = (*i)[1]; }
      if ((*i)[2] < min[2]) { min[2] = (*i)[2]; }
      if ((*i)[0] > max[0]) { max[0] = (*i)[0]; }
      if ((*i)[1] > max[1]) { max[1] = (*i)[1]; }
      if ((*i)[2] > max[2]) { max[2] = (*i)[2]; }
    }
  }
  // Get the size along the biggest axis
  QVector3D modelSize = max-min;
  float maxModelSize = INT_MIN;
  for (unsigned int i = 0; i < 3; ++i)
  {
    if (modelSize[i] > maxModelSize) { maxModelSize = modelSize[i]; }
  }
  // Set the scale to fit this model to the given size
  this->setScale(size / maxModelSize);
}

void Model_WithCalculations::initialize(
    std::vector<QVector3D> vertices, std::vector<QVector3D> triangles)
{
  // Base model initialize
  Model::initialize(vertices, triangles);
  this->isReady = false;
  // Store mesh data
  this->vertices = vertices;
  this->triangles = triangles;
  // Calculate the decal's projection and adjustment matrix (for projected texture style)
  this->decalAdjustAndProjectionTransform.translate(.5, .5, .5);
  this->decalAdjustAndProjectionTransform.scale(.5, .5, .5);
  this->decalAdjustAndProjectionTransform.perspective(45, 1, .1f, 1000);
  // Setup all the shaders
  for (vector<pair<QString,QString>>::iterator i = shaderSources.begin();
       i != shaderSources.end(); ++i)
  {
    QOpenGLShaderProgram* newShader = new QOpenGLShaderProgram();
    newShader->addShaderFromSourceFile(
        QOpenGLShader::Vertex, (*i).first);
    newShader->addShaderFromSourceFile(
        QOpenGLShader::Fragment, (*i).second);
    newShader->link();
    if(!newShader->isLinked())
    {
      qCritical() << "Shader error: <" << (*i).first << ", " << (*i).second << ">";
      return;
    }
    this->shaders.push_back(newShader);
  }
  currentShaderIndex = 0;
  shader = this->shaders[0];
  // Done
  isReady = true;
}

void Model_WithCalculations::draw(QMatrix4x4 projectionCameraTransform, QMatrix4x4 cameraTransform)
{
  // Calculate the decal's look matrix and decal normal (for projected texture style)
  QMatrix4x4 decalCameraTransform;
  decalCameraTransform.lookAt(this->focus, this->nearestPoint, QVector3D(0,1,0));
  QVector3D decalNormal = (this->nearestPoint - this->focus).normalized();
  // Flip front facing (to draw the back of the model)
  this->gl->glDepthMask(false);
  this->gl->glFrontFace(GL_CW);
  // Draw the back of the model
    this->shader->bind();
    // Shader uniforms: Texture & alpha
    this->shader->setUniformValue("alpha", 0.0f); // alpha=0: back won't be drawn, EXCEPT for decal
    this->shader->setUniformValue("mainTexture", 0);
    // Shader uniforms: Projected texture
    this->shader->setUniformValue(
        "decalAdjustAndProjectionTransform", this->decalAdjustAndProjectionTransform);
    this->shader->setUniformValue("decalCameraTransform", decalCameraTransform);
    this->shader->setUniformValue("decalNormal", decalNormal);
    // Shader uniforms: Distanced texture
    this->shader->setUniformValue("focus", this->focus);
    this->shader->setUniformValue("nearestPoint", this->nearestPoint);
    // Actual draw
    Model::draw(projectionCameraTransform, cameraTransform);
  // Flip front facing (to draw the front of the model)
  this->gl->glFrontFace(GL_CCW);
  this->gl->glDepthMask(true);
  // Draw the front of the model
    this->shader->bind();
    // Shader uniforms: Texture & alpha
    this->shader->setUniformValue("alpha", 0.5f); // alpha=.5: transparent, but still visible
    this->shader->setUniformValue("mainTexture", 0);
    // Shader uniforms: Projected texture
    this->shader->setUniformValue(
        "decalAdjustAndProjectionTransform", this->decalAdjustAndProjectionTransform);
    this->shader->setUniformValue("decalCameraTransform", decalCameraTransform);
    this->shader->setUniformValue("decalNormal", decalNormal);
    // Shader uniforms: Distanced texture
    this->shader->setUniformValue("focus", this->focus);
    this->shader->setUniformValue("nearestPoint", this->nearestPoint);
    // Actual draw
    Model::draw(projectionCameraTransform, cameraTransform);
}

QVector3D Model_WithCalculations::calcClosestSurfacePoint(QVector3D focus)
{
  QVector3D result;
  unsigned int resultDistance = INT_MAX;
  // Check ALL triangles for closest point
  // TODO: optimize to not check EVERY triangle (bsp or octree)
  for (vector<QVector3D>::iterator i = this->triangles.begin(); i != this->triangles.end(); ++i)
  {
    QVector3D v1 = this->transform * this->vertices[(*i)[0]];
    QVector3D v2 = this->transform * this->vertices[(*i)[1]];
    QVector3D v3 = this->transform * this->vertices[(*i)[2]];
    // Check the point
    QVector3D localResult = Model_WithCalculations::calcClosestPointOnTriangle(
        focus, v1, v2, v3);
    // Compare the result against the running best (shortest) distance
    float localResultDistance = (localResult - this->focus).length();
    if (localResultDistance < resultDistance)
    {
      result = localResult;
      resultDistance = localResultDistance;
    }
  }
  return result;
}

unsigned int Model_WithCalculations::getShaderCount() const
{
  return (unsigned int)this->shaders.size();
}
QVector3D Model_WithCalculations::getFocus() const { return this->focus; }
QVector3D Model_WithCalculations::getNearestPoint() const { return this->nearestPoint; }

void Model_WithCalculations::setCurrentShader(unsigned int index)
{
  if (index > this->shaders.size()-1)
  {
    qWarning() << "Shader set to invalid index: " << index;
    return;
  }
  this->shader = this->shaders[index];
}

void Model_WithCalculations::setFocus(QVector3D value)
{
  this->focus = value;
}

void Model_WithCalculations::setNearestPoint(QVector3D value)
{
  this->nearestPoint = value;
}

inline float clamp(float x, float a, float b)
{
  return x < a ? a : (x > b ? b : x);
}

QVector3D Model_WithCalculations::calcClosestPointOnTriangle(
    QVector3D point, QVector3D triangle1, QVector3D triangle2, QVector3D triangle3)
{
  //-------------------------------------------------------//
  // This code returns the cloests CORNER of the triangle, //
  // rather than the closest point along its face          //
  //-------------------------------------------------------//
  //  QVector3D result = tri1;
  //  float resultDistance = (point - result).length();
  //  float checkDistance = (point - tri2).length();
  //  if (checkDistance < resultDistance)
  //  {
  //    result = tri2;
  //    resultDistance = checkDistance;
  //  }
  //  checkDistance = (point - tri3).length();
  //  if (checkDistance < resultDistance)
  //  {
  //    result = tri3;
  //    resultDistance = checkDistance;
  //  }
  //  return result;

  //---------------------------------------------------------------//
  // This code returns the cloests point along the triangle's face //
  //---------------------------------------------------------------//
  QVector3D edge0 = triangle2 - triangle1;
  QVector3D edge1 = triangle3 - triangle1;
  QVector3D v0 = triangle1 - point;

  float a = QVector3D::dotProduct(edge0, edge0);
  float b = QVector3D::dotProduct(edge0, edge1);
  float c = QVector3D::dotProduct(edge1, edge1);
  float d = QVector3D::dotProduct(edge0, v0);
  float e = QVector3D::dotProduct(edge1, v0);

  float det = a*c - b*b;
  float s = b*e - c*d;
  float t = b*d - a*e;

  if ( s + t < det )
  {
    if ( s < 0.f )
    {
      if ( t < 0.f )
      {
        if ( d < 0.f )
        {
          s = clamp( -d/a, 0.f, 1.f );
          t = 0.f;
        }
        else
        {
          s = 0.f;
          t = clamp( -e/c, 0.f, 1.f );
        }
      }
      else
      {
        s = 0.f;
        t = clamp( -e/c, 0.f, 1.f );
      }
    }
    else if ( t < 0.f )
    {
      s = clamp( -d/a, 0.f, 1.f );
      t = 0.f;
    }
    else
    {
      float invDet = 1.f / det;
      s *= invDet;
      t *= invDet;
    }
  }
  else
  {
    if ( s < 0.f )
    {
      float tmp0 = b+d;
      float tmp1 = c+e;
      if ( tmp1 > tmp0 )
      {
        float numer = tmp1 - tmp0;
        float denom = a-2*b+c;
        s = clamp( numer/denom, 0.f, 1.f );
        t = 1-s;
      }
      else
      {
        t = clamp( -e/c, 0.f, 1.f );
        s = 0.f;
      }
    }
    else if ( t < 0.f )
    {
      if ( a+d > b+e )
      {
        float numer = c+e-b-d;
        float denom = a-2*b+c;
        s = clamp( numer/denom, 0.f, 1.f );
        t = 1-s;
      }
      else
      {
        s = clamp( -e/c, 0.f, 1.f );
        t = 0.f;
      }
    }
    else
    {
      float numer = c+e-b-d;
      float denom = a-2*b+c;
      s = clamp( numer/denom, 0.f, 1.f );
      t = 1.f - s;
    }
  }
  return triangle1 + s * edge0 + t * edge1;
}
