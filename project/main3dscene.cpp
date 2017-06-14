#include "main3dscene.h"
#include "model.h"
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QDebug>
#include "model_fileloader_off.h"
#include "model_primitiveloader.h"

using namespace std;

Main3DScene::Main3DScene(QWidget *parent) :
  QOpenGLWidget(parent), zoom(-500)
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
  delete this->shader_focus;
  delete this->shader_mesh;
  delete this->shader_reflection;
}

void Main3DScene::refreshSceneTransform()
{
  this->sceneTransform.setToIdentity();
  this->sceneTransform.translate(0, 0, this->zoom);
  this->sceneTransform.rotate(rotation.x(), 1, 0, 0);
  this->sceneTransform.rotate(rotation.y(), 0, 1, 0);
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
  sceneTransform.setToIdentity();
  sceneTransform.translate(0, 0, zoom);
  // Setup shaders
    // focus
    this->shader_focus = new QOpenGLShaderProgram();
    this->shader_focus->addShaderFromSourceFile(
      QOpenGLShader::Vertex, ":/shaders/shader_basic.vert");
    this->shader_focus->addShaderFromSourceFile(
      QOpenGLShader::Fragment, ":/shaders/shader_focus.frag");
    this->shader_focus->link();
    // mesh
    this->shader_mesh = new QOpenGLShaderProgram();
    this->shader_mesh->addShaderFromSourceFile(
      QOpenGLShader::Vertex, ":/shaders/shader_basic.vert");
    this->shader_mesh->addShaderFromSourceFile(
      QOpenGLShader::Fragment, ":/shaders/shader_mesh.frag");
    this->shader_mesh->link();
    // reflection
    this->shader_reflection = new QOpenGLShaderProgram();
    this->shader_reflection->addShaderFromSourceFile(
      QOpenGLShader::Vertex, ":/shaders/shader_basic.vert");
    this->shader_reflection->addShaderFromSourceFile(
      QOpenGLShader::Fragment, ":/shaders/shader_reflection.frag");
    this->shader_reflection->link();
  // Setup models
  Model_FileLoader_OFF fileLoader;
  Model_PrimitiveLoader primitiveLoader;
    // focus
    this->model_focus = new Model(gl, shader_focus);
    primitiveLoader.loadSphereIntoModel(this->model_focus, 10, 2);
    this->models.push_back(this->model_focus);
    // reflection
    this->model_reflection = new Model(gl, shader_reflection);
    primitiveLoader.loadSphereIntoModel(this->model_reflection, 10, 2);
    this->models.push_back(this->model_reflection);
    // mesh
    this->model_mesh = new Model_Calculatable(gl, shader_mesh);
    if (!fileLoader.loadFileIntoModel(this->model_mesh, ":/other/default.off"))
    {
      qCritical() << "Failed to load file final.off";
      qCritical() << fileLoader.getErrorMessage().c_str();
    }
    this->models.push_back(this->model_mesh);
}

void Main3DScene::paintGL()
{
  gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  QVector3D focusPosition = this->model_focus->calcWorldPosition();
  QVector3D reflectedPosition = model_mesh->calcClosestSurfacePoint(focusPosition);
  this->model_reflection->setPosition(reflectedPosition);

  for (vector<Model*>::iterator i = this->models.begin(); i != this->models.end(); ++i)
  {
    QOpenGLShaderProgram* shader = (*i)->getShader();
    shader->bind();
    shader->setUniformValue("projectionTransform", projectionTransform);
    shader->setUniformValue("sceneTransform", sceneTransform);
    (*i)->draw();
    this->shader_focus->release();
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
    QVector3D focusRotation = this->model_focus->getRotation();
    this->previousRotation = QPoint(focusRotation.x(), focusRotation.y());
  }
}

void Main3DScene::mouseMoveEvent(QMouseEvent *event)
{
  if (event->buttons() & Qt::LeftButton)
  {
    rotation.setX(this->previousRotation.x() + (event->y() - this->previousMousePos.y()));
    rotation.setY(this->previousRotation.y() + (event->x() - this->previousMousePos.x()));
    refreshSceneTransform();
  } else if (event->buttons() & Qt::RightButton) {
    QVector3D focusRotation = this->model_focus->getRotation();
    focusRotation.setX(this->previousRotation.x() + (event->y() - this->previousMousePos.y()));
    focusRotation.setY(this->previousRotation.y() + (event->x() - this->previousMousePos.x()));
    this->model_focus->setRotation(focusRotation);
    repaint();
  }
}

void Main3DScene::wheelEvent(QWheelEvent* event)
{
  this->zoom += event->delta() / 2;
  if (this->zoom > -2) { this->zoom = -2; }
  refreshSceneTransform();
}

void Main3DScene::keyPressEvent(QKeyEvent* event)
{
  switch (event->key())
  {
    case Qt::Key_Up:
      rotation.setX(rotation.x() - 10);
      refreshSceneTransform();
      break;
    case Qt::Key_Right:
      rotation.setY(rotation.y() + 10);
      refreshSceneTransform();
      break;
    case Qt::Key_Down:
      rotation.setX(rotation.x() + 10);
      refreshSceneTransform();
      break;
    case Qt::Key_Left:
      rotation.setY(rotation.y() - 10);
      refreshSceneTransform();
      break;
    case Qt::Key_Z:
      this->zoom += 20;
      if (this->zoom > -2) { this->zoom = -2; }
      refreshSceneTransform();
      break;
    case Qt::Key_X:
      this->zoom -= 20;
      if (this->zoom > -2) { this->zoom = -2; }
      refreshSceneTransform();
      break;
    case Qt::Key_W:
    {
      QVector3D focusRotation = this->model_focus->getRotation();
      focusRotation += QVector3D(-10, 0, 0);
      this->model_focus->setRotation(focusRotation);
      repaint();
      break;
    }
    case Qt::Key_A:
    {
      QVector3D focusRotation = this->model_focus->getRotation();
      focusRotation += QVector3D(0, -10, 0);
      this->model_focus->setRotation(focusRotation);
      repaint();
      break;
    }
    case Qt::Key_S:
    {
      QVector3D focusRotation = this->model_focus->getRotation();
      focusRotation += QVector3D(10, 0, 0);
      this->model_focus->setRotation(focusRotation);
      repaint();
      break;
    }
    case Qt::Key_D:
    {
      QVector3D focusRotation = this->model_focus->getRotation();
      focusRotation += QVector3D(0, 10, 0);
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

//void Main3DScene::keyReleaseEvent(QKeyEvent* event) {}
