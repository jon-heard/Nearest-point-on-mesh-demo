#ifndef SCENEUI_H
#define SCENEUI_H

// SceneUi - Handles rendering and user input for the 3d scene.

#include <vector>
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <QQuaternion>

class Model;
class Model_WithCalculations;
class QOpenGLFunctions;
class QOpenGLShaderProgram;
class QOpenGLTexture;
class MainWindow;
class Scene_NearestPointDemo;

class SceneUi : public QOpenGLWidget
{
  public:
  // Construction
    SceneUi(QWidget* parent = 0);
    virtual ~SceneUi();
  // Event handlers - OpenGL
    // Event: OpenGL is initialized
    void initializeGL();
    // Event: The scene has changed
    void paintGL();
    // Event: The window has been resized
    void resizeGL(int w, int h);
  // Event handlers - input
    // Event: A mouse button has been pressed
    void mousePressEvent(QMouseEvent *event);
    // Event: The mouse has moved over the SceneUi
    void mouseMoveEvent(QMouseEvent *event);
    // Event: The mouse wheel has been spun
    void wheelEvent(QWheelEvent* event);
    // Event: A key has been pressed (used for key controls)
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
    QVector2D previousMousePos;
};

#endif // SCENEUI_H
