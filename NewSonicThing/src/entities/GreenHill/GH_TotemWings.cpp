#include <glad/glad.h>

#include "../entity.h"
#include "../../models/models.h"
#include "../../toolbox/vector.h"
#include "ghtotemwings.h"
#include "../../objLoader/objLoader.h"
#include "../../engineTester/main.h"
#include "../../collision/collisionmodel.h"
#include "../../collision/collisionchecker.h"

#include <list>
#include <iostream>

std::list<TexturedModel*> GH_TotemWings::models;
CollisionModel* GH_TotemWings::cmOriginal = nullptr;

GH_TotemWings::GH_TotemWings()
{
    
}

GH_TotemWings::GH_TotemWings(float x, float y, float z, float rotY, float scale)
{
    position.x = x;
    position.y = y;
    position.z = z;
    rotX = 0;
    this->rotY = rotY;
    rotZ = 0;
    this->scale = scale;
    visible = true;
    updateTransformationMatrix();

    collideModelOriginal = GH_TotemWings::cmOriginal;
    collideModelTransformed = collideModelOriginal->duplicateMe();

    CollisionChecker::addCollideModel(collideModelTransformed);

    updateCollisionModel();
}

void GH_TotemWings::step()
{

}

std::list<TexturedModel*>* GH_TotemWings::getModels()
{
    return &GH_TotemWings::models;
}

void GH_TotemWings::loadStaticModels()
{
    if (GH_TotemWings::models.size() > 0)
    {
        return;
    }

    #ifdef DEV_MODE
    std::fprintf(stdout, "Loading GH_TotemWings static models...\n");
    #endif

    ObjLoader::loadModel(&GH_TotemWings::models, "res/Models/Levels/GreenHillZone/Objects/", "GreenHillTotemWings");

    if (GH_TotemWings::cmOriginal == nullptr)
    {
        GH_TotemWings::cmOriginal = ObjLoader::loadCollisionModel("Models/Levels/GreenHillZone/Objects/", "GreenHillTotemCollision");
    }
}

void GH_TotemWings::deleteStaticModels()
{
    #ifdef DEV_MODE
    std::fprintf(stdout, "Deleting GH_TotemWings static models...\n");
    #endif

    Entity::deleteModels(&GH_TotemWings::models);
    Entity::deleteCollisionModel(&GH_TotemWings::cmOriginal);
}
