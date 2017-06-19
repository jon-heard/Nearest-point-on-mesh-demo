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

bool Scene::update() { return false; }

void Scene::draw(QMatrix4x4 transform_projection)
{
  QMatrix4x4 transform_projection_camera = transform_projection * this->transform;
  // Render all models
  for (vector<Model*>::iterator i = this->models.begin(); i != this->models.end(); ++i)
  {
    (*i)->draw(transform_projection_camera, this->transform);
  }
}

bool Scene::getIsInitialized() { return this->isInitialized; }
Model* Scene::getRightMouseRotatedModel() { return NULL; } // Overridden in subclass
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
  this->transform.rotate(rotation);
}
