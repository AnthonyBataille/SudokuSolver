#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "solver.h"

std::shared_ptr<Solver> solver{std::make_shared<Solver>()};

void initTable(QTableWidget* grid){
    for(int i = 0; i < 9; ++i){
        for(int j = 0; j < 9; ++j){
            if(!grid->item(i, j)){
                grid->setItem(i, j, new QTableWidgetItem("0"));
                std::cout << "new item set" << std::endl;
            }
        }
    }
}
void destroyTable(QTableWidget* grid){
    for(int i = 0; i < 9; ++i){
        for(int j = 0; j < 9; ++j){
            if(grid->item(i, j)){
                delete grid->item(i, j);
                grid->setItem(i, j, nullptr);
            }
        }
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/sudoku.ico"));
    setFixedSize(width(), height());
    solver->loadSoundEffects();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_resetButton_clicked()
{
    //destroyTable(ui->sudokuTable);
    for(int i = 0; i < 9; ++i){
        for(int j = 0; j < 9; ++j){
            ui->sudokuTable->item(i, j)->setText("");
        }
    }
    ui->resultLabel->setStyleSheet("QLabel {color : black;}");
    ui->resultLabel->setText("Result");
}

void MainWindow::on_solveButton_clicked()
{
    //initTable(ui->sudokuTable);


    int iterations_count = 0;

    if(solver->solveFromTable(ui->sudokuTable, iterations_count)){
        ui->resultLabel->setStyleSheet("QLabel {color : green;}");
        ui->resultLabel->setText(QString("<b>Puzzle solved !</b> (%1 iterations)").arg(iterations_count));
        if(ui->effectsCheckBox->isChecked())
            solver->playWoawSound();
    }
    else{
        ui->resultLabel->setStyleSheet("QLabel {color : red;}");
        ui->resultLabel->setText(QString("<b>This puzzle is unfeasible !</b> (%1 iterations)").arg(iterations_count));
        if(ui->effectsCheckBox->isChecked())
            solver->playNaniSound();
    }
}
