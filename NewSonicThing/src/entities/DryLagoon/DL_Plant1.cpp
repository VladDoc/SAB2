#include "dlplant1.hpp"
#include "../entity.hpp"
#include "../../engineTester/main.hpp"
#include "../../models/models.hpp"
#include "../../loading/modelloader.hpp"

std::list<TexturedModel*> DL_Plant1::models;

DL_Plant1::DL_Plant1()
{

}

DL_Plant1::DL_Plant1(
    float x, float y, float z,
    float rotX, float rotY, float rotZ,
    float scale)
{
    position.set(x, y, z);
    this->rotX = rotX;
    this->rotY = rotY;
    this->rotZ = rotZ;
    this->scale = scale;
    updateTransformationMatrixYXZ();
}

void DL_Plant1::step()
{
   
}

std::list<TexturedModel*>* DL_Plant1::getModels()
{
    return &DL_Plant1::models;
}

void DL_Plant1::loadStaticModels()
{
    if (DL_Plant1::models.size() > 0)
    {
        return;
    }

    #ifdef DEV_MODE
    std::fprintf(stdout, "Loading DL_Plant1 static models...\n");
    #endif

    ModelLoader::loadModel(&DL_Plant1::models, "res/Models/Levels/DryLagoon/Objects/", "Leaf");
}

void DL_Plant1::deleteStaticModels()
{
    #ifdef DEV_MODE
    std::fprintf(stdout, "Deleting DL_Plant1 static models...\n");
    #endif

    Entity::deleteModels(&DL_Plant1::models);
}
