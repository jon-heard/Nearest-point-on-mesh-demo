#include <QLabel>
#include <QCheckBox>
#include <QFileDialog>
#include <QComboBox>
#include <string>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scene_renderer.h"
#include "model_withcalculations.h"
#include "scene_nearestpointdemo.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  this->sceneRenderer = this->findChild<Scene_Renderer*>("scene");
  this->sceneRenderer->setMainWindow(this);
  this->scene = new Scene_NearestPointDemo();
  this->sceneRenderer->setScene(scene);
  this->toggleTargetSphere = this->findChild<QCheckBox*>("toggleTargetSphere");
  this->decalTypeSelector = this->findChild<QComboBox*>("decalTypeSelector");
  statusBarFilename = new QLabel();
  this->findChild<QStatusBar*>("statusBar")->addWidget(statusBarFilename);
  setModelFilename("default.off <i>(internal)</i>");
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
  Scene_Renderer* tmpScene = sceneRenderer;
  sceneRenderer = NULL;
  this->decalTypeSelector->setCurrentIndex(index);
  sceneRenderer = tmpScene;
}

void MainWindow::on_toggleTargetSphere_stateChanged(int arg1)
{
  this->sceneRenderer->setIsEnabled_targetSphere(arg1 == Qt::Checked);
}

void MainWindow::on_decalTypeSelector_currentIndexChanged(int index)
{
  if (sceneRenderer)
  {
    sceneRenderer->getScene()->getModel_mesh()->setCurrentShader(index);
    sceneRenderer->repaint();
  }
}

void MainWindow::on_actionExit_triggered()
{
  QApplication::exit();
}

void MainWindow::on_actionLoad_Mesh_triggered()
{
  QString filename = QFileDialog::getOpenFileName(this, "Open a model file", "", "OFF Model file (*.off)");
  this->sceneRenderer->initiateLoadMesh(filename.toStdString());
}
