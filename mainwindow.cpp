#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadSettings();

    ui->verticalLayoutPlot->addWidget(Plot::instance());
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate().addDays(1));

    QCalendarWidget *begCalendarWidget = new QCalendarWidget(this);
    begCalendarWidget->setFirstDayOfWeek(Qt::Monday);
    ui->dateTimeEditBeg->setCalendarWidget(begCalendarWidget);
    QCalendarWidget *endCalendarWidget = new QCalendarWidget(this);
    endCalendarWidget->setFirstDayOfWeek(Qt::Monday);
    ui->dateTimeEditEnd->setCalendarWidget(endCalendarWidget);
    ui->dateTimeEditBeg->setDateTime(QDateTime::currentDateTime().addDays(-3));
    ui->dateTimeEditEnd->setDateTime(QDateTime::currentDateTime().addDays(1));

    cfgPlot = new CfgPlot(Plot::instance(),this);
    ui->verticalLayoutCfg->addWidget(cfgPlot);

    plotData = new PlotData(this);
    ui->verticalLayoutChn->addWidget(plotData);

    modelDry = new ModelDry(this);

    ui->tableViewDry->setModel(modelDry);
    ui->tableViewDry->setColumnHidden(0,true);
    ui->tableViewDry->setColumnWidth(1,150);
    ui->tableViewDry->setColumnWidth(2,115);
    ui->tableViewDry->setColumnWidth(7,150);
    for (int i=3; i<ui->tableViewDry->model()->columnCount()-1; i++){
        ui->tableViewDry->setColumnHidden(i,true);
    }

    mapper = new DbMapper(ui->tableViewDry,this);
    mapper->addMapping(ui->lineEditT,3);
    mapper->addMapping(ui->lineEditW,4);
    mapper->addMapping(ui->lineEditEnerg,5);
    mapper->addMapping(ui->lineEditDBeg,2);
    mapper->addMapping(ui->lineEditDEnd,6);
    mapper->addMapping(ui->lineEditProg,7);

    mapper->setNewEbabled(false);

    mapper->addLock(ui->cmdUpdDry);
    mapper->addLock(ui->checkBoxMonly);
    mapper->addLock(ui->checkBoxOst);

    ui->horizontalLayoutMap->insertWidget(0,mapper);

    modelOven = new ModelOven(this);
    modelOven->refresh();
    ui->tableViewOven->setModel(modelOven);
    ui->tableViewOven->setColumnHidden(0,true);
    ui->tableViewOven->verticalHeader()->hide();
    ui->tableViewOven->verticalHeader()->setDefaultSectionSize(ui->tableViewOven->verticalHeader()->fontMetrics().height()*1.5);
    ui->tableViewOven->setColumnWidth(1,200);
    ui->tableViewOven->setColumnWidth(2,120);

    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(setCurrentProc(int)));
    connect(ui->tableViewOven->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(setCurrentOven(QModelIndex)));
    connect(ui->cmdUpdOvenData,SIGNAL(clicked(bool)),this,SLOT(refreshOvenData()));
    connect(ui->cmdUpdDry,SIGNAL(clicked(bool)),this,SLOT(refreshDry()));
    connect(ui->checkBoxMonly,SIGNAL(clicked(bool)),this,SLOT(refreshDry()));
    connect(ui->actionPrint,SIGNAL(triggered(bool)),this,SLOT(print()));
    connect(ui->actionExit,SIGNAL(triggered(bool)),this,SLOT(close()));
    connect(ui->actionCfgOven,SIGNAL(triggered(bool)),this,SLOT(edtOven()));
    connect(ui->actionProgs,SIGNAL(triggered(bool)),this,SLOT(edtProg()));
    connect(ui->actionEnerg,SIGNAL(triggered(bool)),this,SLOT(anDryEnerg()));
    connect(ui->checkBoxOst,SIGNAL(clicked(bool)),this,SLOT(setOst(bool)));
    connect(ui->tableViewDry->horizontalHeader(),SIGNAL(sectionClicked(int)),modelDry,SLOT(sort(int)));

    refreshDry();
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    this->restoreGeometry(settings.value("geometry").toByteArray());
    this->ui->splitter->restoreState(settings.value("splitter_width").toByteArray());
}

void MainWindow::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("geometry", this->saveGeometry());
    settings.setValue("splitter_width",ui->splitter->saveState());
}

QString MainWindow::secToStr(qint64 sec)
{
    if (sec==0) return QString();
    QTime t(0,0,0);
    int d = sec / 86400;
    QString suf = (d>0)? QString::number(d)+QString::fromUtf8(" д ") : QString();
    t=t.addSecs(sec);
    return suf+t.toString("hh:mm:ss");
}


void MainWindow::setCurrentProc(int index)
{
    int id_owrab=ui->tableViewDry->model()->data(ui->tableViewDry->model()->index(index,0),Qt::EditRole).toInt();

    QDateTime begTime=ui->tableViewDry->model()->data(ui->tableViewDry->model()->index(index,2),Qt::EditRole).toDateTime();
    QDateTime endTime=ui->tableViewDry->model()->data(ui->tableViewDry->model()->index(index,6),Qt::EditRole).toDateTime();

    QString own=ui->tableViewDry->model()->data(ui->tableViewDry->model()->index(index,1),Qt::DisplayRole).toString();

    QString wl=ui->tableViewDry->model()->data(ui->tableViewDry->model()->index(index,4),Qt::DisplayRole).toString();
    QString tl=ui->tableViewDry->model()->data(ui->tableViewDry->model()->index(index,3),Qt::DisplayRole).toString();

    qint64 tpi=begTime.secsTo(endTime);
    QString tp=secToStr(tpi);
    ui->lineEditS->setText(tp);

    Plot::instance()->setbaseTime(begTime);
    plotData->refresh(id_owrab,ui->checkBoxOst->isChecked());
    double sec=Plot::instance()->canvasMap(QwtPlot::xBottom).s2();

    QString vr=(ui->checkBoxOst->isChecked()) ? secToStr(sec) : tp;
    QString title="<FONT SIZE=3>"+own+tr(", w0=")+wl+tr("; t0=")+tl+";<br>"+
            begTime.toString("dd.MM.yy hh:mm:ss")+" - "+endTime.toString("dd.MM.yy hh:mm:ss")+" ("+vr+")"+"</FONT>";
    Plot::instance()->setTitle(title);
}

void MainWindow::setCurrentOven(QModelIndex index)
{
    int id_oven=ui->tableViewOven->model()->data(ui->tableViewOven->model()->index(index.row(),0),Qt::EditRole).toInt();
    QString nam=ui->tableViewOven->model()->data(ui->tableViewOven->model()->index(index.row(),1),Qt::DisplayRole).toString();
    Plot::instance()->setbaseTime(ui->dateTimeEditBeg->dateTime());
    plotData->refresh(ui->dateTimeEditBeg->dateTime(),ui->dateTimeEditEnd->dateTime(), id_oven);
    Plot::instance()->setTitle("<FONT SIZE=3>"+nam+"<br>"+ui->dateTimeEditBeg->dateTime().toString("dd.MM.yy hh:mm:ss")+
                               " - "+ui->dateTimeEditEnd->dateTime().toString("dd.MM.yy hh:mm:ss")+"</FONT>");
}

void MainWindow::refreshOvenData()
{
    setCurrentOven(ui->tableViewOven->selectionModel()->currentIndex());
}

void MainWindow::refreshDry()
{
    bool b=ui->checkBoxMonly->isChecked();
    ui->dateEditBeg->setEnabled(!b);
    ui->dateEditEnd->setEnabled(!b);
    if (!modelDry->isEmpty()){
        Rels::instance()->refresh();
    }
    modelDry->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date(),b);
}

void MainWindow::print()
{
    QwtPlotRenderer renderer;
    renderer.setDiscardFlag( QwtPlotRenderer::DiscardBackground );
    renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasBackground );
    renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasFrame );
    renderer.setLayoutFlag( QwtPlotRenderer::FrameWithScales );
    QPrinter printer;
    printer.setColorMode(QPrinter::Color);
    QPrintDialog dialog(&printer,this);
    if (dialog.exec()){
        QPainter painter(&printer);
        QRect page = printer.pageRect();
        int w = page.width()-page.x();
        QRect rect(page.x(), page.y(), w*0.95, w*1.3);
        renderer.render(Plot::instance(),&painter,rect);
    }
}

void MainWindow::edtOven()
{
    CfgOwenDialog d;
    d.exec();
    modelOven->refresh();
    Rels::instance()->relOwen->refreshModel();
}

void MainWindow::edtProg()
{
    DialogProg d;
    d.exec();
}

void MainWindow::anDryEnerg()
{
    QStringList axes;
    axes.push_back(tr("Печь"));
    axes.push_back(tr("Название программы"));
    axes.push_back(tr("Год"));
    axes.push_back(tr("Месяц"));
    axes.push_back(tr("День"));
    QString query("select o.num, or2.nam_prog, "
                  "substr(cast(or2.dt_beg as char(32)),1,4) as yr, "
                  "substr(cast(or2.dt_beg as char(32)),1,7) as mn, "
                  "substr(cast(or2.dt_beg as char(32)),1,10) as dy, "
                  "or2.energ "
                  "from owens_rab or2 "
                  "inner join owens o on o.id = or2.id_owen "
                  "where or2.dt_beg::date between :d1 and :d2");
    CubeWidget *kvoCube = new CubeWidget(tr("Расход электрической энергии на отжиг катанки, кВтч"),axes,query,3);
    kvoCube->show();
    connect(kvoCube,SIGNAL(sigClose()),kvoCube,SLOT(deleteLater()));
}

void MainWindow::setOst(bool /*b*/)
{
    setCurrentProc(ui->tableViewDry->currentIndex().row());
}
