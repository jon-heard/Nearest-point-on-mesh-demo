#include "scene.h"
#include <QOpenGLFunctions>
#include "model.h"

using namespace std;

Scene::Scene() : zoom(-500), isInitialized(false) {}

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
  this->refreshTransform();
  this->isInitialized = true;
}

void Scene::update() {}

void Scene::draw(QMatrix4x4 projectionTransform)
{
  QMatrix4x4 projectionCameraTransform = projectionTransform * this->transform;
  // Render all models
  for (vector<Model*>::iterator i = this->models.begin(); i != this->models.end(); ++i)
  {
    (*i)->draw(projectionCameraTransform, this->transform);
  }
}

bool Scene::getIsInitialized() { return this->isInitialized; }
Model* Scene::getRightMouseRotatedModel() { return NULL; }
float Scene::getZoom() const { return this->zoom; }
QQuaternion Scene::getRotation() const { return this->rotation; }

void Scene::setZoom(float value)
{
  this->zoom = value;
  if (this->zoom > -2) { this->zoom = -2; }
  this->refreshTransform();
}

void Scene::setRotation(QQuaternion value)
{
  this->rotation = value;
  this->refreshTransform();
}

void Scene::refreshTransform()
{
  this->transform.setToIdentity();
  this->transform.translate(0, 0, this->zoom);
  //this->transform.rotate(rotation); // TODO: Fix quaternion calculations
  this->transform.rotate(QQuaternion::fromEulerAngles(this->rotation.x(), this->rotation.y(), 0));
}