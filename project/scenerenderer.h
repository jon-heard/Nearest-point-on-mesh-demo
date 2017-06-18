#ifndef SCENERENDERER_H
#define SCENERENDERER_H

// Main3DScene - The primary I/O control.  Handles 3d rendering and user input.

#include <vector>
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QPoint>
#include <QVector3D>
#include <QQuaternion>

class Model;
class Model_WithCalculations;
class QOpenGLFunctions;
class QOpenGLShaderProgram;
class QOpenGLTexture;
class MainWindow;
class Scene_NearestPointDemo;

class SceneRenderer : public QOpenGLWidget
{
  public:
  // Construction
    SceneRenderer(QWidget* parent = 0);
    virtual ~SceneRenderer();
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
    MainWindow* getWindow();
    Scene_NearestPointDemo* getScene();
    void setWindow(MainWindow* value);
    void setScene(Scene_NearestPointDemo* value);
  private:
  // Fields
    MainWindow* window;
    QOpenGLFunctions* gl;
    Scene_NearestPointDemo* scene;
    QMatrix4x4 projectionTransform;
    QQuaternion previousRotation;
    QPoint previousMousePos;
};

#endif // SCENERENDERER_H
