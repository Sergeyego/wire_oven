#include "models.h"

ModelOven::ModelOven(QObject *parent) :
    QSqlQueryModel(parent)
{
}

void ModelOven::refresh()
{
    setQuery("select o.id, o.num, coalesce((select sum(pwr) from owens_trm_channel where id_owen = o.id and is_enabled = true),0)||' + '||o.pwr_cool||' kW' from owens as o "
             "order by o.num");
    if (lastError().isValid()){
        QMessageBox::critical(NULL,"Error",lastError().text(),QMessageBox::Ok);
    } else {
        setHeaderData(1, Qt::Horizontal,tr("Печь"));
        setHeaderData(2, Qt::Horizontal,tr("Мощность"));
    }
}


ModelDry::ModelDry(QObject *parent) : DbTableModel("owens_rab", parent)
{
    addColumn("id","id");
    addColumn("id_owen",QString::fromUtf8("Печь"),Rels::instance()->relOwen);
    addColumn("dt_beg",QString::fromUtf8("Дата-время"));
    addColumn("tbeg",QString::fromUtf8("Нач.темп."));
    addColumn("timbeg",QString::fromUtf8("Нач.выд."));
    addColumn("energ",QString::fromUtf8("Энерг."));
    addColumn("dt_end",QString::fromUtf8("Конец"));
    addColumn("nam_prog",QString::fromUtf8("Прог."));

    setSuffix("inner join owens as o on o.id = owens_rab.id_owen");

    setSort("owens_rab.dt_beg");

}

QVariant ModelDry::data(const QModelIndex &index, int role) const
{
    if (role==Qt::BackgroundRole){
        if (!this->data(this->index(index.row(),6),Qt::EditRole).isNull())
            return QVariant(QColor(170,255,170));
    }
    return DbTableModel::data(index,role);
}


void ModelDry::refresh(QDate dBeg, QDate dEnd, bool l_month)
{
    QDate date2 =QDate::currentDate().addDays(-30);
    QString flt;
    flt = l_month ? ("dt_beg >= '"+date2.toString("yyyy-MM-dd")+"' ") :
                    ("dt_beg between '"+dBeg.toString("yyyy-MM-dd")+"' and '"+dEnd.toString("yyyy-MM-dd")+"' ");
    setFilter(flt);
    select();

}

void ModelDry::sort(int section)
{
    QString s="owens_rab.dt_beg";
    if (section==1){
        s="o.num, owens_rab.dt_beg";
    } else if (section==7){
        s="owens_rab.nam_prog, owens_rab.dt_beg";
    }
    if (DbTableModel::sort==s){
        int pos=s.indexOf(",");
        if (pos!=-1){
            s=s.insert(pos," desc");
        } else {
            s+=" desc";
        }
    }
    setSort(s);
    select();
}

