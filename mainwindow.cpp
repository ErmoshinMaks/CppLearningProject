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
    Map=ui->Map;
    dbController = new DatabaseController;

    QFileSystemModel *model = new QFileSystemModel;
    QGridLayout* g = new QGridLayout(ui->Center);
    //objFactory = new ObjectFactory;
    g->addWidget(Map);

    model->setRootPath(QDir::currentPath());

    //if you don't have QGS comment bottom line
    //QgsController = new QGSController(Map);

    connect(dbController, SIGNAL(sig_addedToDb()), this, SLOT(addedToDb()));

    float x = 543343334343.433;
    float y = 1.0;
    float scale = 135;
    QLabel *spacer = new QLabel(); // fake spacer
    ui->statusbar->addPermanentWidget(spacer, 1);
    ui->statusbar->addPermanentWidget(ui->labelForIcon);
    ui->statusbar->addPermanentWidget(ui->labelForText);
    ui->statusbar->addPermanentWidget(ui->labelForCoord);
    ui->labelForCoord->setText(QString("%1 : %2").arg(x).arg(y));
    ui->statusbar->addPermanentWidget(ui->labelForTextScale);
    ui->statusbar->addPermanentWidget(ui->labelForScale);
    ui->labelForScale->setText(QString("%1").arg(scale));


}

MainWindow::~MainWindow(){
    delete ui;
}



void MainWindow:: show(){
    QMainWindow::show();
    ui->DockWidgetForTree->raise();
    ui->DockWidgetForTree->close();
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if ( (e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return)){
        qInfo() << "lol, this is right";
    }
}

void MainWindow::on_actionNew_triggered(){
    //if you don't have QGS comment bottom line
    //QgsController->addLayer();
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
    Table type = dynamic_cast<MyTreeItem*>(item)->get_type();
    int id = dynamic_cast<MyTreeItem*>(item)->get_id();
    //dynamic_cast<MyTreeItem*>(item)->get_info();
    this->create_new_object(id,type);
    switch (type) {
    case ZRK:
        //QgsController->activateSelectingSquare();
        break;
    case AIRPLANS:
        //QgsController->activateSelectingPoint();
        break;
    default:
        break;
    }
}

MyTreeItem::MyTreeItem(MyTreeItem *parent, int id, Table type, QString name, int speed, int mass, int distance, int damage) : QTreeWidgetItem(parent){
    this->id=id;
    this->type=type;
    this->name = name;
    this->speed = speed;
    this->mass = mass;
    this->distance = distance;
    this->damage = damage;
    this->setText(0, name);
}

MyTreeItem::MyTreeItem(QTreeWidget *parent, int id, QString name) : QTreeWidgetItem(parent){
    this->id=id;
    this->name = name;
    this->setText(0, name);
}



int MyTreeItem::get_id() const{
    return id;
}
Table MyTreeItem::get_type() const{
    return type;
}


void MainWindow::on_DataBaseButton_clicked(){
    dbController->dataWindow_show();
}

void MainWindow::fillTreeFromDb()
{
    ui->TreeAddedItems->clear();

    QVector<InfoAboutElement> planes = dbController->select_all(AIRPLANS);
    QVector<InfoAboutElement> zrks = dbController->select_all(ZRK);
    MyTreeItem *zrk = new MyTreeItem(ui->TreeAddedItems, 0,  "ЗРК");
    MyTreeItem *plane = new MyTreeItem(ui->TreeAddedItems, 1, "Самолеты");
    MyTreeItem *gyro = new MyTreeItem(ui->TreeAddedItems, 2,  "Вертолеты");

    zrk->setIcon(0, QIcon(":/rec/img/zrk.png"));
    plane->setIcon(0, QIcon(":/rec/img/plane.png"));
    gyro->setIcon(0, QIcon(":/rec/img/gyrocopter.png"));

    //childs
    int sizeOfplanes = planes.size();
    for(int i = 0; i<sizeOfplanes ;i++){
        MyTreeItem *pl = new MyTreeItem(plane, planes[i].id, planes[i].type, planes[i].name, planes[i].speed, planes[i].mass, 0 , 0);
    }


    int sizeOfzrks = zrks.size();
    for(int i = 0; i<sizeOfzrks ;i++){
        MyTreeItem *sam = new MyTreeItem(zrk, zrks[i].id, zrks[i].type, zrks[i].name, 0, 0, zrks[i].distance, zrks[i].damage);
    }

    MyTreeItem *firstGyro = new MyTreeItem(gyro, 2);

}

void MainWindow::create_new_object(int id,Table type)//временное создание объектов(потом переделать) то есть сделать это по клику
{
    InfoAboutElement element = dbController->select(type,id);
    switch (type)
    {
        case AIRPLANS:
            {
                auto plane = ObjectFactory::CreatePlane(element.mass,element.speed,element.name);
            }
        break;
        case ZRK:
            {
        auto zrk = ObjectFactory::CreateSAM(element.mass,element.name, element.distance, Point(0,0));
            }
        break;
        default:
            break;
    }
//    if(type == AIRPLANS)
//        auto plane = ObjectFactory::CreatePlane(element.mass,element.speed,element.name);
//    else if(type == ZRK)
    //        auto zrk = ObjectFactory::CreateSAM(element.mass,element.name);
}

void MainWindow::addedToDb()
{
    qInfo() << "DataBase Updated" ;
    fillTreeFromDb();
}


void MainWindow::on_addFromTreeButton_clicked(){

    if ((!ui->DockWidgetForTree->isVisible()))//maybe you must write '!' (on macOS it does not work)
        ui->DockWidgetForTree->show();
    else
        ui->DockWidgetForTree->close();

    if(ui->TreeAddedItems->topLevelItemCount()==0)
        fillTreeFromDb();
}
