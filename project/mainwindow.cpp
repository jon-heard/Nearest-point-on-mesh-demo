#include <QLabel>
#include <QCheckBox>
#include <QFileDialog>
#include <QComboBox>
#include <string>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scenerenderer.h"
#include "model_withcalculations.h"
#include "scene_nearestpointdemo.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  // Get widgets
  this->sceneRenderer = this->findChild<SceneRenderer*>("scene");
  this->toggleTargetSphere = this->findChild<QCheckBox*>("toggleTargetSphere");
  this->decalTypeSelector = this->findChild<QComboBox*>("decalTypeSelector");
  statusBarFilename = new QLabel();
  // Setup the status bar
  this->findChild<QStatusBar*>("statusBar")->addWidget(statusBarFilename);
  setModelFilename("default.off <i>(internal)</i>");
  // Setup the scene / scenerenderer
  this->sceneRenderer->setWindow(this);
  this->scene = new Scene_NearestPointDemo(this);
  this->sceneRenderer->setScene(scene);
}

MainWindow::~MainWindow()
{
  delete this->ui;
}

void MainWindow::setModelFilename(QString value)
{
  this->statusBarFilename->setText("<b>Model file:</b> " + value);
}

void MainWindow::setDecalTypeSelector(int index)
{
  // Disabling scene temporarily to prevent infinite loop
  Scene_NearestPointDemo* tmpScene = scene;
  sceneRenderer = NULL;
  // Set the decal type
  this->decalTypeSelector->setCurrentIndex(index);
  // Restore the scene
  scene = tmpScene;
}

void MainWindow::on_toggleTargetSphere_stateChanged(int arg1)
{
  this->scene->setIsTargetSphereEnabled(arg1 == Qt::Checked);
  this->sceneRenderer->repaint();
}

void MainWindow::on_decalTypeSelector_currentIndexChanged(int index)
{
  // Scene may be set to NULL to prevent infinite loop
  if (scene)
  {
    scene->setDecalType(index);
    sceneRenderer->repaint();
  }
}

void MainWindow::on_actionExit_triggered()
{
  QApplication::exit();
}

void MainWindow::on_actionLoad_Mesh_triggered()
{
  QString filename = QFileDialog::getOpenFileName(
      this, "Open a model file", "", "OFF Model file (*.off)");
  this->scene->initiateMeshLoading(filename.toStdString());
}
