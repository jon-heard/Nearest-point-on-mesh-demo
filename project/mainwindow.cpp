#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <string>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  this->scene = this->findChild<Main3DScene*>("scene");
  this->scene->setMainWindow(this);
  this->instructions = this->findChild<QLabel*>("instructions");
  this->instructionsTemplate = instructions->text();
  refreshInstructions();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::refreshInstructions()
{
  char instructionsText[1024];
  string targetSphere = this->scene->getIsEnabled_targetSphere() ? "ON" : "OFF";
  string targetDecal1 = this->scene->getIsEnabled_targetDecal1() ? "ON" : "OFF";
  sprintf(instructionsText, this->instructionsTemplate.toStdString().c_str(), targetSphere.c_str(), targetDecal1.c_str());
  instructions->setText(instructionsText);//instructionsText);
}
