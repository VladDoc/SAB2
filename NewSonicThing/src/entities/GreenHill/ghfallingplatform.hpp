#ifndef GHFALLINGPLATFORM_H
#define GHFALLINGPLATFORM_H

class TexturedModel;
class CollisionModel;

#include <list>
#include "../entity.hpp"
#include "../collideableobject.hpp"

class GH_FallingPlatform : public CollideableObject
{
private:
    static std::vector<TexturedModel*> models;
    static CollisionModel* cmOriginal;
    float timer = 0;

public:
    GH_FallingPlatform();
    GH_FallingPlatform(float x, float y, float z, float rotY, float scale);

    void step();

    std::vector<TexturedModel*>* getModels();

    static void loadStaticModels();

    static void deleteStaticModels();
};
#endif
