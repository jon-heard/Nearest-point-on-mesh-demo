#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
  class MainWindow;
}

class Main3DScene;
class QLabel;
class QCheckBox;
class QComboBox;

class MainWindow : public QMainWindow
{
  Q_OBJECT
  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setModelFilename(QString value);
    void setDecalTypeSelector(int index);
  private slots:
    void on_toggleTargetSphere_stateChanged(int arg1);
    void on_decalTypeSelector_currentIndexChanged(int index);
    void on_actionExit_triggered();

    void on_actionLoad_Mesh_triggered();

  private:
    Ui::MainWindow *ui;
    Main3DScene* scene;
    QCheckBox* toggleTargetSphere;
    QLabel* statusBarFilename;
    QComboBox* decalTypeSelector;
};

#endif // MAINWINDOW_H
