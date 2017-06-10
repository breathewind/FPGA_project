/***********************************************************************
 *           Author: Wenlong Wang & Xin Guan
 *      Create date: 28/05/2017
 * Modifiacion date: 05/06/2017
 *      Description: Class MainWindow, all the UI will be displayed here
 ***********************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QFileDialog>
#include <QProcess>

#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //Function 00-1: Default construction function
    explicit MainWindow(QWidget *parent = 0);
    //Function 00-2: Construction function with already-initalized arguments
    explicit MainWindow(QString lattice_file_path, QString workspace_file_path, QString vhdl_file);
    //Function 01: Destruction function
    ~MainWindow();

private slots:
    //Slot 03: Open file dialog for choosing the location of Diamond Lattice
    void on_pushButton_lattice_clicked();
    //Slot 04: Open file dialog for choosing the location of top-level entity vhdl code
    void on_pushButton_workspace_clicked();
    //Slot 05: Accept all previous operations and begin to generate bit-stream
    void on_pushButton_accept_clicked();
    //Slot 06: Cancel all previous operations and quit application
    void on_pushButton_cancel_clicked();

    void on_pushButton_vhdl_clicked();

private:
    Ui::MainWindow *ui;
    QString lattice_dir_path; // Diamond Lattice directory path
    QString workspace_dir_path;    // The path of directory includes top-level entity vhdl code
    QString vhdl_dir_path;
    QStringList vhdl_files;

    //Function 02: Initialize parameters
    void init();
};

#endif // MAINWINDOW_H
