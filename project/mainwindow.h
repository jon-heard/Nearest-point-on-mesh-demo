#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class Main3DScene;
class QLabel;

class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  void refreshInstructions();
private:
  Ui::MainWindow *ui;
  QLabel* instructions;
  Main3DScene* scene;
  QString instructionsTemplate;
};

#endif // MAINWINDOW_H
