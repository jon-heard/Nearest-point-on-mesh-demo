#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// MainWindow - The main window of this application.

namespace Ui {
  class MainWindow;
}

class SceneUi;
class Scene_NearestPointDemo;
class QLabel;
class QCheckBox;
class QComboBox;

class MainWindow : public QMainWindow
{
  Q_OBJECT
  public:
  // Construction
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
  // Functionality
    // Updates the status bar with the given filename
    void setModelFilename(QString value);
    // Sets the decal type dropdown to the given type index
    void setDecalTypeSelector(unsigned int index);
    // Key presses are forwarded to the SceneUi as it contains the input logic
    bool eventFilter(QObject *obj, QEvent *event);
  private slots:
  // Callbacks
    // Event: The checkbox for toggling the target sphere has been checked/unckecked
    void on_toggleTargetSphere_stateChanged(int arg1);
    // Event: The dropdown for selecting the decal type has been set to a new type
    void on_decalTypeSelector_currentIndexChanged(int index);
    // Event: The "Exit" menu item has been clicked
    void on_actionExit_triggered();
    // Event: The "Load Mesh" menu item has been clicked
    void on_actionLoad_Mesh_triggered();
    // Event: The "Load Default Mesh" menu item has been clicked
    void on_actionLoad_Default_Mesh_triggered();
    // Event: The "Load Low_Poly Sphere" menu item has been clicked
    void on_actionLoad_Low_Poly_Sphere_triggered();
    // Event: The "Load High_Poly Sphere" menu item has been clicked
    void on_actionLoad_High_Poly_Sphere_triggered();
  private:
    Ui::MainWindow *ui;
    SceneUi* sceneUi;
    Scene_NearestPointDemo* scene;
    QCheckBox* toggleTargetSphere;
    QLabel* statusBarFilename;
    QComboBox* decalTypeSelector;
};

#endif // MAINWINDOW_H
