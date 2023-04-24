#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include "datawindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    QFileSystemModel *model = new QFileSystemModel;
    model->setRootPath(QDir::currentPath());

    Map=ui->Map;

    QGridLayout* g = new QGridLayout(ui->Center);
    g->addWidget(Map);

    //if you don't have QGS comment bottom line
    QgsController = new QGSController(Map);
    ui->TreeAddedItems->clear();
    
    SetLine = ui->SetLine;
    RadarBtn = ui->RadarButton;
    connect(SetLine, &QPushButton::clicked, QgsController, &QGSController::addLine);
    SetLine->hide();
    connect(ui->LinesButton, &QPushButton::clicked, this, &MainWindow::showLinesListWidget);
    LinesWidgetInit();
    connect(ui->TreeLinesWidget, &QTreeWidget::itemClicked, this, &MainWindow::getLineId);
    connect(QgsController, &QGSController::sendLine, this, &MainWindow::addLine);
    connect(RadarBtn, &QPushButton::clicked, QgsController, &QGSController::showRadarZones);
}

MainWindow::~MainWindow(){
    delete ui;
}


void MainWindow::show(){
    QMainWindow::show();
    ui->DockWidgetForTree->raise();
    ui->DockWidgetForTree->close();
    ui->TreeLinesWidget->hide();
}

void MainWindow::on_actionNew_triggered(){
    //if you don't have QGS comment bottom line
    QgsController->addLayer();
}
void MainWindow::on_actionauthors_triggered(){
    //TODO вынести в connect это
    //QgsController->activateSelectingPoint();
    //QMessageBox *msg = new QMessageBox;
    //msg->setText(" Max1 \n Max2 \n Ilya \n Nikita \n Oleg");
    //msg->exec();
}

void MainWindow::on_actionExit_triggered(){
    close();
}

void MainWindow::on_TreeAddedItems_itemClicked(QTreeWidgetItem *item, int column){
    if (item->childCount()!=0)
        return;
    Table type=dynamic_cast<MyTreeItem*>(item)->get_type();
    if (type == ZRK)
        QgsController->activateSelectingSquare();
    if (type == AIRPLANS)
        QgsController->activateSelectingPoint();
}

MyTreeItem::MyTreeItem(MyTreeItem *parent, int id, QString name, int speed, int mass, Table type) : QTreeWidgetItem(parent){
    this->id=id;
    this->name = name;
    this->speed = speed;
    this->mass = mass;
    this->type=type;
    this->setText(0, name);
}

MyTreeItem::MyTreeItem(QTreeWidget *parent, int id, QString name, int speed, int mass, Table type): QTreeWidgetItem(parent){
    this->id=id;
    this->name = name;
    this->speed = speed;
    this->mass = mass;
    this->type=type;
    this->setText(0, name);
}

void MyTreeItem::get_info()
{
    qInfo() << id;
    qInfo() << name;
    qInfo() << speed;
    qInfo() << mass;
}

Table MyTreeItem::get_type() const{
    return type;
}


void MainWindow::on_DataBaseButton_clicked(){
    dbController.dataWindow_show();
    if(ui->TreeAddedItems->topLevelItemCount()==0)
        fillTreeFromDb();
}

void MainWindow::fillTreeFromDb()
{
    QVector<InfoAboutElement> planes = dbController.select_all(AIRPLANS);
    QVector<InfoAboutElement> zrks = dbController.select_all(ZRK);
    MyTreeItem *zrk = new MyTreeItem(ui->TreeAddedItems, 0, "ЗРК");
    MyTreeItem *plane = new MyTreeItem(ui->TreeAddedItems, 1, "Самолеты");
    MyTreeItem *gyro = new MyTreeItem(ui->TreeAddedItems, 2, "Вертолеты");

    zrk->setIcon(0, QIcon(":/rec/img/zrk.png"));
    plane->setIcon(0, QIcon(":/rec/img/plane.png"));
    gyro->setIcon(0, QIcon(":/rec/img/gyrocopter.png"));

    //childs

    MyTreeItem *firstZrk = new MyTreeItem(zrk, zrks[0].id, zrks[0].name, zrks[0].speed, zrks[0].mass,zrks[0].type); //toDO:: create ctr from InfoElements
    MyTreeItem *secondZrk = new MyTreeItem(zrk, zrks[1].id, zrks[1].name, zrks[1].speed, zrks[1].mass,zrks[1].type);
    MyTreeItem *thirdZrk = new MyTreeItem(zrk, zrks[2].id, zrks[2].name, zrks[2].speed, zrks[2].mass,zrks[2].type);

    MyTreeItem *firstPlane = new MyTreeItem(plane, planes[0].id, planes[0].name, planes[0].speed, planes[0].mass,planes[0].type);
    MyTreeItem *secondPlane = new MyTreeItem(plane, planes[1].id, planes[1].name, planes[1].speed, planes[1].mass,planes[1].type);
    MyTreeItem *thirdPlane = new MyTreeItem(plane, planes[2].id, planes[2].name, planes[2].speed, planes[2].mass,planes[2].type);

    MyTreeItem *firstGyro = new MyTreeItem(gyro, 2);

}


void MainWindow::on_addFromTreeButton_clicked(){
    if ((!ui->DockWidgetForTree->isVisible()))
        ui->DockWidgetForTree->show();
    else
        ui->DockWidgetForTree->close();
}

void MainWindow::on_actionHand_triggered(){
    QgsController->activatePanTool();
}

void MainWindow::on_actionLine_triggered(){
    SetLine->show();
    SetLine->raise();
    QgsController->selectionPoints();
    connect(QgsController->selectionPointTool, &QgsMapToolEmitPoint::deactivated, this, &MainWindow::setLineHide);
    //приходится курсор доставать
}
void MainWindow::setLineHide(){
    SetLine->hide();
}
void MainWindow::showLinesListWidget(){
    if ((!ui->TreeLinesWidget->isVisible())){
        ui->TreeLinesWidget->show();
        ui->TreeLinesWidget->raise();
    }
    else
        ui->TreeLinesWidget->hide();
}


LineTreeItem::LineTreeItem(LineTreeItem *parent, int id, QString name) : QTreeWidgetItem(parent){
    this->id=id;
    this->name = name;
    this->setText(0, name);
}

LineTreeItem::LineTreeItem(QTreeWidget *parent, int id, QString name): QTreeWidgetItem(parent){
    this->id=id;
    this->name = name;
    this->setText(0, name);
}

void MainWindow::LinesWidgetInit(){
    lines = new LineTreeItem(ui->TreeLinesWidget, 0, "Линии");
    lines->setIcon(0, QIcon(":/rec/img/line.png"));
}

void MainWindow::addLine(int id, QString name){
    LineTreeItem *line = new LineTreeItem(lines, id, name);
    line->setFlags(line->flags() | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

void MainWindow::getLineId(QTreeWidgetItem *item, int column){
    QgsController->getLineId(dynamic_cast<LineTreeItem*>(item)->id);
};

/*void MainWindow::showRadarWidget(){
    if ((!RadarWidget->isVisible())){
        RadarWidget->show();
        RadarWidget->raise();
    }
    else
        RadarWidget->hide();
}

void MainWindow::initRadarWidget(){
//    RadarLayout = new QVBoxLayout(RadarWidget);
    QLabel* drawingLabel = new QLabel("Radar", RadarWidget);
    drawingLabel->setAlignment(Qt::AlignCenter);
    QPicture* pic = new QPicture();
    QPainter* painter = new QPainter(pic);
    QPen* pen = new QPen();
    pen->setWidth(3);
    pen->setBrush(Qt::SolidPattern);
    pen->setColor(QColor(84, 64, 237, 100));
    painter->setPen(*pen);
    //painter->setRenderHint(QPainter::Antialiasing);
    painter->drawEllipse(0, 0, 1, 1);//for start point
    painter->drawEllipse(85, 65, 100, 100);
    painter->drawEllipse(0, 0, 200, 200);
    painter->drawEllipse(0, 0, 50, 50);
    painter->end();

    drawingLabel->setPicture(*pic);    
}*/