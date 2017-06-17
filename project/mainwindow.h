#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
  class MainWindow;
}

class Main3DScene;
class QLabel;
class QCheckBox;

class MainWindow : public QMainWindow
{
  Q_OBJECT
  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void refreshInstructions();
  private slots:
    void on_toggleTargetSphere_stateChanged(int arg1);
    void on_decalTypeSelector_currentIndexChanged(int index);
  private:
    Ui::MainWindow *ui;
    Main3DScene* scene;
    QCheckBox* toggleTargetSphere;
};

#endif // MAINWINDOW_H
