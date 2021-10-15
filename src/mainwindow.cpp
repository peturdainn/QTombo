#include <QtCore>
#include <QWidget>
#include <qfile.h>
#include <stdio.h>
#include "ui_mainwindow.h"
#include "textinput.h"
#include "./crypt/CryptManager.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    QSettings settings("Qtombo", "settings");
    current_root = settings.value("General/root").toString();

    QStringList name_filters;
    name_filters.append("*.txt");
    name_filters.append("*.chi");

    ui->setupUi(this);
    ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeView->setModel(&filesystem);
    filesystem.setRootPath(current_root);
    filesystem.setNameFilters(name_filters);
    ui->treeView->setRootIndex(filesystem.index(current_root));
    ui->treeView->show();
    ui->treeView->repaint();
    dirty = 0;
    encrypted = 0;
    have_key = 0;

    ui->actionSave->setDisabled(true);

    // attach filetree to treeview
    connect( ui->treeView->selectionModel(), SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection & )), this, SLOT( selectionchanged( const QItemSelection &, const QItemSelection & )));

    contextnewfileAct = new QAction(tr("New Note here"), this);
    connect(contextnewfileAct, SIGNAL(triggered()), this, SLOT(context_newfile()));
    contextnewfolderAct = new QAction(tr("New Folder here"), this);
    connect(contextnewfolderAct, SIGNAL(triggered()), this, SLOT(context_newfolder()));
    contextdeleteAct = new QAction(tr("Delete"), this);
    connect(contextdeleteAct, SIGNAL(triggered()), this, SLOT(context_delete()));
    contextrenameAct = new QAction(tr("Rename"), this);
    connect(contextrenameAct, SIGNAL(triggered()), this, SLOT(context_rename()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectionchanged(const QItemSelection & selected, const QItemSelection & deselected)
{
    (void)deselected;

    QModelIndexList indexlist = selected.indexes();
    QModelIndex selindex(indexlist.first()); // single selection model

    // check if current file in editor has changed
    if((dirty) && (selindex != current_index))
    {
        // if new selection, ask to save....
        QMessageBox msgbox_q_save;
        msgbox_q_save.setIcon(QMessageBox::Warning);
        msgbox_q_save.setText("Note changed... Save?");
        msgbox_q_save.setStandardButtons(QMessageBox::Save | QMessageBox::Ignore | QMessageBox::Cancel);
        int user_choice = msgbox_q_save.exec();
        switch(user_choice)
        {
            case QMessageBox::Save:
            {
                // save previous note
                save(current_index);
                break;
            }
            case QMessageBox::Ignore:
            {
                // do nothing (ignore changes)
                break;
            }
            case QMessageBox::Cancel:
            {
                // re-select the old note
                ui->treeView->setCurrentIndex(current_index);
                return;
            }
        }
    }

    // we have a new selection
    current_index = ui->treeView->currentIndex();

    // determine if the clicked item is a file or directory
    QString path, ext;
    QFile file;
    ext = filesystem.type(current_index);
    if(filesystem.isDir(current_index))
    {
        ui->actionDelete->setDisabled(true);
        ui->textEdit->setText("");
        dirty = 0; // since we updated it from file....
        return; // we don't edit directories...
    }
    else
    {
        ui->actionDelete->setDisabled(false);
        path = filesystem.filePath(current_index);
        file.setFileName(path);
        file.open(QIODevice::ReadWrite);
    }

    if(ext == "txt File")
    {
        // plain text file
        QTextStream stream( &file );
        ui->textEdit->setText(stream.readAll());
        dirty = 0;
        encrypted = 0;
    }
    if(ext == "chi File")
    {
        // encrypted... try current key, if that fails ask for the key
        QString key;
        unsigned char* decoded;
        unsigned int size;
        unsigned char empty = 0;
        decoded = cMgr.LoadAndDecrypt(&size, (char*)(path.toLatin1().constBegin()));
        if(!decoded)
        {
            // decode failed... the current key (if any) is probably wrong
            textinput tid;
            tid.set_label("Enter encryption key:");
            tid.set_mode_key();
            tid.show();
            tid.exec();
            key = tid.get_text();
            cMgr.Init(key.toLatin1());
            key.clear(); // erase password from memory again (ASAP).
            decoded = cMgr.LoadAndDecrypt(&size, (char*)(path.toLatin1().constBegin()));
            if(!decoded)
                decoded = &empty; // give an errorbox too...?
        }
        have_key = 1;
        ui->textEdit->setText((const char*)decoded);
        dirty = 0;
        encrypted = 1;
    }
}

void MainWindow::on_textEdit_textChanged()
{
    dirty = 1;
    ui->actionSave->setDisabled(false);
}

void MainWindow::on_treeView_customContextMenuRequested(const QPoint & iPoint)
{
    context_index = ui->treeView->indexAt(iPoint);
    if(!context_index.isValid())
        return;
    QString path = filesystem.filePath(context_index);
    QMenu menu(ui->treeView);
    //menu.set

    // right-click on dir: (1) add Note in folder (2) add folder in folder
    menu.addAction(contextnewfileAct);
    menu.addAction(contextnewfolderAct);
    menu.addAction(contextrenameAct);
    menu.addAction(contextdeleteAct);

    menu.exec(ui->treeView->mapToGlobal(iPoint));
}

void MainWindow::context_newfile()
{
    newfile(context_index);
}

void MainWindow::context_newfolder()
{
    newfolder(context_index);
}

void MainWindow::context_delete()
{
    deletefile(context_index);
}

void MainWindow::context_rename()
{
    rename(context_index);
}

void MainWindow::save()
{
    save(current_index);
}

void MainWindow::save(const QModelIndex & index)
{
    QString path, ext;
    ext = filesystem.type(index);
    path = filesystem.filePath(index);
    QString str = ui->textEdit->toPlainText();
    if(ext == "txt File")
    {
        QFile file;
        file.setFileName(path);
        file.open(QIODevice::ReadWrite | QIODevice::Truncate);
        file.write( str.toUtf8() );
        dirty = 0;
        ui->actionSave->setDisabled(true);
    }
    if(ext == "chi File")
    {
        if(!have_key)
        {
            QString key;
            textinput tid;
            tid.set_label("Enter encryption key:");
            tid.set_mode_key();
            tid.show();
            tid.exec();
            key = tid.get_text();
            cMgr.Init(key.toLatin1());
            key.clear(); // erase password from memory again (ASAP).
            have_key = 1;
        }
        bool ret = cMgr.EncryptAndStore((const unsigned char*)(str.toUtf8().constBegin()),str.toUtf8().length(),(char*)(path.toLatin1().constBegin()));
        if(ret)
        {
            dirty = 0;
            ui->actionSave->setDisabled(true);
        }
    }
}

void MainWindow::setroot()
{
    // 1. open a dialog to browse for a location
    QString new_root;
    QFileDialog fd(this);
    fd.setFileMode( QFileDialog::DirectoryOnly);
    new_root = fd.getExistingDirectory(this, tr("New Virtual Directory"), current_root);
    if(new_root.isNull() == true )
    {
        return;
    }

    // 2. set current location and refresh
    current_root = new_root;
    filesystem.setRootPath(current_root);
    this->ui->treeView->setRootIndex(filesystem.index(current_root));
    this->ui->treeView->show();
    this->ui->treeView->repaint();

    // 3. save to disk
    QSettings settings("Qtombo", "settings");
    settings.setValue("General/root", current_root);
    settings.sync();
}

void MainWindow::newfile()
{
    return newfile(ui->treeView->currentIndex());
}

void MainWindow::newfolder()
{
    return newfolder(ui->treeView->currentIndex());
}

void MainWindow::newfile(const QModelIndex & index)
{
    // 1. get current position in the tree - we create the new file here
    QString pathname, filename, completename;
    QFile file;
    if(filesystem.isDir(index))
    {
        // user selected a dir
        pathname = QFileInfo(filesystem.filePath(index)).absoluteFilePath();
    }
    else
    {
        // user selected a file
        pathname = QFileInfo(filesystem.filePath(index)).absolutePath();
    }

    // 1. open a dialog to browse for a location
    textinput tid;
    tid.set_label("New file name:");
    tid.show();
    tid.exec();
    filename = tid.get_text();
    if(tid.is_cb_encrypted())
    {
        // always ask for encryption key
        QString key;
        textinput tid;
        tid.set_label("Enter encryption key:");
        tid.set_mode_key();
        tid.show();
        tid.exec();
        key = tid.get_text();
        cMgr.Init(key.toLatin1());
        key.clear(); // erase password from memory again (ASAP).
        have_key = 1;
        filename += ".chi";
    }
    else
    {
        filename += ".txt";
    }

    completename = pathname + "/" + filename;

    file.setFileName(completename);
    if(file.exists())
        return; // file already exists - DO NOT overwrite!

    QString str("");

    if(tid.is_cb_encrypted())
    {
        bool ret = cMgr.EncryptAndStore((const unsigned char*)(str.toUtf8().constBegin()),0,(char*)(completename.toLatin1().constBegin()));
        (void)ret;
        // to do: handle failure
    }
    else
    {
        file.open(QIODevice::ReadWrite | QIODevice::Truncate);
        file.write( str.toUtf8() );
        // to do: handle failure
   }

    // select the new file in the tree
    ui->treeView->setCurrentIndex(filesystem.index(completename));

    // update edit box *after* selection!
    ui->textEdit->setText("");
    dirty = 0; // since we updated it from file....

    save();
}

void MainWindow::newfolder(const QModelIndex & index)
{
    // 1. get current position in the tree - we create the new file here
    QString foldername;

    // 1. open a dialog to browse for a location
    textinput tid;
    tid.set_label("New folder name:");
    tid.allow_encrypt_sel(false);
    tid.show();
    tid.exec();
    foldername = tid.get_text();

    if(filesystem.isDir(index))
    {
        // user selected a dir
        filesystem.mkdir(index, foldername);
    }
    else
    {
        // user selected a file
        filesystem.mkdir(filesystem.parent(index), foldername);
    }
}

void MainWindow::deletefile()
{
    deletefile(ui->treeView->currentIndex());
}

void MainWindow::deletefile(const QModelIndex & index)
{
    QString completename;
    QDir dir;

    // user selected a file
    completename = QFileInfo(filesystem.filePath(index)).absoluteFilePath();
    QMessageBox msgbox_q_del;
    msgbox_q_del.setIcon(QMessageBox::Warning);
    if(filesystem.isDir(index))
        msgbox_q_del.setText("Delete Folder "+completename+" and all its subfolders and files?");
    else
        msgbox_q_del.setText("Delete "+completename+"?");
    msgbox_q_del.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int user_choice = msgbox_q_del.exec();
    switch(user_choice)
    {
        case QMessageBox::Ok:
        {
            // remove the file, update tree and editor
            filesystem.remove(index);
            break;
        }
        case QMessageBox::Cancel:
        {
            return;
        }
    }
}

void MainWindow::rename()
{
    rename(current_index);
}

void MainWindow::rename(const QModelIndex & index)
{
    // determine if the selected item is a file or directory
    QString type = filesystem.type(index);
    QString ext = filesystem.fileInfo(index).completeSuffix();
    QString name = filesystem.fileInfo(index).completeBaseName();
    QString path = filesystem.fileInfo(index).absolutePath();
    // ask for the new name
    textinput tid;
    tid.allow_encrypt_sel(false);
    tid.set_label("New file name:");
    tid.set_text(name);
    tid.show();
    while(true)
    {
        if(tid.exec() == QDialog::Accepted)
        {
            QFile newfile;
            QDir dir;
            QString newname = tid.get_text();
            QString completenewname;
            QString completeoldname;
            if(ext == "")
            {
                completeoldname = path+"/"+name;
                completenewname = path+"/"+newname;
            }
            else
            {
                completeoldname = path+"/"+name+"."+ext;
                completenewname = path+"/"+newname+"."+ext;
            }
            newfile.setFileName(completenewname);
            if(newfile.exists())
            {
                tid.set_label("File exists! New name:");
                tid.set_text(newname);
                continue; // file already exists - let user enter better name
            }
            // we have a new valid name, rename!
            dir.rename(completeoldname,completenewname);
            ui->treeView->setCurrentIndex(filesystem.index(completenewname));
            break;
        }
        else
        {
            break;
        }
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox msgbox_about;
    msgbox_about.setText("Qtombo 0.5 2021.06.15");
    msgbox_about.setStandardButtons(QMessageBox::Ok);
    msgbox_about.exec();
}


