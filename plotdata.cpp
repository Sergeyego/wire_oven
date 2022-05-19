#include "plotdata.h"

PlotData::PlotData(QWidget *parent) :
    QWidget(parent)
{
    this->setLayout(new QVBoxLayout(this));
}

void PlotData::clearData()
{
    foreach (SrcChannel *s, srcData) {
        s->clear();
        this->layout()->removeWidget(s);
        delete s;
    }
    srcData.clear();
}

void PlotData::refresh(int id_owrab, bool ost)
{
    clearData();
    QSqlQuery query;
    query.prepare("select distinct n.id_channel as id, ch.number as num from owens_data_new as n "
                  "inner join owens_trm_channel as ch on ch.id=n.id_channel "
                  "where n.id_ow_rab= :id1 order by ch.number");

    query.bindValue(":id1",id_owrab);
    if (query.exec()){
        if (query.size()){
            while (query.next()){
                srcData.push_back(new SrcChannel(query.value(0).toInt(),id_owrab,ost,this));
                this->layout()->addWidget(srcData.at(srcData.size()-1));
            }
        } else {
            query.clear();
            query.prepare("select c.id as id, c.number as num from owens_trm_channel as c "
                          "inner join owens_rab as r on r.id_owen=c.id_owen "
                          "where r.id = :id2 and c.is_enabled=true order by c.number");
            query.bindValue(":id2",id_owrab);
            if (query.exec()){
                while (query.next()){
                    srcData.push_back(new SrcChannel(query.value(0).toInt(),id_owrab,ost,this));
                    this->layout()->addWidget(srcData.at(srcData.size()-1));
                }
            } else {
                QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Ok);
            }
        }
    } else {
        QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Ok);
    }
}

void PlotData::refresh(QDateTime dBeg, QDateTime dEnd, int id_oven)
{
    clearData();
    QSqlQuery query;
    query.prepare("select distinct d.id_channel, c.number from owens_data_new as d "
                  "inner join owens_trm_channel as c on c.id = d.id_channel "
                  "where c.id_owen = :id and d.dat_time between :d1 and :d2 "
                  "order by c.number");
    query.bindValue(":id",id_oven);
    query.bindValue(":d1",dBeg);
    query.bindValue(":d2",dEnd);
    if (query.exec()){
        while (query.next()){
            srcData.push_back(new SrcChannel(query.value(0).toInt(),dBeg,dEnd,this));
            this->layout()->addWidget(srcData.at(srcData.size()-1));
        }
    } else {
        QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Ok);
    }
}
