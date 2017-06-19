#include "sceneui.h"
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
#include "modelloader_file_off.h"
#include "modelloader_primitive.h"
#include "mainwindow.h"
#include "scene_nearestpointdemo.h"

using namespace std;

const float MOUSE_ROTATION_SPEED = 1;
const float KEY_ROTATION_SPEED = 10;
const float MOUSE_ZOOM_SPEED = .5;
const float KEY_ZOOM_SPEED = 60;
const float KEY_MOVE_SPEED = 10;

SceneUi::SceneUi(QWidget *parent) : QOpenGLWidget(parent), scene(NULL)
{
  this->gl = new QOpenGLFunctions;
  setFocusPolicy(Qt::StrongFocus);
}

SceneUi::~SceneUi()
{
  delete this->gl;
  this->gl = NULL;
  delete this->scene;
  this->scene = NULL;
}

void SceneUi::initializeGL()
{
  // Setup opengl
  this->gl->initializeOpenGLFunctions();
  this->gl->glClearColor(.25,.25,.25,1);
  this->gl->glEnable(GL_DEPTH_TEST);
  this->gl->glEnable(GL_BLEND);
  this->gl->glEnable(GL_CULL_FACE);
  this->gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // Setup scene
  if (this->scene != NULL)
  {
    this->scene->initialize(gl);
  }
}

void SceneUi::paintGL()
{
  if (this->scene == NULL) { return; }
  if (!this->scene->getIsInitialized()) { this->scene->initialize(gl); }
  if (this->scene->update())
  {
    // Scene.update returns true.  This indicates that a mesh was just loaded, which requires
    // forcing the UI to repaint.  This fixes a small bug wherein the UI gets whited out and the
    // nearest-point calculation is bad.
    this->window->repaint();
    this->repaint();
  }
  gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  this->scene->draw(this->projectionTransform);
}

void SceneUi::resizeGL(int w, int h)
{
  this->projectionTransform.setToIdentity();
  this->projectionTransform.perspective(45, (float)w/h, 0.001f, 10000.0f);
}

void SceneUi::mousePressEvent(QMouseEvent *event)
{
  this->previousMousePos = QVector2D(event->pos());
  if (event->buttons() & Qt::LeftButton)
  {
    this->previousRotation = this->scene->getRotation();
  }
  else if (event->buttons() & Qt::RightButton && this->scene->getRightMouseRotatedModel() != NULL)
  {
    this->previousRotation = this->scene->getRightMouseRotatedModel()->getRotation();
  }
}

void SceneUi::mouseMoveEvent(QMouseEvent *event)
{
  if (event->buttons() & Qt::LeftButton)
  {
    QVector2D mouseDelta = QVector2D(event->pos()) - this->previousMousePos;
    this->scene->setRotation(
        QQuaternion::fromAxisAndAngle(
            mouseDelta.y(), mouseDelta.x(), 0,
            mouseDelta.length()*MOUSE_ROTATION_SPEED) *
        this->previousRotation);
    repaint();
    qInfo() << this->scene->getRotation().inverted() * QVector3D(0,1,0) << " :: " << this->scene->getRotation().inverted() * QVector3D(1,0,0);
    mousePressEvent(event); // reset offsets for the next move
  }
  else if (event->buttons() & Qt::RightButton && this->scene->getRightMouseRotatedModel() != NULL)
  {
    QVector2D mouseDelta = QVector2D(event->pos()) - this->previousMousePos;
    QQuaternion invertedSceneRotation = this->scene->getRotation().inverted();
    this->scene->getRightMouseRotatedModel()->setRotation(
         QQuaternion::fromAxisAndAngle(
            invertedSceneRotation * QVector3D(0,mouseDelta.x(),0) +
            invertedSceneRotation * QVector3D(mouseDelta.y(),0,0),
            mouseDelta.length()*MOUSE_ROTATION_SPEED) *
        this->previousRotation);
    repaint();
    mousePressEvent(event); // reset offsets for the next move
  }
}

void SceneUi::wheelEvent(QWheelEvent* event)
{
  float zoom = this->scene->getZoom();
  zoom += event->delta() * MOUSE_ZOOM_SPEED;
  this->scene->setZoom(zoom);
  repaint();
}

void SceneUi::keyPressEvent(QKeyEvent* event)
{
  switch (event->key())
  {
    case Qt::Key_Up:
    {
      this->scene->setRotation(
          QQuaternion::fromAxisAndAngle(1, 0, 0, -KEY_ROTATION_SPEED) *
          this->scene->getRotation());
      repaint();
      break;
    }
    case Qt::Key_Right:
    {
      this->scene->setRotation(
          QQuaternion::fromAxisAndAngle(0, -1, 0, -KEY_ROTATION_SPEED) *
          this->scene->getRotation());
      repaint();
      break;
    }
    case Qt::Key_Down:
    {
      this->scene->setRotation(
          QQuaternion::fromAxisAndAngle(1, 0, 0, KEY_ROTATION_SPEED) *
          this->scene->getRotation());
      repaint();
      break;
    }
    case Qt::Key_Left:
    {
      this->scene->setRotation(
          QQuaternion::fromAxisAndAngle(0, 1, 0, -KEY_ROTATION_SPEED) *
          this->scene->getRotation());
      repaint();
      break;
    }
    case Qt::Key_Z:
    {
      float zoom = this->scene->getZoom();
      zoom += KEY_ZOOM_SPEED;
      this->scene->setZoom(zoom);
      repaint();
      break;
    }
    case Qt::Key_X:
    {
      float zoom = this->scene->getZoom();
      zoom -= KEY_ZOOM_SPEED;
      this->scene->setZoom(zoom);
      repaint();
      break;
    }
    case Qt::Key_W:
    {
      Model* model = this->scene->getRightMouseRotatedModel();
      if (model != NULL)
      {
        model->setRotation(
            QQuaternion::fromAxisAndAngle(1, 0, 0, -KEY_ROTATION_SPEED) *
            model->getRotation());
        repaint();
      }
      break;
    }
    case Qt::Key_A:
    {
      Model* model = this->scene->getRightMouseRotatedModel();
      if (model != NULL)
      {
        model->setRotation(
            QQuaternion::fromAxisAndAngle(0, 1, 0, -KEY_ROTATION_SPEED) *
            model->getRotation());
        repaint();
      }
      break;
    }
    case Qt::Key_S:
    {
      Model* model = this->scene->getRightMouseRotatedModel();
      if (model != NULL)
      {
        model->setRotation(
            QQuaternion::fromAxisAndAngle(1, 0, 0, KEY_ROTATION_SPEED) *
            model->getRotation());
        repaint();
      }
      break;
    }
    case Qt::Key_D:
    {
      Model* model = this->scene->getRightMouseRotatedModel();
      if (model != NULL)
      {
        model->setRotation(
            QQuaternion::fromAxisAndAngle(0, 1, 0, KEY_ROTATION_SPEED) *
            model->getRotation());
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
        position += QVector3D(0, 0, KEY_MOVE_SPEED);
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
        position += QVector3D(0, 0, -KEY_MOVE_SPEED);
        model->setPosition(position);
        repaint();
      }
      break;
    }
  }
}

Scene_NearestPointDemo* SceneUi::getScene() { return this->scene; }
MainWindow* SceneUi::getWindow() { return this->window; }

void SceneUi::setScene(Scene_NearestPointDemo* value)
{
  delete scene;
  this->scene = value;
}

void SceneUi::setWindow(MainWindow* value)
{
  this->window = value;
}
