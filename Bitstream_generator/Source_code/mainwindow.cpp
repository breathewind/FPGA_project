/***********************************************************************
 *           Author: Wenlong Wang & Xin Guan
 *      Create date: 28/05/2017
 * Modifiacion date: 05/06/2017
 *      Description: Class MainWindow, all the UI will be displayed here
 ***********************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"

/***********************************************************************
 *             Name: MainWindow
 *      Function ID: 00-1
 *      Create date: 28/05/2017
 * Modifiacion date: 28/05/2017
 *      Description: Default construction function
 ***********************************************************************/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    init();

    lattice_dir_path = QDir::homePath();
    workspace_dir_path = QDir::homePath();
    vhdl_dir_path = QDir::homePath();

    ui->lineEdit_lattice->setText(lattice_dir_path);
    ui->lineEdit_workspace->setText(workspace_dir_path);
    ui->lineEdit_vhdl->setText("");
}

/***********************************************************************
 *             Name: MainWindow
 *      Function ID: 00-2
 *      Create date: 28/05/2017
 * Modifiacion date: 28/05/2017
 *      Description: Construction function with already-initalized
 *                   arguments
 ***********************************************************************/
MainWindow::MainWindow(QString lattice_file_path, QString workspace_file_path, QString vhdl_file):
    QMainWindow(0),
    ui(new Ui::MainWindow)
{
    init();

//    qDebug() << lattice_file_path;
    lattice_dir_path = lattice_file_path;
//    qDebug() << lattice_dir_path;

//    qDebug() << vhdl_file_path;
    workspace_dir_path = workspace_file_path;
//    qDebug() << vhdl_dir_path;

    vhdl_dir_path = vhdl_file.left(vhdl_file.lastIndexOf("/"));

    ui->lineEdit_lattice->setText(lattice_file_path);
    ui->lineEdit_workspace->setText(workspace_file_path);
    ui->lineEdit_vhdl->setText("");
}

/***********************************************************************
 *             Name: ~MainWindow
 *      Function ID: 01
 *      Create date: 28/05/2017
 * Modifiacion date: 28/05/2017
 *      Description: Destruction function
 ***********************************************************************/
MainWindow::~MainWindow()
{
    delete ui;
}

/***********************************************************************
 *             Name: init
 *      Function ID: 02
 *      Create date: 28/05/2017
 * Modifiacion date: 28/05/2017
 *      Description: Initialize parameters
 ***********************************************************************/
void MainWindow::init()
{
    ui->setupUi(this);

    ui->pushButton_lattice->setIcon(QIcon(":/img/img/folder.png"));
    ui->pushButton_workspace->setIcon(QIcon(":/img/img/folder.png"));
    ui->pushButton_vhdl->setIcon(QIcon(":/img/img/folder.png"));
}

/***********************************************************************
 *             Name: on_pushButton_lattice_clicked
 *      Function ID: 03
 *      Create date: 28/05/2017
 * Modifiacion date: 28/05/2017
 *      Description: Open file dialog for choosing the location of
 *                   Diamond Lattice
 ***********************************************************************/
void MainWindow::on_pushButton_lattice_clicked()
{
    QString filepath = QFileDialog::getExistingDirectory(this, "Open file", lattice_dir_path);
    if(filepath.size()){
        ui->lineEdit_lattice->setText(filepath);
        lattice_dir_path = filepath;
        qDebug() << lattice_dir_path;
    }
}

/***********************************************************************
 *             Name: on_pushButton_workspace_clicked
 *      Function ID: 04
 *      Create date: 28/05/2017
 * Modifiacion date: 28/05/2017
 *      Description: Open file dialog for choosing the location of
 *                   top-level entity vhdl code
 ***********************************************************************/
void MainWindow::on_pushButton_workspace_clicked()
{
    QString filepath = QFileDialog::getExistingDirectory(this, "Open file", workspace_dir_path);
    if(filepath.size()){
        ui->lineEdit_workspace->setText(filepath);
        workspace_dir_path = filepath;
        qDebug() << workspace_dir_path;
    }
}

/***********************************************************************
 *             Name: on_pushButton_accept_clicked
 *      Function ID: 05
 *      Create date: 28/05/2017
 * Modifiacion date: 28/05/2017
 *      Description: Accept all previous operations and begin to
 *                   generate bit-stream
 ***********************************************************************/
void MainWindow::on_pushButton_accept_clicked()
{
    QProcess *process = new QProcess(this);
    QString output;

    QString file = "\"" + lattice_dir_path + "/bin/nt64/pnmainc.exe\"";
    process->setProgram(file);

    QByteArray ba = workspace_dir_path.toLatin1();
    const char *workspace_dir_char = ba.data();

//    qDebug()<< vhdl_dir_char;
    process->start();
    process->write("cd ");
    process->write(workspace_dir_char);
    process->write("\n\r");
    process->write("prj_project new -name \"project1\" -impl \"impl1\" -dev LCMXO2-7000HE-5TG144C -synthesis \"lse\"\n\r");
    for(int i=0; i< vhdl_files.size(); i++){
        ba = vhdl_files.at(i).toLatin1();
        const char *vhdl_dir_char = ba.data();

        process->write("prj_src add \"");
        process->write(vhdl_dir_char);
        process->write("\"\n\r");

        qDebug() << "prj_src add \"" << vhdl_dir_char << "\"";
    }
    process->write("prj_run Export -impl impl1 -task Bitgen\n\r");
    process->write("prj_project save\n\r");
    process->write("prj_project close\n\r");
    process->write("exit\n\r");
    while(!process->waitForFinished(100)){ // sets current thread to sleep and waits for pingProcess end
//        output = process->readChannel();
//        ui->textBrowser->append(output);
    }
    output = process->readAllStandardOutput();
    ui->textBrowser->append(output);
    ui->textBrowser->update();
    process->close();

    process = new QProcess(this);
    file = "\"" + lattice_dir_path + "/bin/nt64/ddtcmd\"";
    process->setProgram(file);
    process->setArguments(QStringList() << "-oft"
                                        << "-sspi"
                                        << "-if"
                                        << workspace_dir_path + "/impl1/project1_impl1.bit"
                                        << "-op"
                                        << "Slave SPI Fast Configuration"
                                        << "-hex"
                                        << "-ofa"
                                        << workspace_dir_path + "/impl1/project1_impl1_algo.sea"
                                        << "-ofd"
                                        << workspace_dir_path + "/impl1/project1_impl1_data.sed"
                              );
    process->start();
    while(!process->waitForFinished(100)){ // sets current thread to sleep and waits for pingProcess end
//        output = process->readChannel();
//        ui->textBrowser->append(output);
    }
    output = process->readAllStandardOutput();
    ui->textBrowser->append(output);
    process->close();

    process = new QProcess(this);
    file = "\"file_trans.exe\"";
    process->setProgram(file);
    process->setArguments(QStringList()<<workspace_dir_path);
    process->start();
    while(!process->waitForFinished(100)){ // sets current thread to sleep and waits for pingProcess end
    }
    output = process->readAllStandardOutput();
    ui->textBrowser->append(output);
    process->close();
}

/***********************************************************************
 *             Name: on_pushButton_cancel_clicked
 *      Function ID: 06
 *      Create date: 28/05/2017
 * Modifiacion date: 28/05/2017
 *      Description: Cancel all previous operations and quit application
 ***********************************************************************/
void MainWindow::on_pushButton_cancel_clicked()
{
    qDebug() << lattice_dir_path;
    QApplication::quit();
    QApplication::topLevelWidgets();
}

/***********************************************************************
 *             Name: on_pushButton_vhdl_clicked
 *      Function ID: 07
 *      Create date: 28/05/2017
 * Modifiacion date: 05/06/2017
 *      Description: Open file dialog for choosing the location of
 *                   top-level entity vhdl code
 ***********************************************************************/
void MainWindow::on_pushButton_vhdl_clicked()
{
    vhdl_files = QFileDialog::getOpenFileNames(this, "Select one or more files", vhdl_dir_path, "VHDL (*.vhd *.vhdl)");
    if(vhdl_files.size()){
        vhdl_dir_path = vhdl_files.at(0).left(vhdl_files.at(0).lastIndexOf("/"));
        ui->lineEdit_vhdl->setText(vhdl_dir_path);
        for(int i=0; i<vhdl_files.size();i++)
            qDebug() << "++" << vhdl_files.at(i);
    }
}
