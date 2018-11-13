#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPushButton *button1=new QPushButton();
    button1->setText(QString::fromLocal8Bit("¹Ø±Õ"));
    connect(button1,SIGNAL(clicked()),this,SLOT(close()));

    ui->statusBar->addWidget(button1);
}

MainWindow::~MainWindow()
{
    delete ui;
}
