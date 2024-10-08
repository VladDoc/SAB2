#include <glad/glad.h>

#include "entity.hpp"
#include "../models/models.hpp"
#include "../toolbox/vector.hpp"
#include "bomb.hpp"
#include "../loading/modelloader.hpp"
#include "../engineTester/main.hpp"
#include "../entities/camera.hpp"
#include "../entities/playersonic.hpp"
#include "../collision/collisionchecker.hpp"
#include "../collision/triangle3d.hpp"
#include "../toolbox/maths.hpp"
#include "../audio/audioplayer.hpp"
#include "../particles/particle.hpp"
#include "../particles/particleresources.hpp"
#include "../particles/particlemaster.hpp"

#include <list>
#include <iostream>

std::vector<TexturedModel*> Bomb::models;

Bomb::Bomb()
{

}

Bomb::Bomb(float x, float y, float z, float xVel, float yVel, float zVel)
{
    position.x = x;
    position.y = y;
    position.z = z;
    moves = true;
    vel.set(xVel, yVel, zVel);
    timer = 0.0f;
    visible = true;
    updateTransformationMatrix();
}

void Bomb::step()
{
    extern float dt;

    timer += dt;

    if (moves)
    {
        vel.y -= 0.1f*60*60*dt; //gravity
        vel.scale(1 - (0.01f*60*dt)); //air friction

        if (CollisionChecker::checkCollision(
            position.x, position.y - 2.7f, position.z,
            position.x + vel.x*dt, position.y + vel.y*dt - 2.7f, position.z + vel.z*dt))
        {
            position.set(CollisionChecker::getCollidePosition());
            position.y += 2.7f;

            Vector3f normal = CollisionChecker::getCollideTriangle()->normal;
            float speed = vel.length();

            if (speed > 0.5f*60.0f)
            {
                Vector3f bounceVel = Maths::bounceVector(&vel, &normal, 0.7f);
                vel = bounceVel;
                increasePosition(normal.x*0.01f, normal.y*0.01f, normal.z*0.01f);
            }
            else
            {
                moves = false;
            }
        }
        else
        {
            position = position + vel.scaleCopy(dt);
        }
    }

    updateTransformationMatrix();

    //flashing and dissapear
    if (timer > 3.0f)
    {
        visible = (fmodf(timer, 0.3f) > 0.15f);
    
        if (timer > 4.0f)
        {
            AudioPlayer::play(73, &position);
            Global::deleteEntity(this);

            const float bombRange = 40.0f;

            if ((Global::gameMainPlayer->getCenterPosition() - position).lengthSquared() < bombRange*bombRange)
            {
                Global::gameMainPlayer->takeDamage(&position);
            }

            extern std::unordered_set<Entity*> gameEntities;
            for (auto e : gameEntities)
            {
                if (e->isEnemy())
                {
                    Vector3f diff = e->position - position;
                    if (diff.lengthSquared() < bombRange*bombRange)
                    {
                        e->die();
                    }
                }
            }

            std::vector<std::unordered_set<Entity*>*> nearbyChunkedEntities;
            Global::getNearbyEntities(position.x, position.z, &nearbyChunkedEntities, bombRange);
            for (auto set : nearbyChunkedEntities)
            {
                for (auto e : (*set))
                {
                    if (e->isEnemy())
                    {
                        Vector3f diff = e->position - position;
                        if (diff.lengthSquared() < bombRange*bombRange)
                        {
                            e->die();
                        }
                    }
                }
            }

            float height = 0.2f;
            float spread = 7.0f;

            Vector3f velZero(0, 0, 0);
            Vector3f toCamDiff = Global::gameCamera->eye - position;
            toCamDiff.setLength(20);
    
            for (int i = 7; i != 0; i--)
            {
                Vector3f pos(
                    position.x + spread*(Maths::random() - 0.5f),
                    position.y + spread*(Maths::random() - 0.5f) + height,
                    position.z + spread*(Maths::random() - 0.5f));
                pos = pos + toCamDiff; //so that these aren't behind the big explosion

                ParticleMaster::createParticle(ParticleResources::textureExplosion1, &pos, &velZero, 0, 0.75f, 3*Maths::random() + 6, 0, false, false, 0.5f, true);
            }
    
            Vector3f pos(
                position.x,
                position.y + height,
                position.z);
    
            ParticleMaster::createParticle(ParticleResources::textureExplosion2, &pos, &velZero, 0, 0.916f, 12, 0, false, false, 0.75f, true);
            return;
        }
    }

    //pick up
    //if (std::abs(position.y - Global::gameMainPlayer->position.y) < 40 &&
    //    std::abs(position.z - Global::gameMainPlayer->position.z) < 40 &&
    //    std::abs(position.x - Global::gameMainPlayer->position.x) < 40 &&
    //    (ageTimer > 1.0f || ageTimer < 0.0f))
    //{
    //    if ((Global::gameMainPlayer->getCenterPosition() - position).lengthSquared() < (5.0f*5.0f + 9.0f*9.0f))
    //    {
    //        AudioPlayer::play(4, &position);
    //
    //        for (int i = 0; i < 10; i++)
    //        {
    //            Vector3f pos(
    //                position.x + Maths::random() * 8 - 4,
    //                position.y + Maths::random() * 8 - 4,
    //                position.z + Maths::random() * 8 - 4);
    //        
    //            Vector3f partVel(0, 0.4f*60, 0);
    //        
    //            ParticleMaster::createParticle(ParticleResources::textureSparkleYellow, &pos, &partVel,
    //                0.025f*60*60, 0.5f, 0, 4, -(4.0f / 0.5f), false, false, 0.5f, true);
    //        }
    //
    //        Global::increaseRingCount(1);
    //
    //        if (ageTimer < 0.0f)
    //        {
    //            Global::gameScore += 10;
    //        }
    //
    //        Global::deleteEntity(this);
    //        return;
    //    }
    //}
}

std::vector<TexturedModel*>* Bomb::getModels()
{
    return &Bomb::models;
}

void Bomb::loadStaticModels()
{
    if (Bomb::models.size() > 0)
    {
        return;
    }

    #ifdef DEV_MODE
    printf("Loading Bomb static models...\n");
    #endif

    ModelLoader::loadModel(&Bomb::models,    "res/Models/Objects/Bomb/", "Bomb2");
}

void Bomb::deleteStaticModels()
{
    #ifdef DEV_MODE
    printf("Deleting Bomb static models...\n");
    #endif

    Entity::deleteModels(&Bomb::models);
}
