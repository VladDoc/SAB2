#include <list>

#include "tcdash.hpp"

#include "../entity.hpp"
#include "../../models/models.hpp"
#include "../../toolbox/vector.hpp"
#include "../../loading/modelloader.hpp"
#include "../../engineTester/main.hpp"
#include "../../toolbox/maths.hpp"
#include "../controllableplayer.hpp"
#include "tckart.hpp"

std::vector<TexturedModel*> TC_Dash::models;

TC_Dash::TC_Dash()
{

}

TC_Dash::TC_Dash(
    float x, float y, float z,
    float atX, float atY, float atZ,
    float normX, float normY, float normZ)
{
    position.set(x, y, z);

    Vector3f currNorm(normX, normY, normZ);
    Vector3f groundSpeeds = Maths::calculatePlaneSpeed(atX, atY, atZ, &currNorm);
    float twistAngle = Maths::toDegrees(atan2f(-groundSpeeds.z, groundSpeeds.x));
    float nX = currNorm.x;
    float nY = currNorm.y;
    float nZ = currNorm.z;
    float normHLength = sqrtf(nX*nX + nZ*nZ);
    float pitchAngle = Maths::toDegrees(atan2f(nY, normHLength));
    float yawAngle = Maths::toDegrees(atan2f(-nZ, nX));
    float diff = Maths::compareTwoAngles(twistAngle, yawAngle);

    rotX = diff;
    rotY = yawAngle;
    rotZ = pitchAngle;
    rotRoll = 0;
    updateTransformationMatrix();
}

void TC_Dash::step()
{
    if (abs(position.x - Global::gameMainPlayer->position.x) < 50 && 
        abs(position.z - Global::gameMainPlayer->position.z) < 50 &&
        abs(position.y - Global::gameMainPlayer->position.y) < 50)
    {
        if (Global::gameKart != nullptr)
        {
            float distToPlayer = (Global::gameKart->position - position).lengthSquared();

            if (distToPlayer < 20*20)
            {
                if (!playerIsIn)
                {
                    Global::gameKart->giveMeABoost();
                }
                playerIsIn = true;
            }
            else
            {
                playerIsIn = false;
            }
        }
    }
    else
    {
        playerIsIn = false;
    }
}

std::vector<TexturedModel*>* TC_Dash::getModels()
{
    return &TC_Dash::models;
}

void TC_Dash::loadStaticModels()
{
    if (TC_Dash::models.size() > 0)
    {
        return;
    }

    #ifdef DEV_MODE
    printf("Loading TC_Dash static models...\n");
    #endif

    ModelLoader::loadModel(&TC_Dash::models, "res/Models/Levels/TwinkleCircuit/Objects/Dash/", "Dash");
}

void TC_Dash::deleteStaticModels()
{
    #ifdef DEV_MODE
    printf("Deleting TC_Dash static models...\n");
    #endif

    Entity::deleteModels(&TC_Dash::models);
}
