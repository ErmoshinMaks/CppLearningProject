#include "engine.h"
#include <QDebug>

Engine::Engine() {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Engine::moveObjects);
    sendTimer = new QTimer(this);
    connect(sendTimer, &QTimer::timeout, this, &Engine::packObjects);
};

void Engine::createNewObject(InfoAboutElement element){
    lastelement_ = element; //можно threadом сделать без сохранения, но пока сделаем с сохранением параметра.
    switch (element.type)
    {
        case AIRPLANS:
        {
            emit planeCreated();
        }
        break;
        case ZRK:
        {
            //auto sam = ObjectFactory::CreateSAM(element.mass, element.name, element.distance, std::make_unique<Point>(0,0));
            emit samCreated(); 
        }
        break;
        default:
            break;
    }
}

void Engine::addLine(QVector<QPair<double, double>>* linePoints){
    for(auto i = linePoints->begin(); i<linePoints->end(); ++i)
        allLines.push_back(new Point((*i).first, (*i).second));
}

void Engine::addSAM(double x, double y){ 
    SAM* sam = new SAM(lastelement_.mass, lastelement_.name, lastelement_.distance, new Point(x,y)); //создал SAM здесь, кинул emit об этом в QGIS, QGIS дал мне точку на которую кликнули, я её добавил в движке и кинул QGIS обратно какого радиуса круги и где создать
    sams.push_back(sam);
    emit createSAMCircles(x, y, sams[sams.size()-1]->distance());
}

void Engine::addPlane(QVector<QPair<double, double>>* points) {
    QVector<Point*>* vec = new QVector<Point*>(0); 
    for(auto i : *points)
        vec->push_back(new Point(i.first, i.second));
    Plane* plane = new Plane(lastelement_.mass,lastelement_.speed,lastelement_.name, vec);
    planes.push_back(plane);
}

void Engine::startRenderCycle(){
    if(!timer->isActive())
        timer->start(100);
    if(!sendTimer->isActive())
        sendTimer->start(500);
}

void Engine::pauseRenderCycle(){
    if(timer->isActive())
        timer->stop();
    if(sendTimer->isActive())
        sendTimer->stop();
}

void Engine::moveObjects(){
    for(int i = 0; i<planes.size(); ++i)
        planes[i]->Move();
    for(int i = 0; i<rockets.size(); ++i)
        rockets[i]->Move();
    SAMscane();
}

void Engine::SAMscane(){
    for(int i = 0; i<sams.size(); ++i)
        for(int j = 0; j<planes.size(); ++j){
            if(sams[i]->length(planes[j])<sams[i]->distance() & rockets.size() < 1){
                Rocket* rocket = new Rocket(1000, 1200, 0.5, sams[i], planes[j]);
                rockets.push_back(rocket);
                emit rocketCreated(sams[i]->X(), sams[i]->Y());
            }
        }
}

void Engine::packObjects(){
    QVector<QList<double>>* sendPlanes = new QVector<QList<double>>(0);
    for(int i = 0; i<planes.size(); ++i)
        sendPlanes->push_back({planes[i]->X(), planes[i]->Y(), planes[i]->retAngle()});
    QVector<QPair<double, double>>* sendSams = new QVector<QPair<double, double>>(0);
    for(int i = 0; i<sams.size(); ++i)
        sendSams->push_back({sams[i]->X(), sams[i]->Y()});
    QVector<QList<double>>* sendRockets = new QVector<QList<double>>(0);
    for(int i = 0; i<rockets.size(); ++i)
        sendRockets->push_back({rockets[i]->X(), rockets[i]->Y(), rockets[i]->retAngle()});
    emit sendObjects(sendSams, sendPlanes, sendRockets);
}