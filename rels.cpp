#include "rels.h"

Rels* Rels::rels_instance=nullptr;

Rels *Rels::instance()
{
    if (rels_instance==nullptr)
        rels_instance = new Rels();
    return rels_instance;
}

Rels::Rels(QObject *parent) : QObject(parent)
{
    relOwen = new DbRelation(QString("select id, num from owens order by num"),0,1,this);
}

void Rels::refresh()
{
    relOwen->refreshModel();
}
