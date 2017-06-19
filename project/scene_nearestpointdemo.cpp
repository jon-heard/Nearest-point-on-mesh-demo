#include "scene_nearestpointdemo.h"

#include "model.h"
#include <QOpenGLTexture>
#include "mainwindow.h"
#include "model_withcalculations.h"
#include "modelloader_primitive.h"
#include "modelloader_file_off.h"

using namespace std;

const QString STARTING_MESH_FILE = "DEFAULT";
//const QString STARTING_MESH_FILE = "LOW_POLY_SPHERE";
//const QString STARTING_MESH_FILE = "D:/_projects/Nearest-point-on-mesh-demo/off files/helm.off";
const int DEAULT_DECAL_TYPE = 3;

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
    this->model_focus->setPosition({0, 0, 150});
    // NearestPoint
    this->model_nearestPoint = new Model(
        gl, {":/shaders/basic.vert", ":/shaders/lightAndBlue.frag"});
    primitiveLoader.loadSphereIntoModel(this->model_nearestPoint, 5, 3);
    this->models.push_back(this->model_nearestPoint);
    // Mesh
    this->loadMesh(STARTING_MESH_FILE);
  // Setup decal texture
    this->targetTexture = new QOpenGLTexture(QImage(":/other/target.png"));
    this->targetTexture->setWrapMode(QOpenGLTexture::ClampToBorder);
    this->targetTexture->bind();
  // Tell UI which file we are viewing
    window->setModelFilename(STARTING_MESH_FILE);
}

bool Scene_NearestPointDemo::update()
{
  bool result = false;
  // Superclass Scene update
  Scene::update();
  // Load new mesh, if requested
  if (this->newMeshFilename != "")
  {
    if (loadMesh(this->newMeshFilename))
    {
      window->setModelFilename(this->newMeshFilename);
    }
    this->newMeshFilename = "";
    // Return true to force a repaint.  See SceneUi.paintGL() for more info.
    result = true;
  }
  // Prepare our only texture
  targetTexture->bind();
  // Nearest point calculation
  QVector3D focus = this->model_focus->calcWorldPosition();
  QVector3D nearestPoint = this->model_mesh->calcClosestSurfacePoint(focus);
  // Update the position of the nearest-point sphere
  this->model_nearestPoint->setPosition(nearestPoint);
  // Update values of focus and nearest point for mesh to render decal
  this->model_mesh->setFocus(focus);
  this->model_mesh->setNearestPoint(nearestPoint);
  return result;
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

bool Scene_NearestPointDemo::loadMesh(QString filename)
{
  bool result = true;
  // Keep track of old mesh, in case new one fails to load right
  Model_WithCalculations* oldModel = this->model_mesh;
  // Remove old mesh from the model list (if it exists)
  if (this->model_mesh != NULL)
  {
    this->models.erase(std::find(this->models.begin(), this->models.end(), this->model_mesh));
  }
  // Create new model_mesh...
  this->model_mesh = new Model_WithCalculations(
      gl,
      {{":/shaders/basic.vert", ":/shaders/mesh_basic.frag"},
       {":/shaders/mesh_projectedTexture.vert", ":/shaders/lightAndTextureProjAndAlpha.frag"},
       {":/shaders/mesh_projectedTexture_truncated.vert",
        ":/shaders/lightAndTextureProjAndAlpha.frag"},
       {":/shaders/mesh_projectedTexture_truncated_noBackface.vert",
        ":/shaders/lightAndTextureProjAndAlpha.frag"}});
  // ... and load mesh data (based on filename)
  if (filename == "LOW_POLY_SPHERE")
  {
    // Load primitive data
    ModelLoader_Primitive loader;
    loader.loadSphereIntoModel(this->model_mesh, 1, 1);
  }
  else if (filename == "DEFAULT")
  {
    // load mesh data from a file.  If loading fails, revert back to the old mesh
    ModelLoader_File_OFF loader;
    if (!loader.loadFileIntoModel(this->model_mesh, ":other/default.off"))
    {
      qCritical() << "Failed to load file final.off";
      qCritical() << loader.getErrorMessage();
      this->model_mesh = oldModel;
      result = false;
    }
  }
  else
  {
    // load mesh data from a file.  If loading fails, revert back to the old mesh
    ModelLoader_File_OFF loader;
    if (!loader.loadFileIntoModel(this->model_mesh, filename))
    {
      qCritical() << "Failed to load file final.off";
      qCritical() << loader.getErrorMessage();
      this->model_mesh = oldModel;
      result = false;
    }
  }
  // If mesh was loaded successfully, erase the old mesh
  if (result == true)
  {
    delete oldModel;
    oldModel = NULL;
  }
  // Size mesh to fit
  model_mesh->scaleToFit(250);
  // Add mesh to model list
  this->models.push_back(this->model_mesh);
  // Set the decal type of the mesh (and update the ui to match)
  model_mesh->setCurrentShader(DEAULT_DECAL_TYPE);
  this->window->setDecalTypeSelector(DEAULT_DECAL_TYPE);

  // Done.  Return whether new mesh was loaded or not
  return result;
}

void Scene_NearestPointDemo::initiateMeshLoading(QString filename)
{
  this->newMeshFilename = filename;
}
