#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/images/charm.png"));

    QSqlDatabase db=QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName("wire_oven");
    db.setHostName("192.168.1.7");
    db.setPort(5432);
    db.setUserName("user");
    db.setPassword("szsm");
    if (!db.open()){
        QMessageBox::critical(NULL,"Error",db.lastError().text(),QMessageBox::Ok);
        exit(0);
    }

    MainWindow w;
    w.show();

    return a.exec();
}
