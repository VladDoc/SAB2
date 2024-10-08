#ifndef RH_BLIMP
#define RH_BLIMP

#include <list>
#include "../entity.hpp"

class TexturedModel;
class Dummy;

class RH_Blimp : public Entity
{
private:
    static std::vector<TexturedModel*> modelsBlimp;
    static std::vector<TexturedModel*> modelsSpotlight;

    Dummy* spotlight = nullptr;

    Vector3f spotlightRotAxis;
    Vector3f spotlightDirectionTarget;
    Vector3f spotlightDirectionCurrent;

    float timer = 0.0f;

public:
    RH_Blimp();

    RH_Blimp(float x, float y, float z, float newRotY);

    void step();

    std::vector<TexturedModel*>* getModels();

    static void loadStaticModels();

    static void deleteStaticModels();
};
#endif
