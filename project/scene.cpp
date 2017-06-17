#include "scene.h"
#include <QOpenGLFunctions>
#include "model.h"

Scene::Scene() :
  zoom(-500)
{}

Scene::~Scene()
{
  while (this->models.size() > 0)
  {
    delete this->models.back();
    this->models.pop_back();
  }
}

void Scene::initialize(QOpenGLFunctions* gl)
{
  this->gl = gl;
  refreshTransform();
}

void Scene::update()
{

}

float Scene::getZoom() const { return this->zoom; }
QQuaternion Scene::getRotation() const { return this->rotation; }
QMatrix4x4 Scene::getTransform() const { return this->transform; }
std::vector<Model*>* Scene::getModels() { return &this->models; }

void Scene::setZoom(float value)
{
  this->zoom = value;
  if (this->zoom > -2) { this->zoom = -2; }
  refreshTransform();
}

void Scene::setRotation(QQuaternion value)
{
  this->rotation = value;
  refreshTransform();
}

void Scene::refreshTransform()
{
  this->transform.setToIdentity();
  this->transform.translate(0, 0, this->zoom);
  //this->transform.rotate(rotation);
  this->transform.rotate(QQuaternion::fromEulerAngles(rotation.x(), rotation.y(), 0));
}
