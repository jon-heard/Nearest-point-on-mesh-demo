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
  refreshTransform();
  isInitialized = true;
}

void Scene::update() {}

void Scene::draw(QMatrix4x4 projectionTransform)
{
  QMatrix4x4 projectionCameraTransform = projectionTransform * transform;
  // Render all models
  for (vector<Model*>::iterator i = models.begin();
       i != models.end(); ++i)
  {
      (*i)->draw(projectionCameraTransform, transform);
  }
}

bool Scene::getIsInitialized() { return isInitialized; }
Model* Scene::getRightMouseRotatedModel() { return NULL; }
float Scene::getZoom() const { return this->zoom; }
QQuaternion Scene::getRotation() const { return this->rotation; }

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
