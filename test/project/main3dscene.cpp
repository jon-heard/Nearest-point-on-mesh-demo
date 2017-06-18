#include "main3dscene.h"
#include <algorithm>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QOpenGLTexture>
#include <QDebug>
#include "model.h"
#include "model_withcalculations.h"
#include "model_loader_file_off.h"
#include "model_loader_primitive.h"
#include "mainwindow.h"

using namespace std;

const float FOCUS_ROTATION_SPEED = 5;

Main3DScene::Main3DScene(QWidget *parent) :
  QOpenGLWidget(parent), zoom(-500), isEnabled_targetSphere(true), model_mesh(NULL),
  newMeshFilename("")
{
  this->gl = new QOpenGLFunctions;
  setFocusPolicy(Qt::StrongFocus);
}

Main3DScene::~Main3DScene()
{
  delete this->gl;
  this->gl = NULL;
  while (this->models.size() > 0)
  {
    delete this->models.back();
    this->models.pop_back();
  }
  this->targetTexture->destroy();
  delete this->targetTexture;
}

void Main3DScene::refreshCameraTransform()
{
  this->cameraTransform.setToIdentity();
  this->cameraTransform.translate(0, 0, this->zoom);
  this->cameraTransform.rotate(QQuaternion::fromEulerAngles(rotation.x(), rotation.y(), 0));
  repaint();
}

void Main3DScene::initiateLoadMesh(std::string filename)
{
  newMeshFilename = filename;
}

void Main3DScene::initializeGL()
{
  // Setup opengl
  this->gl->initializeOpenGLFunctions();
  this->gl->glClearColor(.25,.25,.25,1);
  this->gl->glEnable(GL_DEPTH_TEST);
  this->gl->glEnable(GL_BLEND);
  this->gl->glEnable(GL_CULL_FACE);
  this->gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // Setup scene
  cameraTransform.setToIdentity();
  cameraTransform.translate(0, 0, zoom);
  // Setup models
    // loaders
    Model_Loader_Primitive primitiveLoader;
    // focus
    this->model_focus = new Model(
        gl, {":/shaders/basic.vert", ":/shaders/lightAndRed.frag"});
    primitiveLoader.loadSphereIntoModel(this->model_focus, 10, 3);
    this->models.push_back(this->model_focus);
    // nearestPoint
    this->model_nearestPoint = new Model(
        gl, {":/shaders/basic.vert", ":/shaders/lightAndBlue.frag"});
    primitiveLoader.loadSphereIntoModel(this->model_nearestPoint, 5, 3);
    this->models.push_back(this->model_nearestPoint);
    // mesh
    loadMesh(":/other/default.off");
    //loadMesh("D:/_projects/Nearest-point-on-mesh-demo/off files/Apple.off");
  // Setup decal texture
    this->targetTexture = new QOpenGLTexture(QImage(":/other/target.png"));
    this->targetTexture->setWrapMode(QOpenGLTexture::ClampToBorder);
    this->targetTexture->bind();
}

void Main3DScene::paintGL()
{
  if (newMeshFilename != "")
  {
    loadMesh(newMeshFilename);
    window->setModelFilename(newMeshFilename.c_str());
    newMeshFilename = "";
  }

  gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  targetTexture->bind();
  qInfo() << cameraTransform;
  // Camera transform calculation
  QMatrix4x4 projectionCameraTransform = projectionTransform * cameraTransform;
  // Nearest point calculation
  QVector3D focus = this->model_focus->calcWorldPosition();
  QVector3D nearestPoint = model_mesh->calcClosestSurfacePoint(focus);
  this->model_nearestPoint->setPosition(nearestPoint);
  // Render all models
  for (vector<Model*>::iterator i = this->models.begin(); i != this->models.end(); ++i)
  {
    if (*i != this->model_mesh)
    {
      (*i)->draw(projectionCameraTransform, cameraTransform);
    } else {
      // If model is the mesh, render with extra parameters
      ((Model_WithCalculations*)*i)->draw(
            projectionCameraTransform, cameraTransform,
            focus, nearestPoint);
    }
  }
}

void Main3DScene::resizeGL(int w, int h)
{
  this->projectionTransform.setToIdentity();
  this->projectionTransform.perspective(45, (float)w/h, 0.001f, 10000.0f);
}

void Main3DScene::mousePressEvent(QMouseEvent *event)
{
  this->previousMousePos = event->pos();
  if (event->buttons() & Qt::LeftButton)
  {
    this->previousRotation = rotation;
  }
  else if (event->buttons() & Qt::RightButton)
  {
    this->previousRotation = this->model_focus->getRotation();
  }
}

void Main3DScene::mouseMoveEvent(QMouseEvent *event)
{
  if (event->buttons() & Qt::LeftButton)
  {
    rotation.setX(this->previousRotation.x() + (event->y() - this->previousMousePos.y()));
    rotation.setY(this->previousRotation.y() + (event->x() - this->previousMousePos.x()));
    refreshCameraTransform();
  } else if (event->buttons() & Qt::RightButton) {
    QQuaternion focusRotation = this->model_focus->getRotation();
    focusRotation = QQuaternion::fromAxisAndAngle(1, 0, 0,
        this->previousRotation.x() + (event->y() - this->previousMousePos.y()));
    focusRotation *= QQuaternion::fromAxisAndAngle(0, 1, 0,
        this->previousRotation.y() + (event->x() - this->previousMousePos.x()));
    this->model_focus->setRotation(focusRotation);
    repaint();
  }
}

void Main3DScene::wheelEvent(QWheelEvent* event)
{
  this->zoom += event->delta() / 2;
  if (this->zoom > -2) { this->zoom = -2; }
  refreshCameraTransform();
}

void Main3DScene::keyPressEvent(QKeyEvent* event)
{
  switch (event->key())
  {
    case Qt::Key_Up:
      rotation.setX(rotation.x() - 10);
      refreshCameraTransform();
      break;
    case Qt::Key_Right:
      rotation.setY(rotation.y() + 10);
      refreshCameraTransform();
      break;
    case Qt::Key_Down:
      rotation.setX(rotation.x() + 10);
      refreshCameraTransform();
      break;
    case Qt::Key_Left:
      rotation.setY(rotation.y() - 10);
      refreshCameraTransform();
      break;
    case Qt::Key_Z:
      this->zoom += 20;
      if (this->zoom > -2) { this->zoom = -2; }
      refreshCameraTransform();
      break;
    case Qt::Key_X:
      this->zoom -= 20;
      if (this->zoom > -2) { this->zoom = -2; }
      refreshCameraTransform();
      break;
    case Qt::Key_W:
    {
      QQuaternion focusRotation = this->model_focus->getRotation();
      focusRotation *= QQuaternion::fromAxisAndAngle(1, 0, 0, -FOCUS_ROTATION_SPEED);
      this->model_focus->setRotation(focusRotation);
      repaint();
      break;
    }
    case Qt::Key_A:
    {
      QQuaternion focusRotation = this->model_focus->getRotation();
      focusRotation *= QQuaternion::fromAxisAndAngle(0, 1, 0, -FOCUS_ROTATION_SPEED);
      this->model_focus->setRotation(focusRotation);
      repaint();
      break;
    }
    case Qt::Key_S:
    {
      QQuaternion focusRotation = this->model_focus->getRotation();
      focusRotation *= QQuaternion::fromAxisAndAngle(1, 0, 0, FOCUS_ROTATION_SPEED);
      this->model_focus->setRotation(focusRotation);
      repaint();
      break;
    }
    case Qt::Key_D:
    {
      QQuaternion focusRotation = this->model_focus->getRotation();
      focusRotation *= QQuaternion::fromAxisAndAngle(0, 1, 0, FOCUS_ROTATION_SPEED);
      this->model_focus->setRotation(focusRotation);
      repaint();
      break;
    }
    case Qt::Key_Q:
    {
      QVector3D position = this->model_focus->getPosition();
      position += QVector3D(0, 0, 10);
      this->model_focus->setPosition(position);
      repaint();
      break;
    }
    case Qt::Key_E:
    {
      QVector3D position = this->model_focus->getPosition();
      position += QVector3D(0, 0, -10);
      this->model_focus->setPosition(position);
      repaint();
      break;
    }
  }
}

bool Main3DScene::getIsEnabled_targetSphere() { return this->isEnabled_targetSphere; }
Model_WithCalculations* Main3DScene::getMesh() { return this->model_mesh; }
MainWindow* Main3DScene::getMainWindow() { return this->window; }

void Main3DScene::setIsEnabled_targetSphere(bool value)
{
  this->isEnabled_targetSphere = value;
  this->model_nearestPoint->setIsVisible(value);
  repaint();
}

void Main3DScene::setMainWindow(MainWindow* value)
{
  this->window = value;
}

bool Main3DScene::loadMesh(string filename)
{
  bool result = true;
  Model_Loader_File_OFF fileLoader;
  // Keep track of old model_mesh, in case new one fails
  Model_WithCalculations* oldModel = model_mesh;
  // Remove old model_mesh
  if (model_mesh != NULL)
  {
    models.erase(std::find(models.begin(), models.end(), model_mesh));
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
  this->window->setDecalTypeSelector(0);
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