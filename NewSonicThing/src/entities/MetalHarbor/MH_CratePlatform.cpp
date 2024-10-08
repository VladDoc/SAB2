#include "mhcrateplatform.hpp"
#include "../entity.hpp"
#include "../camera.hpp"
#include "../../engineTester/main.hpp"
#include "../../models/models.hpp"
#include "../../loading/modelloader.hpp"
#include "../../toolbox/maths.hpp"
#include "../controllableplayer.hpp"
#include "../../collision/collisionmodel.hpp"
#include "../../collision/collisionchecker.hpp"
#include "../../audio/audioplayer.hpp"
#include "../../audio/source.hpp"

extern float dt;

std::vector<TexturedModel*> MH_CratePlatform::modelsCratePlatformLong;
std::vector<TexturedModel*> MH_CratePlatform::modelsCratePlatformCrate;
std::vector<TexturedModel*> MH_CratePlatform::modelsCratePlatformNoCrate;
CollisionModel* MH_CratePlatform::cmCratePlatformLong;
CollisionModel* MH_CratePlatform::cmCratePlatformCrate;
CollisionModel* MH_CratePlatform::cmCratePlatformNoCrate;

MH_CratePlatform::MH_CratePlatform() {}

MH_CratePlatform::MH_CratePlatform(float x, float y, float z, float dirX, float dirZ, float displacementMax, float speed, int type)
{
    position.x = x;
    position.y = y;
    position.z = z;
    rotX = 0;
    rotZ = 0;

    scale = 1;
    visible = true;

    platformMoves = true;
    this->speed = speed;
    this->displacementMax = displacementMax;
    displacementCurrent = 0;
    positionInitial = &position;

    shakeTimer = SHAKE_TIMER_MIN;
    stoppedTimer = 0;

    directionVector.set(dirX, 0.0f, dirZ);
    directionVector.normalize();

    rotY = Maths::toDegrees(atan2f(directionVector.x, directionVector.z));

    if (displacementMax == 0)
    {
        platformMoves = false;
    }

    stateCurrent = MOVING_FORWARD;

    updateTransformationMatrix();

    platformType = (MH_CratePlatform::types)type;
    //Load the correct collision model for the platform type
    switch (platformType)
    {
        case LONG:
            collideModelOriginal = MH_CratePlatform::cmCratePlatformLong;
            break;
        case CRATE:
            collideModelOriginal = MH_CratePlatform::cmCratePlatformCrate;
            break;
        case CRATELESS:
            collideModelOriginal = MH_CratePlatform::cmCratePlatformNoCrate;
            break;
    }

    collideModelTransformed = collideModelOriginal->duplicateMe();

    CollisionChecker::addCollideModel(collideModelTransformed);

    updateCollisionModel();
}

MH_CratePlatform::~MH_CratePlatform()
{
    if (platformAudioSource != nullptr)
    {
        platformAudioSource->stop();
        platformAudioSource = nullptr;
    }
}

void MH_CratePlatform::step() 
{
    if (platformMoves == true)
    {    
        Vector3f movementAmount;
        switch (stateCurrent)
        {
            case MOVING_FORWARD:
                movementAmount = calculateMovementAmount(directionVector);
                position = position + movementAmount;
                displacementCurrent += movementAmount.length();

                pushSonicAway(true, false);

                if (collideModelTransformed->playerIsOn)
                {
                    movePlayer(movementAmount);
                }
                
                if (displacementCurrent >= displacementMax)
                {
                    displacementCurrent = displacementMax;
                    
                    stateCurrent = STOPPED;
                    positionStopped = position;
                    stoppedTimer = 0;
                    shakeTimer = SHAKE_TIMER_MIN;
                }
                break;                
            case MOVING_BACKWARDS:
                movementAmount = calculateMovementAmount(Vector3f() - directionVector);
                position = position + movementAmount;
                displacementCurrent -= movementAmount.length();

                pushSonicAway(false, true);

                if (collideModelTransformed->playerIsOn)
                {
                    movePlayer(movementAmount);
                }

                if (displacementCurrent <= 0)
                {
                    displacementCurrent = 0;
                    position = positionInitial;
                    
                    stateCurrent = STOPPED;
                    positionStopped = position;
                    stoppedTimer = 0;
                    shakeTimer = SHAKE_TIMER_MIN;
                }
                break;
            case STOPPED:
                stoppedTimer += dt;

                movementAmount = shakePlatform();
                position = position + movementAmount;

                playStoppingSound();

                if (shakeTimer < SHAKE_TIMER_MAX && sinf(shakeTimer)/shakeTimer * 5 > 0)
                {
                    pushSonicAway(true, false);
                }
                else if (shakeTimer < SHAKE_TIMER_MAX && sinf(shakeTimer)/shakeTimer * 5 < 0)
                {
                    pushSonicAway(false, true);
                }

                if (collideModelTransformed->playerIsOn)
                {
                    movePlayer(movementAmount);
                }

                if (stoppedTimer > 2)
                {
                    resetStoppingSound();
                    if (displacementCurrent == displacementMax)
                    {
                        stateCurrent = MOVING_BACKWARDS;
                    }
                    else
                    {
                        stateCurrent = MOVING_FORWARD;
                    }
                }
                break;
        }

        updateTransformationMatrix();
        updateCollisionModel();

        Global::gameMainPlayer->animate();
        Global::gameMainPlayer->refreshCamera();
    }
}

std::vector<TexturedModel*>* MH_CratePlatform::getModels()
{
    //Load the correct model for the platform type
    switch (platformType)
    {
        case LONG:
            return &MH_CratePlatform::modelsCratePlatformLong;
            break;
        case CRATE:
            return &MH_CratePlatform::modelsCratePlatformCrate;
            break;
        case CRATELESS:
            return &MH_CratePlatform::modelsCratePlatformNoCrate;
            break;
    }

    return nullptr;
}

void MH_CratePlatform::loadStaticModels()
{
    if (MH_CratePlatform::modelsCratePlatformLong.size() > 0)
    {
        return;
    }

    #ifdef DEV_MODE
    printf("Loading MH_CratePlatform static models...\n");
    #endif

    ModelLoader::loadModel(&MH_CratePlatform::modelsCratePlatformLong, "res/Models/Objects/MetalHarbor/CratePlatform/", "CratePlatformLong");
    ModelLoader::loadModel(&MH_CratePlatform::modelsCratePlatformCrate, "res/Models/Objects/MetalHarbor/CratePlatform/", "CratePlatformCrate");
    ModelLoader::loadModel(&MH_CratePlatform::modelsCratePlatformNoCrate, "res/Models/Objects/MetalHarbor/CratePlatform/", "CratePlatformCrateless");

    if (MH_CratePlatform::cmCratePlatformLong == nullptr)
    {
        MH_CratePlatform::cmCratePlatformLong = ModelLoader::loadCollisionModel("Models/Objects/MetalHarbor/CratePlatform/", "CratePlatformLongCollision");
    }
    if (MH_CratePlatform::cmCratePlatformCrate == nullptr)
    {
        MH_CratePlatform::cmCratePlatformCrate = ModelLoader::loadCollisionModel("Models/Objects/MetalHarbor/CratePlatform/", "CratePlatformCrateCollision");
    }
    if (MH_CratePlatform::cmCratePlatformNoCrate == nullptr)
    {
        MH_CratePlatform::cmCratePlatformNoCrate = ModelLoader::loadCollisionModel("Models/Objects/MetalHarbor/CratePlatform/", "CratePlatformCratelessCollision");
    }
}

void MH_CratePlatform::deleteStaticModels()
{
    #ifdef DEV_MODE
    printf("Deleting MH_CratePlatform static models...\n");
    #endif

    Entity::deleteModels(&MH_CratePlatform::modelsCratePlatformLong);
    Entity::deleteModels(&MH_CratePlatform::modelsCratePlatformCrate);
    Entity::deleteModels(&MH_CratePlatform::modelsCratePlatformNoCrate);
    Entity::deleteCollisionModel(&MH_CratePlatform::cmCratePlatformLong);
    Entity::deleteCollisionModel(&MH_CratePlatform::cmCratePlatformCrate);
    Entity::deleteCollisionModel(&MH_CratePlatform::cmCratePlatformNoCrate);
}

inline Vector3f MH_CratePlatform::calculateMovementAmount(Vector3f directionVectorLocal)
{
    return directionVectorLocal.scaleCopy(speed * dt);
}

inline void MH_CratePlatform::movePlayer(Vector3f movementAmount)
{
    Vector3f newPlayerPos = movementAmount + Global::gameMainPlayer->getPosition();
    Global::gameMainPlayer->position = newPlayerPos;
}

inline Vector3f MH_CratePlatform::shakePlatform()
{
    Vector3f distanceFromPositionStopped = positionStopped - position;

    if (shakeTimer > SHAKE_TIMER_MAX)
    {
        return distanceFromPositionStopped;
    }
    shakeTimer += dt * 30;
    return directionVector.scaleCopy(sinf(shakeTimer)/shakeTimer * 5) + distanceFromPositionStopped;
}

inline void MH_CratePlatform::pushSonicAway(bool /*frontHitboxes*/, bool /*backHitboxes*/)
{
    //first check if sonic is even close to the platform
    //if (collisionCheckCylinder(position, 100, 100))
    {
        //Vector3f rotateAround = Vector3f(0, 1, 0);
        //Vector3f sidewaysVector = Maths::rotatePoint(&directionVector, &rotateAround, Maths::toRadians(90));
        //Vector3f collisionCenterPos1;
        //Vector3f collisionCenterPos2;
        //Vector3f collisionCenterPos3;
        //Vector3f collisionCenterPos4;

        //collisions on front of platform
        //collisionCenterPos1 = position + directionVector.scaleCopy(COLLISION_POSITION_FORWARD) + sidewaysVector.scaleCopy(COLLISION_POSITION_SIdEWAYS);
        //collisionCenterPos2 = position + directionVector.scaleCopy(COLLISION_POSITION_FORWARD);
        //collisionCenterPos3 = position + directionVector.scaleCopy(COLLISION_POSITION_FORWARD) + sidewaysVector.scaleCopy(COLLISION_POSITION_SIdEWAYS * -1);
        //if (collisionCheckCylinder(collisionCenterPos1, COLLISION_RADIUS, COLLISION_HEIGHT) || collisionCheckCylinder(collisionCenterPos2, COLLISION_RADIUS, COLLISION_HEIGHT) || collisionCheckCylinder(collisionCenterPos3, COLLISION_RADIUS, COLLISION_HEIGHT))
        {
            //Global::gameMainPlayer->increasePosition(directionVector.x * speed/30, directionVector.y * speed/30, directionVector.z * speed/30);
        }

        //collisions on back of platform
        //collisionCenterPos1 = position + directionVector.scaleCopy(COLLISION_POSITION_BACKWARD_OUTER) + sidewaysVector.scaleCopy(COLLISION_POSITION_SIdEWAYS);
        //collisionCenterPos2 = position + directionVector.scaleCopy(COLLISION_POSITION_BACKWARD_INNER) + sidewaysVector.scaleCopy(COLLISION_POSITION_BACKSIdEWAYS_INNER);
        //collisionCenterPos3 = position + directionVector.scaleCopy(COLLISION_POSITION_BACKWARD_INNER) + sidewaysVector.scaleCopy(COLLISION_POSITION_BACKSIdEWAYS_INNER * -1);
        //collisionCenterPos4 = position + directionVector.scaleCopy(COLLISION_POSITION_BACKWARD_OUTER) + sidewaysVector.scaleCopy(COLLISION_POSITION_SIdEWAYS * -1);
        //if (collisionCheckCylinder(collisionCenterPos1, COLLISION_RADIUS, COLLISION_HEIGHT) || collisionCheckCylinder(collisionCenterPos2, COLLISION_RADIUS, COLLISION_HEIGHT) || collisionCheckCylinder(collisionCenterPos3, COLLISION_RADIUS, COLLISION_HEIGHT) || collisionCheckCylinder(collisionCenterPos4, COLLISION_RADIUS, COLLISION_HEIGHT))
        {
            //Global::gameMainPlayer->increasePosition(directionVector.x * -speed/30, directionVector.y * -speed/30, directionVector.z * -speed/30);
        }
    }    
}

inline bool MH_CratePlatform::collisionCheckCylinder(Vector3f collisionCenterPos, float hitboxRadius, float hitboxHeight)
{
    Vector3f playerPos = Global::gameMainPlayer->position;
    Vector3f playerToCenterDistance = playerPos - collisionCenterPos;
    float playerToCenterDistanceSquared = playerToCenterDistance.x * playerToCenterDistance.x + playerToCenterDistance.z * playerToCenterDistance.z;
    if (playerToCenterDistanceSquared <= hitboxRadius * hitboxRadius && fabs(playerToCenterDistance.y) < hitboxHeight)
    {
        return true;
    }
    return false;
}

inline void MH_CratePlatform::playStoppingSound()
{
    if (platformAudioSource == nullptr)
    {
        platformAudioSource = AudioPlayer::play(63, &position, 1, false);
    }
}

inline void MH_CratePlatform::resetStoppingSound()
{
    platformAudioSource = nullptr;
}

