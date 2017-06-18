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
  QOpenGLWidget(parent), scene(NULL)
{
  this->gl = new QOpenGLFunctions;
  setFocusPolicy(Qt::StrongFocus);
}

Scene_Renderer::~Scene_Renderer()
{
  delete this->gl;
  this->gl = NULL;
  delete scene;
  scene = NULL;
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
  if (scene != NULL)
  {
    scene->initialize(gl);
  }
}

void Scene_Renderer::paintGL()
{
  if (scene == NULL) { return; }
  if (!scene->getIsInitialized()) { scene->initialize(gl); }
  this->scene->update();

  // Render
  gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  this->scene->draw(projectionTransform);
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
  else if (event->buttons() & Qt::RightButton &&
           this->scene->getRightMouseRotatedModel() != NULL)
  {
    this->previousRotation = this->scene->getRightMouseRotatedModel()->getRotation();
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
  } else if (event->buttons() & Qt::RightButton &&
             this->scene->getRightMouseRotatedModel() != NULL) {
    QQuaternion rotation = this->scene->getRightMouseRotatedModel()->getRotation();
    rotation.setX(this->previousRotation.x() + (event->y() - this->previousMousePos.y()));
    rotation.setY(this->previousRotation.y() + (event->x() - this->previousMousePos.x()));
    this->scene->getRightMouseRotatedModel()->setRotation(rotation);
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
      Model* model = this->scene->getRightMouseRotatedModel();
      if (model != NULL)
      {
        QQuaternion focusRotation = model->getRotation();
        focusRotation *= QQuaternion::fromAxisAndAngle(1, 0, 0, -FOCUS_ROTATION_SPEED);
        model->setRotation(focusRotation);
        repaint();
      }
      break;
    }
    case Qt::Key_A:
    {
      Model* model = this->scene->getRightMouseRotatedModel();
      if (model != NULL)
      {
        QQuaternion focusRotation = model->getRotation();
        focusRotation *= QQuaternion::fromAxisAndAngle(0, 1, 0, -FOCUS_ROTATION_SPEED);
        model->setRotation(focusRotation);
        repaint();
      }
      break;
    }
    case Qt::Key_S:
    {
      Model* model = this->scene->getRightMouseRotatedModel();
      if (model != NULL)
      {
        QQuaternion focusRotation = model->getRotation();
        focusRotation *= QQuaternion::fromAxisAndAngle(1, 0, 0, FOCUS_ROTATION_SPEED);
        model->setRotation(focusRotation);
        repaint();
      }
      break;
    }
    case Qt::Key_D:
    {
      Model* model = this->scene->getRightMouseRotatedModel();
      if (model != NULL)
      {
        QQuaternion focusRotation = model->getRotation();
        focusRotation *= QQuaternion::fromAxisAndAngle(0, 1, 0, FOCUS_ROTATION_SPEED);
        model->setRotation(focusRotation);
        repaint();
      }
      break;
    }
    case Qt::Key_Q:
    {
      Model* model = this->scene->getRightMouseRotatedModel();
      if (model != NULL)
      {
        QVector3D position = model->getPosition();
        position += QVector3D(0, 0, 10);
        model->setPosition(position);
        repaint();
      }
      break;
    }
    case Qt::Key_E:
    {
      Model* model = this->scene->getRightMouseRotatedModel();
      if (model != NULL)
      {
        QVector3D position = model->getPosition();
        position += QVector3D(0, 0, -10);
        model->setPosition(position);
        repaint();
      }
      break;
    }
  }
}

Scene_NearestPointDemo* Scene_Renderer::getScene() { return this->scene; }
MainWindow* Scene_Renderer::getWindow() { return this->window; }

void Scene_Renderer::setScene(Scene_NearestPointDemo* value)
{
  delete scene;
  this->scene = value;
}

void Scene_Renderer::setWindow(MainWindow* value)
{
  this->window = value;
}
