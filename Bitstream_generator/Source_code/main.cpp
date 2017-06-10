/***********************************************************************
 *           Author: Wenlong Wang & Xin Guan
 *      Create date: 28/05/2017
 * Modifiacion date: 28/05/2017
 *      Description: Main function, entrance of entire application
 ***********************************************************************/
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    MainWindow w(argv[1], argv[2], argv[3]);

    MainWindow w;
//    MainWindow w("G:/lattice diamond/diamond/3.9_x64",
//                 "C:/Users/679408/Desktop/lattice",
//                 "C:/Users/679408/Desktop/sdp vhdlmerge/vhdl_code/iv.vhd");
    w.show();
    return a.exec();
}
