#include <glad/glad.h>

#include "../loading/modelloader.hpp"
#include "entity.hpp"
#include "../models/models.hpp"
#include "stagetransparent.hpp"
#include "../engineTester/main.hpp"

#include <list>

std::vector<TexturedModel*> StageTransparent::models;

StageTransparent::StageTransparent()
{
    
}

StageTransparent::StageTransparent(const char* objFolder, const char* objFilename)
{
    position.x = 0;
    position.y = 0;
    position.z = 0;
    rotX = 0;
    rotY = 0;
    rotZ = 0;
    scale = 1;
    visible = true;
    
    if (StageTransparent::models.size() == 0)
    {
        #ifdef DEV_MODE
        printf("Loading StageTransparent static models...\n");
        #endif

        ModelLoader::loadModel(&StageTransparent::models, objFolder, objFilename);

        setModelsRenderOrder(&StageTransparent::models, 3);
    }
    
    updateTransformationMatrix();
}

void StageTransparent::step()
{

}

std::vector<TexturedModel*>* StageTransparent::getModels()
{
    return &StageTransparent::models;
}

void StageTransparent::deleteStaticModels()
{
    #ifdef DEV_MODE
    printf("Deleting StageTransparent static models...\n");
    #endif

    Entity::deleteModels(&StageTransparent::models);
}
