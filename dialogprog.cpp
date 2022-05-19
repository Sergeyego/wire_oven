#include "dialogprog.h"
#include "ui_dialogprog.h"

DialogProg::DialogProg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogProg)
{
    ui->setupUi(this);

    modelProg = new DbTableModel("owens_progs",this);
    modelProg->addColumn("id",tr("ID"));
    modelProg->addColumn("nam",tr("Название"));
    modelProg->setSort("owens_progs.id");
    modelProg->select();

    modelCont = new DbTableModel("owens_progs_data",this);
    modelCont->addColumn("id_prog",tr("id_prog"));
    modelCont->addColumn("step",tr("Шаг"));
    modelCont->addColumn("tim",tr("Время, мин"));
    modelCont->addColumn("tra",tr("Темпер., град."));
    modelCont->setSort("owens_progs_data.step");

    ui->tableViewCont->setModel(modelCont);
    ui->tableViewCont->setColumnHidden(0,true);
    ui->tableViewCont->setColumnWidth(1,50);
    ui->tableViewCont->setColumnWidth(2,120);
    ui->tableViewCont->setColumnWidth(3,120);

    ui->tableViewProg->setModel(modelProg);
    ui->tableViewProg->setColumnWidth(0,50);
    ui->tableViewProg->setColumnWidth(1,170);

    mapper = new DbMapper(ui->tableViewProg,this);
    ui->verticalLayoutCont->addWidget(mapper);
    mapper->addMapping(ui->lineEditName,1);
    mapper->addEmptyLock(ui->tableViewCont);

    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(updCont(int)));

    if (ui->tableViewProg->model()->rowCount()) ui->tableViewProg->selectRow(0);
}

DialogProg::~DialogProg()
{
    delete ui;
}

void DialogProg::updCont(int index)
{
    int id_progs=ui->tableViewProg->model()->data(ui->tableViewProg->model()->index(index,0),Qt::EditRole).toInt();
    modelCont->setFilter("owens_progs_data.id_prog="+QString::number(id_progs));
    modelCont->setDefaultValue(0,id_progs);
    modelCont->select();
}
