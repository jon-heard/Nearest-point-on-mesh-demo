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
  this->toggleTargetDecal1 = this->findChild<QCheckBox*>("toggleTargetDecal1");
  refreshInstructions();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::refreshInstructions()
{
  if (this->scene->getIsEnabled_targetSphere() != this->toggleTargetSphere->isChecked())
  {
    this->toggleTargetSphere->setChecked(this->scene->getIsEnabled_targetSphere());
  }
  if (this->scene->getIsEnabled_targetDecal1() != this->toggleTargetDecal1->isChecked())
  {
    this->toggleTargetDecal1->setChecked(this->scene->getIsEnabled_targetDecal1());
  }
}

void MainWindow::on_toggleTargetSphere_stateChanged(int arg1)
{
  if (this->scene->getIsEnabled_targetSphere() != (arg1 == Qt::Checked)) {
    this->scene->setIsEnabled_targetSphere(arg1 == Qt::Checked);
  }
}

void MainWindow::on_toggleTargetDecal1_stateChanged(int arg1)
{
  if (this->scene->getIsEnabled_targetDecal1() != (arg1 == Qt::Checked)) {
    this->scene->setIsEnabled_targetDecal1(arg1 == Qt::Checked);
  }
}
