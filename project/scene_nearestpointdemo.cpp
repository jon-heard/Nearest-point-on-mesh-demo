#include "scene_nearestpointdemo.h"

#include "model.h"
#include <QOpenGLTexture>
#include "mainwindow.h"
#include "model_withcalculations.h"
#include "modelloader_primitive.h"
#include "modelloader_file_off.h"

using namespace std;

Scene_NearestPointDemo::Scene_NearestPointDemo(MainWindow* window) :
    model_mesh(NULL), isTargetSphereEnabled(true), newMeshFilename(""), window(window),
    decalType(0) {}

Scene_NearestPointDemo::~Scene_NearestPointDemo()
{
  this->targetTexture->destroy();
  delete this->targetTexture;
}

void Scene_NearestPointDemo::initialize(QOpenGLFunctions* gl)
{
  Scene::initialize(gl);
  // Setup models
    // Loaders
    ModelLoader_Primitive primitiveLoader;
    // Focus
    this->model_focus = new Model(gl, {":/shaders/basic.vert", ":/shaders/lightAndRed.frag"});
    primitiveLoader.loadSphereIntoModel(this->model_focus, 10, 3);
    this->models.push_back(this->model_focus);
    // NearestPoint
    this->model_nearestPoint = new Model(
        gl, {":/shaders/basic.vert", ":/shaders/lightAndBlue.frag"});
    primitiveLoader.loadSphereIntoModel(this->model_nearestPoint, 5, 3);
    this->models.push_back(this->model_nearestPoint);
    // Mesh
    this->loadMesh(":/other/default.off");
  // Setup decal texture
    this->targetTexture = new QOpenGLTexture(QImage(":/other/target.png"));
    this->targetTexture->setWrapMode(QOpenGLTexture::ClampToBorder);
    this->targetTexture->bind();
}

void Scene_NearestPointDemo::update()
{
  Scene::update();

  // Load mesh
  if (this->newMeshFilename != "")
  {
    loadMesh(this->newMeshFilename);
    window->setModelFilename(this->newMeshFilename.c_str());
    this->newMeshFilename = "";
  }

  targetTexture->bind();

  // Nearest point calculation
  QVector3D focus = this->model_focus->calcWorldPosition();
  QVector3D nearestPoint = this->model_mesh->calcClosestSurfacePoint(focus);
  this->model_nearestPoint->setPosition(nearestPoint);
  this->model_mesh->setFocus(focus);
  this->model_mesh->setNearestPoint(nearestPoint);
}

Model* Scene_NearestPointDemo::getRightMouseRotatedModel() { return this->model_focus; }
bool Scene_NearestPointDemo::getIsTargetSphereEnabled() { return this->isTargetSphereEnabled; }
unsigned int Scene_NearestPointDemo::getDecalType() const { return this->decalType; }

void Scene_NearestPointDemo::setIsTargetSphereEnabled(bool value)
{
  this->isTargetSphereEnabled = value;
  this->model_nearestPoint->setIsVisible(value);
}

void Scene_NearestPointDemo::setDecalType(unsigned int value)
{
  if (value > this->model_mesh->getShaderCount())
  {
    return;
  }
  this->decalType = value;
  this->model_mesh->setCurrentShader(value);
}

bool Scene_NearestPointDemo::loadMesh(string filename)
{
  bool result = true;
  // Keep track of old model_mesh, in case new one fails
  Model_WithCalculations* oldModel = this->model_mesh;
  // Remove old model_mesh
  if (this->model_mesh != NULL)
  {
    this->models.erase(std::find(this->models.begin(), this->models.end(), this->model_mesh));
  }
  // Create new model_mesh
  this->model_mesh = new Model_WithCalculations(
      gl, {
          {":/shaders/basic.vert", ":/shaders/mesh_basic.frag"},
          {":/shaders/mesh_projectedTexture.vert", ":/shaders/lightAndTextureProjAndAlpha.frag"},
          {":/shaders/mesh_distancedTexture.vert", ":/shaders/lightAndTextureAndAlpha.frag"}
  });
  // Load mesh data
  ModelLoader_File_OFF loader;
  if (!loader.loadFileIntoModel(this->model_mesh, filename))
  {
    qCritical() << "Failed to load file final.off";
    qCritical() << loader.getErrorMessage().c_str();
    this->model_mesh = oldModel;
    result = false;
  }
  //ModelLoader_Primitive loader;
  //primitiveLoader.loadBoxIntoModel(this->model_mesh, 100, 100, 100);
  // Add new model_mesh
  //this->window->setDecalTypeSelector(0);
  this->models.push_back(this->model_mesh);
  // Erase old model_mesh
  if (oldModel != this->model_mesh)
  {
    delete oldModel;
    oldModel = NULL;
  }
  model_mesh->scaleToFit(250);
  return result;
}

void Scene_NearestPointDemo::initiateMeshLoading(string filename)
{
  this->newMeshFilename = filename;
}
