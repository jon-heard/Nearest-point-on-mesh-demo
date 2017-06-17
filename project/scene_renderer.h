#ifndef SCENE_RENDERER_H
#define SCENE_RENDERER_H

// Main3DScene - The driver class of this application

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

class Scene_Renderer : public QOpenGLWidget
{
  public:
  // Construction
    Scene_Renderer(QWidget* parent = 0);
    virtual ~Scene_Renderer();
  // functionality
    void initiateLoadMesh(std::string filename);
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
    Scene_NearestPointDemo* getScene();
    bool getIsEnabled_targetSphere();
    MainWindow* getMainWindow();
    void setScene(Scene_NearestPointDemo* value);
    void setIsEnabled_targetSphere(bool value);
    void setMainWindow(MainWindow* value);
  private:
  // Functionality
    //bool loadMesh(std::string filename);
  // Fields
    Scene_NearestPointDemo* scene;
    QMatrix4x4 projectionTransform;
    QQuaternion previousRotation;
    QPoint previousMousePos;
    QOpenGLFunctions* gl;
    bool isEnabled_targetSphere;
    MainWindow* window;
    std::string newMeshFilename;
};

#endif // SCENE_RENDERER_H
