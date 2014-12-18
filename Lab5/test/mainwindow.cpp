#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "contactsdll.h"
#include "QDebug"
#include "string"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    base = LoadBase(L"ContactsBase.txt");

    char *test = (char*)base;
    qDebug() << test;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    qDebug() << ui->comboBox->currentIndex() << ui->lineEdit->text();
    std::string res = ParallelSearch(ui->comboBox->currentIndex(), (char *)ui->lineEdit->text().toStdString().c_str(), base);
    ui->plainTextEdit->setPlainText(res.c_str());
}
