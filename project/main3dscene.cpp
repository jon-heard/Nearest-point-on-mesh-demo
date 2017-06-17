#include "main3dscene.h"
#include "model.h"
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QOpenGLTexture>
#include <QDebug>
#include "model_loader_file_off.h"
#include "model_loader_primitive.h"
#include "mainwindow.h"

using namespace std;

const float FOCUS_ROTATION_SPEED = 5;

Main3DScene::Main3DScene(QWidget *parent) :
  QOpenGLWidget(parent), zoom(-500), isEnabled_targetSphere(true)
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
  this->target->destroy();
  delete this->target;
}

void Main3DScene::refreshCameraTransform()
{
  this->cameraTransform.setToIdentity();
  this->cameraTransform.translate(0, 0, this->zoom);
  this->cameraTransform.rotate(QQuaternion::fromEulerAngles(rotation.x(), rotation.y(), 0));
  repaint();
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
    Model_Loader_File_OFF fileLoader;
    Model_Loader_Primitive primitiveLoader;
    // focus
    this->model_focus = new Model(
        gl, {":/shaders/basic.vert", ":/shaders/lightAndRed.frag"});
    primitiveLoader.loadSphereIntoModel(this->model_focus, 10, 3);
    this->models.push_back(this->model_focus);
    // reflection
    this->model_reflection = new Model(
        gl, {":/shaders/basic.vert", ":/shaders/lightAndBlue.frag"});
    primitiveLoader.loadSphereIntoModel(this->model_reflection, 5, 3);
    this->models.push_back(this->model_reflection);
    // mesh
    this->model_mesh = new Model_Calculatable(
        gl, {
            {":/shaders/basic.vert", ":/shaders/mesh_basic.frag"},
            {":/shaders/mesh_projectedTexture.vert", ":/shaders/lightAndTextureAndAlpha.frag"}
    });
    if (!fileLoader.loadFileIntoModel(this->model_mesh, ":/other/default.off"))
    {
      qCritical() << "Failed to load file final.off";
      qCritical() << fileLoader.getErrorMessage().c_str();
    }
    this->model_mesh->setCurrentShader(1);
//    primitiveLoader.loadBoxIntoModel(this->model_mesh, 100, 100, 100);
    this->models.push_back(this->model_mesh);
  // Setup decal texture
    this->target = new QOpenGLTexture(QImage(":/other/target.png"));
    this->target->setWrapMode(QOpenGLTexture::ClampToBorder);
    this->target->bind();
}

void Main3DScene::paintGL()
{
  gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  QVector3D focusPosition = this->model_focus->calcWorldPosition();
  QVector3D reflectedPosition = model_mesh->calcClosestSurfacePoint(focusPosition);
  this->model_reflection->setPosition(reflectedPosition);
  this->decalCameraTransform.setToIdentity();
  this->decalCameraTransform.lookAt(focusPosition, reflectedPosition, QVector3D(0,1,0));
  QVector3D decalNormal = (reflectedPosition - focusPosition).normalized();

  for (vector<Model*>::iterator i = this->models.begin(); i != this->models.end(); ++i)
  {
    QOpenGLShaderProgram* shader = (*i)->getShader();
    shader->bind();
    target->bind();
    shader->setUniformValue("projectionTransform", projectionTransform);
    shader->setUniformValue("cameraTransform", cameraTransform);
    if (*i == this->model_mesh)
    {
      ((Model_Calculatable*)*i)->draw(decalCameraTransform, decalNormal);
    } else {
      (*i)->draw();
    }
    shader->release();
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
Model_Calculatable* Main3DScene::getMesh() { return this->model_mesh; }
MainWindow* Main3DScene::getMainWindow() { return this->window; }

void Main3DScene::setIsEnabled_targetSphere(bool value)
{
  this->isEnabled_targetSphere = value;
  this->model_reflection->setIsVisible(value);
  repaint();
}

void Main3DScene::setMainWindow(MainWindow* value)
{
  this->window = value;
}
