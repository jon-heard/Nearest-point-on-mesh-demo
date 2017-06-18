#include <QLabel>
#include <QCheckBox>
#include <QFileDialog>
#include <QComboBox>
#include <QString>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sceneui.h"
#include "model_withcalculations.h"
#include "scene_nearestpointdemo.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  setFocusPolicy(Qt::StrongFocus);
  // Get widgets
  this->sceneUi = this->findChild<SceneUi*>("scene");
  this->toggleTargetSphere = this->findChild<QCheckBox*>("toggleTargetSphere");
  this->decalTypeSelector = this->findChild<QComboBox*>("decalTypeSelector");
  statusBarFilename = new QLabel();
  // Setup widgets
  this->findChild<QStatusBar*>("statusBar")->addWidget(statusBarFilename);
  this->setModelFilename("default.off <i>(internal)</i>");
  // Setup the sceneUi & scene
  this->sceneUi->setWindow(this);
  this->scene = new Scene_NearestPointDemo(this);
  this->sceneUi->setScene(scene);
}

MainWindow::~MainWindow()
{
  delete this->ui;
}

void MainWindow::setModelFilename(QString value)
{
  this->statusBarFilename->setText("<b>Model file:</b> " + value);
}

void MainWindow::setDecalTypeSelector(unsigned int index)
{
  // Disabling scene (prevent infinite loop: combobox updates sceneUi updates combobox)
  Scene_NearestPointDemo* tmpScene = this->scene;
  this->scene = NULL;
  // Set the decal type
  this->decalTypeSelector->setCurrentIndex(index);
  // Reenable the scene
  this->scene = tmpScene;
}

// Returns keyboard focus to sceneUi for keyboard controls
void MainWindow::keyPressEvent(QKeyEvent* event)
{
  this->sceneUi->setFocus();
  this->sceneUi->keyPressEvent(event);
}

void MainWindow::on_toggleTargetSphere_stateChanged(int arg1)
{
  this->scene->setIsTargetSphereEnabled(arg1 == Qt::Checked);
  this->sceneUi->repaint();
}

void MainWindow::on_decalTypeSelector_currentIndexChanged(int index)
{
  // Scene may be set to NULL to prevent infinite loop (see 'setDecalTypeSelector' for details)
  if (this->scene)
  {
    this->scene->setDecalType(index);
    this->sceneUi->repaint();
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
  if (filename != "")
  {
    this->scene->initiateMeshLoading(filename);
  }
}
