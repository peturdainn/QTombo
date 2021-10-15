#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include <QtCore>
//#include <QWidget>
#include <QMainWindow>
#include <QItemSelection>
//#include <QtWidgets/QWidget>
#include <QFileSystemModel>
#include "./crypt/CryptManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    CryptManager cMgr;
    QString current_root;
    QFileSystemModel filesystem;
    unsigned int dirty;
    unsigned int encrypted;
    unsigned int have_key;
    QModelIndex current_index;
    QModelIndex context_index;
    void save(const QModelIndex & index);
    void encrypt_file(const QModelIndex & index);
    void decrypt_file(const QModelIndex & index);
    QAction *contextnewfileAct;
    QAction *contextnewfolderAct;
    QAction *contextdeleteAct;
    QAction *contextencryptAct;
    QAction *contextdecryptAct;
    QAction *contextrenameAct;

private slots:
    void on_actionAbout_triggered();
    void on_treeView_customContextMenuRequested(const QPoint & iPoint);
    void on_textEdit_textChanged();
    void selectionchanged(const QItemSelection & selected, const QItemSelection & deselected);
    void save();
    void setroot();
    void newfile();
    void newfile(const QModelIndex & index);
    void newfolder();
    void newfolder(const QModelIndex & index);
    void deletefile();
    void deletefile(const QModelIndex & index);
    void rename();
    void rename(const QModelIndex & index);
    void context_newfile();
    void context_newfolder();
    void context_delete();
    void context_rename();
};

#endif // MAINWINDOW_H
