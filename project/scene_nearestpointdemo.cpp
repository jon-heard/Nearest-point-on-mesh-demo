#include "scene_nearestpointdemo.h"

#include "model.h"
#include <QOpenGLTexture>
#include "mainwindow.h"
#include "model_withcalculations.h"
#include "modelloader_primitive.h"
#include "modelloader_file_off.h"

using namespace std;

Scene_NearestPointDemo::Scene_NearestPointDemo(MainWindow* window) :
  model_mesh(NULL), isTargetSphereEnabled(true), newMeshFilename(""), window(window), decalType(0)
{}

Scene_NearestPointDemo::~Scene_NearestPointDemo()
{
  this->targetTexture->destroy();
  delete this->targetTexture;
}

void Scene_NearestPointDemo::initialize(QOpenGLFunctions* gl)
{
  Scene::initialize(gl);
  // Setup models
    // loaders
    ModelLoader_Primitive primitiveLoader;
    // focus
    this->model_focus = new Model(
        gl, {":/shaders/basic.vert", ":/shaders/lightAndRed.frag"});
    primitiveLoader.loadSphereIntoModel(this->model_focus, 10, 3);
    models.push_back(this->model_focus);
    // nearestPoint
    this->model_nearestPoint = new Model(
        gl, {":/shaders/basic.vert", ":/shaders/lightAndBlue.frag"});
    primitiveLoader.loadSphereIntoModel(this->model_nearestPoint, 5, 3);
    models.push_back(this->model_nearestPoint);
    // mesh
    this->loadMesh(":/other/default.off");
    //loadMesh("D:/_projects/Nearest-point-on-mesh-demo/off files/Apple.off");
  // Setup decal texture
    this->targetTexture = new QOpenGLTexture(QImage(":/other/target.png"));
    this->targetTexture->setWrapMode(QOpenGLTexture::ClampToBorder);
    this->targetTexture->bind();
}

void Scene_NearestPointDemo::update()
{
  Scene::update();

  // Load mesh
  if (newMeshFilename != "")
  {
    loadMesh(newMeshFilename);
    window->setModelFilename(newMeshFilename.c_str());
    newMeshFilename = "";
  }

  targetTexture->bind();

  // Nearest point calculation
  QVector3D focus = this->model_focus->calcWorldPosition();
  QVector3D nearestPoint = this->model_mesh->calcClosestSurfacePoint(focus);
  this->model_nearestPoint->setPosition(nearestPoint);
  this->model_mesh->setFocus(focus);
  this->model_mesh->setNearestPoint(nearestPoint);
}

Model* Scene_NearestPointDemo::getRightMouseRotatedModel() { return model_focus; }
//Model_WithCalculations* Scene_NearestPointDemo::getModel_mesh() { return model_mesh; }
//Model* Scene_NearestPointDemo::getModel_focus() { return model_focus; }
//Model* Scene_NearestPointDemo::getModel_nearestPoint() { return model_nearestPoint; }
//QOpenGLTexture* Scene_NearestPointDemo::getTargetTexture() { return targetTexture; }
bool Scene_NearestPointDemo::getIsTargetSphereEnabled() { return this->isTargetSphereEnabled; }
int Scene_NearestPointDemo::getDecalType() const { return this->decalType; }

void Scene_NearestPointDemo::setIsTargetSphereEnabled(bool value)
{
  this->isTargetSphereEnabled = value;
  this->model_nearestPoint->setIsVisible(value);
}

void Scene_NearestPointDemo::setDecalType(unsigned int value)
{
  if (value > model_mesh->getShaderCount())
  {
    return;
  }
  this->decalType = value;
  model_mesh->setCurrentShader(value);
}

bool Scene_NearestPointDemo::loadMesh(string filename)
{
  bool result = true;
  ModelLoader_File_OFF fileLoader;
  // Keep track of old model_mesh, in case new one fails
  Model_WithCalculations* oldModel = model_mesh;
  // Remove old model_mesh
  if (model_mesh != NULL)
  {
    this->models.erase(std::find(this->models.begin(), this->models.end(), model_mesh));
  }
  // Create new model_mesh
  this->model_mesh = new Model_WithCalculations(
      gl, {
          {":/shaders/basic.vert", ":/shaders/mesh_basic.frag"},
          {":/shaders/mesh_projectedTexture.vert", ":/shaders/lightAndTextureProjAndAlpha.frag"},
          {":/shaders/mesh_distancedTexture.vert", ":/shaders/lightAndTextureAndAlpha.frag"}
  });
  // Load mesh data
  if (!fileLoader.loadFileIntoModel(this->model_mesh, filename))
  {
    qCritical() << "Failed to load file final.off";
    qCritical() << fileLoader.getErrorMessage().c_str();
    model_mesh = oldModel;
    result = false;
  }
  // primitiveLoader.loadBoxIntoModel(this->model_mesh, 100, 100, 100);
  // Add new model_mesh
  //this->window->setDecalTypeSelector(0);
  this->models.push_back(this->model_mesh);
  // Erase old model_mesh
  if (oldModel != model_mesh)
  {
    delete oldModel;
    oldModel = NULL;
  }
  model_mesh->scaleToFit(250);
  return result;
}

void Scene_NearestPointDemo::initiateMeshLoading(string filename)
{
  newMeshFilename = filename;
}
