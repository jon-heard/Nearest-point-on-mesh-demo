#include "scene_renderer.h"
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
#include "scene_nearestpointdemo.h"

using namespace std;

const float FOCUS_ROTATION_SPEED = 5;

Scene_Renderer::Scene_Renderer(QWidget *parent) :
  QOpenGLWidget(parent), scene(NULL), isEnabled_targetSphere(true),
  newMeshFilename("")
{
  this->gl = new QOpenGLFunctions;
  setFocusPolicy(Qt::StrongFocus);
}

Scene_Renderer::~Scene_Renderer()
{
  delete scene;
  scene = NULL;
  delete this->gl;
  this->gl = NULL;
}

void Scene_Renderer::initiateLoadMesh(std::string filename)
{
  newMeshFilename = filename;
}

void Scene_Renderer::initializeGL()
{
  // Setup opengl
  this->gl->initializeOpenGLFunctions();
  this->gl->glClearColor(.25,.25,.25,1);
  this->gl->glEnable(GL_DEPTH_TEST);
  this->gl->glEnable(GL_BLEND);
  this->gl->glEnable(GL_CULL_FACE);
  this->gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // Setup scene
  scene->initialize(gl);
}

void Scene_Renderer::paintGL()
{
  // Load mesh
  if (newMeshFilename != "")
  {
    //loadMesh(newMeshFilename);
    window->setModelFilename(newMeshFilename.c_str());
    newMeshFilename = "";
  }

  this->scene->update();

  // Render
  gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // Camera transform calculation
  QMatrix4x4 projectionCameraTransform = projectionTransform * scene->getTransform();
  // Nearest point calculation
  QVector3D focus = this->scene->getModel_focus()->calcWorldPosition();
  QVector3D nearestPoint = this->scene->getModel_mesh()->calcClosestSurfacePoint(focus);
  this->scene->getModel_nearestPoint()->setPosition(nearestPoint);
  this->scene->getModel_mesh()->setFocus(focus);
  this->scene->getModel_mesh()->setNearestPoint(nearestPoint);
  // Render all models
  for (vector<Model*>::iterator i = this->scene->getModels()->begin(); i != this->scene->getModels()->end(); ++i)
  {
      (*i)->draw(projectionCameraTransform, scene->getTransform());
  }
}

void Scene_Renderer::resizeGL(int w, int h)
{
  this->projectionTransform.setToIdentity();
  this->projectionTransform.perspective(45, (float)w/h, 0.001f, 10000.0f);
}

void Scene_Renderer::mousePressEvent(QMouseEvent *event)
{
  this->previousMousePos = event->pos();
  if (event->buttons() & Qt::LeftButton)
  {
    this->previousRotation = scene->getRotation();
  }
  else if (event->buttons() & Qt::RightButton)
  {
    this->previousRotation = this->scene->getModel_focus()->getRotation();
  }
}

void Scene_Renderer::mouseMoveEvent(QMouseEvent *event)
{
  if (event->buttons() & Qt::LeftButton)
  {
    QQuaternion rotation = this->scene->getRotation();
    rotation.setX(this->previousRotation.x() + (event->y() - this->previousMousePos.y()));
    rotation.setY(this->previousRotation.y() + (event->x() - this->previousMousePos.x()));
    this->scene->setRotation(rotation);
    repaint();
  } else if (event->buttons() & Qt::RightButton) {
    QQuaternion rotation = this->scene->getModel_focus()->getRotation();
    rotation.setX(this->previousRotation.x() + (event->y() - this->previousMousePos.y()));
    rotation.setY(this->previousRotation.y() + (event->x() - this->previousMousePos.x()));
    this->scene->getModel_focus()->setRotation(rotation);
    repaint();
  }
}

void Scene_Renderer::wheelEvent(QWheelEvent* event)
{
  float zoom = this->scene->getZoom();
  zoom += event->delta() / 2;
  this->scene->setZoom(zoom);
  repaint();
}

void Scene_Renderer::keyPressEvent(QKeyEvent* event)
{
  switch (event->key())
  {
    case Qt::Key_Up:
    {
      QQuaternion rotation = this->scene->getRotation();
      rotation.setX(rotation.x() - 10);
      this->scene->setRotation(rotation);
      repaint();
      break;
    }
    case Qt::Key_Right:
    {
      QQuaternion rotation = this->scene->getRotation();
      rotation.setY(rotation.y() + 10);
      this->scene->setRotation(rotation);
      repaint();
      break;
    }
    case Qt::Key_Down:
    {
      QQuaternion rotation = this->scene->getRotation();
      rotation.setX(rotation.x() + 10);
      this->scene->setRotation(rotation);
      repaint();
      break;
    }
    case Qt::Key_Left:
    {
      QQuaternion rotation = this->scene->getRotation();
      rotation.setY(rotation.y() - 10);
      this->scene->setRotation(rotation);
      repaint();
      break;
    }
    case Qt::Key_Z:
    {
      float zoom = this->scene->getZoom();
      zoom += 20;
      this->scene->setZoom(zoom);
      repaint();
      break;
    }
    case Qt::Key_X:
    {
      float zoom = this->scene->getZoom();
      zoom -= 20;
      this->scene->setZoom(zoom);
      repaint();
      break;
    }
    case Qt::Key_W:
    {
      QQuaternion focusRotation = this->scene->getModel_focus()->getRotation();
      focusRotation *= QQuaternion::fromAxisAndAngle(1, 0, 0, -FOCUS_ROTATION_SPEED);
      this->scene->getModel_focus()->setRotation(focusRotation);
      repaint();
      break;
    }
    case Qt::Key_A:
    {
      QQuaternion focusRotation = this->scene->getModel_focus()->getRotation();
      focusRotation *= QQuaternion::fromAxisAndAngle(0, 1, 0, -FOCUS_ROTATION_SPEED);
      this->scene->getModel_focus()->setRotation(focusRotation);
      repaint();
      break;
    }
    case Qt::Key_S:
    {
      QQuaternion focusRotation = this->scene->getModel_focus()->getRotation();
      focusRotation *= QQuaternion::fromAxisAndAngle(1, 0, 0, FOCUS_ROTATION_SPEED);
      this->scene->getModel_focus()->setRotation(focusRotation);
      repaint();
      break;
    }
    case Qt::Key_D:
    {
      QQuaternion focusRotation = this->scene->getModel_focus()->getRotation();
      focusRotation *= QQuaternion::fromAxisAndAngle(0, 1, 0, FOCUS_ROTATION_SPEED);
      this->scene->getModel_focus()->setRotation(focusRotation);
      repaint();
      break;
    }
    case Qt::Key_Q:
    {
      QVector3D position = this->scene->getModel_focus()->getPosition();
      position += QVector3D(0, 0, 10);
      this->scene->getModel_focus()->setPosition(position);
      repaint();
      break;
    }
    case Qt::Key_E:
    {
      QVector3D position = this->scene->getModel_focus()->getPosition();
      position += QVector3D(0, 0, -10);
      this->scene->getModel_focus()->setPosition(position);
      repaint();
      break;
    }
  }
}

Scene_NearestPointDemo* Scene_Renderer::getScene() { return this->scene; }
bool Scene_Renderer::getIsEnabled_targetSphere() { return this->isEnabled_targetSphere; }
MainWindow* Scene_Renderer::getMainWindow() { return this->window; }

void Scene_Renderer::setScene(Scene_NearestPointDemo* value)
{
  delete scene;
  this->scene = value;
}

void Scene_Renderer::setIsEnabled_targetSphere(bool value)
{
  this->isEnabled_targetSphere = value;
  this->scene->getModel_nearestPoint()->setIsVisible(value);
  repaint();
}

void Scene_Renderer::setMainWindow(MainWindow* value)
{
  this->window = value;
}
