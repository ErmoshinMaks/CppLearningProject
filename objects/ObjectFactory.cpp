#include "ObjectFactory.h"


std::unique_ptr<Point> ObjectFactory::CreateTarget(float x, float y) {
    return std::make_unique<Point>(x, y);
}

std::unique_ptr<Plane> ObjectFactory::CreatePlane(float health, float speed, const QString& model) {
    return std::make_unique<Plane>(health, speed, model);
}

std::unique_ptr<SAM> ObjectFactory::CreateSAM(float health, const QString& model) {
    return std::make_unique<SAM>(health, model);
}