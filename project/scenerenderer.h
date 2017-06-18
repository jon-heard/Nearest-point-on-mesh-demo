#ifndef SCENERENDERER_H
#define SCENERENDERER_H

// SceneRenderer - Handles rendering of the 3d scene and also user input.

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

class SceneRenderer : public QOpenGLWidget
{
  public:
  // Construction
    SceneRenderer(QWidget* parent = 0);
    virtual ~SceneRenderer();
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
    // Event: The mouse has moved over the SceneRenderer
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

#endif // SCENERENDERER_H
