#include <glad/glad.h>

#include "entity.hpp"
#include "../models/models.hpp"
#include "../toolbox/vector.hpp"
#include "goalring.hpp"
#include "../loading/modelloader.hpp"
#include "../engineTester/main.hpp"
#include "../entities/controllableplayer.hpp"
#include "../toolbox/maths.hpp"
#include "dummy.hpp"
#include "../entities/camera.hpp"
#include "../audio/audioplayer.hpp"
#include "../audio/source.hpp"
#include "../particles/particleresources.hpp"
#include "../particles/particle.hpp"
#include "../menu/timer.hpp"

#include <list>
#include <iostream>
#include <cmath>

std::list<TexturedModel*> GoalRing::modelsRing;
std::list<TexturedModel*> GoalRing::modelsText;

GoalRing::GoalRing()
{

}

GoalRing::~GoalRing()
{
    if (sourceRing != nullptr)
    {
        sourceRing->stop();
        sourceRing = nullptr;
    }
}

GoalRing::GoalRing(float x, float y, float z)
{
    position.x = x;
    position.y = y;
    position.z = z;
    scale = 1;
    visible = true;

    text = new Dummy(&GoalRing::modelsText); INCR_NEW("Entity");
    text->setVisible(true);
    text->setPosition(&position);
    Global::addEntity(text);
}

void GoalRing::step()
{
    if (Global::gameIsRingMode ||
        Global::gameIsChaoMode)
    {
        Global::deleteEntity(this);
        Global::deleteEntity(text);
    }

    if (Global::gameIsRaceMode && Global::mainHudTimer != nullptr)
    {
        if (Global::mainHudTimer->totalTime >= Global::gameRaceTimeLimit)
        {
            Global::deleteEntity(this);
            Global::deleteEntity(text);

            Global::gameMainPlayer->die();
        }
    }

    extern float dt;
    increaseRotation(0, dt*100, 0);
    text->setRotation(0, rotY, 0, 0);
    
    float distToPlayerSquared = (Global::gameMainPlayer->getCenterPosition() - position).lengthSquared();

    if (distToPlayerSquared < 800.0f*800.0f)
    {
        if (sourceRing == nullptr)
        {
            sourceRing = AudioPlayer::play(42, &position, 1.0f, true);
        }
    }
    else
    {
        if (sourceRing != nullptr)
        {
            sourceRing->stop();
            sourceRing = nullptr;
        }
    }

    if (distToPlayerSquared < 31.7177f*31.7177f && //25.7177 from ring + 6 from sonic
        Global::finishStageTimer < 0.0f)
    {
        Global::mainHudTimer->freeze(true);
        Global::finishStageTimer = 0.0f;
    }

    if (Global::finishStageTimer >= 0.0f)
    {
        if (Global::finishStageTimer < 1.0f)
        {
            float size = 1.0f - 2.4f*Global::finishStageTimer;

            scale = size;
            text->scale = size;

            if (sourceRing != nullptr)
            {
                sourceRing->setVolume(AudioPlayer::soundLevelSFX*(1.0f - Global::finishStageTimer));
            }
        }
        else
        {
            visible = false;
            text->setVisible(false);

            if (sourceRing != nullptr)
            {
                sourceRing->stop();
                sourceRing = nullptr;
            }
        }
    }

    updateTransformationMatrix();
    text->updateTransformationMatrix();
}

std::list<TexturedModel*>* GoalRing::getModels()
{
    return &GoalRing::modelsRing;
}

void GoalRing::loadStaticModels()
{
    if (GoalRing::modelsRing.size() > 0)
    {
        return;
    }

    #ifdef DEV_MODE
    std::fprintf(stdout, "Loading GoalRing static models...\n");
    #endif

    ModelLoader::loadModel(&GoalRing::modelsRing, "res/Models/Objects/GoalRing/", "Pass1");
    ModelLoader::loadModel(&GoalRing::modelsText, "res/Models/Objects/GoalRing/", "Pass2");

    setModelsRenderOrder(&GoalRing::modelsText, 1);
}

void GoalRing::deleteStaticModels()
{
    #ifdef DEV_MODE
    std::fprintf(stdout, "Deleting GoalRing static models...\n");
    #endif

    Entity::deleteModels(&GoalRing::modelsRing);
    Entity::deleteModels(&GoalRing::modelsText);
}
