#ifndef MAIN3DSCENE_H
#define MAIN3DSCENE_H

// Main3DScene - The driver class of this application

#include <vector>
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QPoint>
#include <QVector3D>
#include <QQuaternion>
#include "Model_Calculatable.h"

class Model;
class QOpenGLFunctions;
class QOpenGLShaderProgram;
class QOpenGLTexture;
class MainWindow;

class Main3DScene : public QOpenGLWidget
{
  public:
  // Construction
    Main3DScene(QWidget* parent = 0);
    virtual ~Main3DScene();
  // functionality
    void refreshCameraTransform();
  // Event handlers - OpenGL
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
  // Event handlers - input
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent* event);
    void keyPressEvent(QKeyEvent* event);
  // Accessors
    bool getIsEnabled_targetSphere();
    Model_Calculatable* getMesh();
    MainWindow* getMainWindow();
    void setIsEnabled_targetSphere(bool value);
    void setMainWindow(MainWindow* value);
  private:
    QMatrix4x4 projectionTransform;
    QMatrix4x4 cameraTransform;
    float zoom;
    QQuaternion rotation;
    QQuaternion previousRotation;
    QPoint previousMousePos;
    QOpenGLFunctions* gl;
    std::vector<Model*> models;
    Model_Calculatable* model_mesh;
    Model* model_reflection;
    Model* model_focus;
    QOpenGLTexture* targetTexture;

    QMatrix4x4 decalCameraTransform;

    bool isEnabled_targetSphere;

    MainWindow* window;
};

#endif // MAIN3DSCENE_H
