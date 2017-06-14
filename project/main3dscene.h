#ifndef MAIN3DSCENE_H
#define MAIN3DSCENE_H

#include <vector>
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QPoint>
#include <QVector3D>
#include "Model_Calculatable.h"

class Model;
class QOpenGLFunctions;
class QOpenGLShaderProgram;

class Main3DScene : public QOpenGLWidget
{
  public:
    Main3DScene(QWidget* parent = 0);
    virtual ~Main3DScene();

    void refreshSceneTransform();

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent* event);
    void keyPressEvent(QKeyEvent* event);
    //void keyReleaseEvent(QKeyEvent* event);
  private:
    QMatrix4x4 projectionTransform;
    QMatrix4x4 sceneTransform;
    float zoom;
    QPoint rotation;
    QPoint previousRotation;
    QPoint previousMousePos;
    QOpenGLFunctions* gl;
    std::vector<Model*> models;
    QOpenGLShaderProgram* shader_focus;
    QOpenGLShaderProgram* shader_reflection;
    QOpenGLShaderProgram* shader_mesh;
    Model_Calculatable* model_mesh;
    Model* model_focus;
    Model* model_reflection;
};

#endif // MAIN3DSCENE_H
