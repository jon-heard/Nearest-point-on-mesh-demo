#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QCheckBox>
#include <string>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  this->scene = this->findChild<Main3DScene*>("scene");
  this->scene->setMainWindow(this);
  this->toggleTargetSphere = this->findChild<QCheckBox*>("toggleTargetSphere");
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_toggleTargetSphere_stateChanged(int arg1)
{
  this->scene->setIsEnabled_targetSphere(arg1 == Qt::Checked);
}

void MainWindow::on_decalTypeSelector_currentIndexChanged(int index)
{
  scene->getMesh()->setCurrentShader(index);
  scene->repaint();
}
