#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_pushButton_binaryPlain2Sipher_clicked();

    void on_pushButton_binarySipher2Plain_clicked();

    void on_pushButton_asciiPlain2Sipher_clicked();

    void on_pushButton_asciiSipher2Plain_clicked();

    void on_pushButton_doublePlain2Sipher_clicked();

    void on_pushButton_doubleSipher2Plain_clicked();

    void on_pushButton_crack_clicked();

    void on_pushButton_triplePlain2Sipher_clicked();

    void on_pushButton_tripleSipher2Plain_clicked();

    void on_pushButton_CBCPlain2Sipher_clicked();

    void on_pushButton_CBCSipher2Plain_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
