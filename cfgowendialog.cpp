#include "cfgowendialog.h"
#include "ui_cfgowendialog.h"

CfgOwenDialog::CfgOwenDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CfgOwenDialog)
{
    ui->setupUi(this);

    modelOven = new DbTableModel("owens",this);
    modelOven->addColumn("id","id");
    modelOven->addColumn("num",tr("Название"));
    modelOven->addColumn("pwr_cool",tr("Мощность вент"));
    modelOven->setSort("owens.num");
    modelOven->select();

    modelChannel = new DbTableModel("owens_trm_channel",this);
    modelChannel->addColumn("id_owen",tr("id_owen"));
    modelChannel->addColumn("id",tr("Id"));
    modelChannel->addColumn("number",tr("Номер"));
    modelChannel->addColumn("nam",tr("Название"));
    modelChannel->addColumn("is_main",tr("Главный"));
    modelChannel->addColumn("color_ust",tr("Уставка"));
    modelChannel->addColumn("color_val",tr("Значение"));
    modelChannel->addColumn("pwr",tr("Мощность"));
    modelChannel->addColumn("is_enabled",tr("Включен"));

    modelChannel->setSort("owens_trm_channel.number, owens_trm_channel.nam");
    modelChannel->setColumnFlags(1,Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    ui->tableViewCh->setModel(modelChannel);
    ui->tableViewCh->setColumnHidden(0,true);
    ui->tableViewCh->setColumnWidth(1,50);
    ui->tableViewCh->setColumnWidth(2,50);
    ui->tableViewCh->setColumnWidth(3,120);
    ui->tableViewCh->setColumnWidth(4,80);
    ui->tableViewCh->setColumnWidth(5,80);
    ui->tableViewCh->setColumnWidth(6,80);
    ui->tableViewCh->setColumnWidth(7,80);
    ui->tableViewCh->setColumnWidth(8,80);

    ui->tableViewCh->setItemDelegateForColumn(5,new ColorDelegate(this));
    ui->tableViewCh->setItemDelegateForColumn(6,new ColorDelegate(this));

    ui->listViewOven->setModel(modelOven);
    ui->listViewOven->setModelColumn(1);
    push = new DbMapper(ui->listViewOven,this);
    ui->horizontalLayoutMap->insertWidget(0,push);
    push->addMapping(ui->lineEditId,0);
    push->addMapping(ui->lineEditNum,1);
    push->addMapping(ui->lineEditPv,2);
    push->addLock(ui->listViewOven);
    push->addLock(ui->tableViewCh);

    connect(push,SIGNAL(currentIndexChanged(int)),this,SLOT(setCurrentOven(int)));
}

CfgOwenDialog::~CfgOwenDialog()
{
    delete ui;
}

void CfgOwenDialog::setCurrentOven(int index)
{
    int id_oven=ui->listViewOven->model()->data(ui->listViewOven->model()->index(index,0),Qt::EditRole).toInt();
    modelChannel->setFilter("owens_trm_channel.id_owen = "+QString::number(id_oven));
    modelChannel->setDefaultValue(0,id_oven);
    modelChannel->select();
}
