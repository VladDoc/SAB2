#ifndef STAGE_H
#define STAGE_H

class TexturedModel;

#include <list>
#include <vector>
#include "entity.hpp"
#include "dummy.hpp"
#include "../toolbox/vector.hpp"
#include <string>

class Stage : public Entity
{
private:
    static std::vector<std::vector<TexturedModel*>> chunkModels;
    static std::vector<Dummy*> chunkDummies;
    static std::vector<std::vector<Vector3f>> chunkMin;
    static std::vector<std::vector<Vector3f>> chunkMax;

public:
    Vector3f finishPlayerPosition;
    Vector3f finishPlayerDir;
    Vector3f finishPlayerUp;
    Vector3f finishCameraDir;

    Stage();

    void step();

    std::vector<TexturedModel*>* getModels();

    static void loadModels(
        const char* folder, 
        std::vector<std::string>* fnames, 
        std::vector<std::vector<Vector3f>>* mins,  
        std::vector<std::vector<Vector3f>>* maxs);

    static void respawnChunks();

    static void updateVisibleChunks();

    static void deleteModels();

    std::string getName();
};
#endif
