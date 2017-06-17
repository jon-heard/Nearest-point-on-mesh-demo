#include <QLabel>
#include <QCheckBox>
#include <QFileDialog>
#include <QComboBox>
#include <string>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "main3dscene.h"
#include "model_withcalculations.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  this->scene = this->findChild<Main3DScene*>("scene");
  this->scene->setMainWindow(this);
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
  Main3DScene* tmpScene = scene;
  scene = NULL;
  this->decalTypeSelector->setCurrentIndex(index);
  scene = tmpScene;
}

void MainWindow::on_toggleTargetSphere_stateChanged(int arg1)
{
  this->scene->setIsEnabled_targetSphere(arg1 == Qt::Checked);
}

void MainWindow::on_decalTypeSelector_currentIndexChanged(int index)
{
  if (scene)
  {
    scene->getMesh()->setCurrentShader(index);
    scene->repaint();
  }
}

void MainWindow::on_actionExit_triggered()
{
  QApplication::exit();
}

void MainWindow::on_actionLoad_Mesh_triggered()
{
  QString filename = QFileDialog::getOpenFileName(this, "Open a model file", "", "OFF Model file (*.off)");
  this->scene->initiateLoadMesh(filename.toStdString());
}
