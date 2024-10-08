#ifndef OBJLOADER_H
#define OBJLOADER_H

class TexturedModel;
class CollisionModel;
class Vertex;
class QuadTreeNode;

#include <list>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdio.h>
#include "../textures/modeltexture.hpp"
#include "../toolbox/vector.hpp"

class ModelLoader
{
private:
    class Vertex
    {
    public:
        const int NO_INDEX = -1;

        Vector3f position;
        Vector4f color;
        int textureIndex = NO_INDEX;
        int normalIndex = NO_INDEX;
        Vertex* duplicateVertex = nullptr;
        int index = 0;
        float length = 0.0f;

    public:
        Vertex(int index, Vector3f* position, Vector4f* color);

        Vertex(int index, Vector3f* position);

        int getIndex();

        float getLength();

        int isSet();

        int hasSameTextureAndNormal(int textureIndexOther, int normalIndexOther);

        void setTextureIndex(int textureIndex);

        void setNormalIndex(int normalIndex);

        Vector3f* getPosition();

        int getTextureIndex();

        int getNormalIndex();

        Vertex* getDuplicateVertex();

        void setDuplicateVertex(Vertex* duplicateVertex);
    };

    class FakeTexture
    {
    public:
        std::string name;
        char type = 0;
        char sound = 0;
        char particle = 0;

        FakeTexture();
    };

    static void parseMtl(std::string filePath, std::string fileName, std::unordered_map<std::string, ModelTexture>* outMtlMap);

    static void deleteUnusedMtl(std::unordered_map<std::string, ModelTexture>* mtlMap, std::vector<ModelTexture>* usedMtls);

    static void processVertex(char** vertex,
        std::vector<Vertex*>* vertices,
        std::vector<int>* indices);

    static void processVertexBinary(int, int, int,
        std::vector<Vertex*>* vertices,
        std::vector<int>* indices);

    static void dealWithAlreadyProcessedVertex(Vertex*, 
        int, 
        int, 
        std::vector<int>*, 
        std::vector<Vertex*>*);

    static void removeUnusedVertices(std::vector<Vertex*>* vertices);

    static void convertDataToArrays(
        std::vector<Vertex*>* vertices, 
        std::vector<Vector2f>* textures,
        std::vector<Vector3f>* normals, 
        std::vector<float>* verticesArray, 
        std::vector<float>* texturesArray,
        std::vector<float>* normalsArray,
        std::vector<float>* colorsArray);

    static void workOnQuadTreeNode(FILE* file, QuadTreeNode* node);

    static inline void read(void* buf, int numBytesToRead, FILE* file);

public:
    //Attempts to load a mode as either an OBJ or binary format.
    //Checks for binary file first, then tries OBJ.
    //Each TexturedModel contained within 'models' must be deleted later.
    //Returns 0 if successful, 1 if model is already loaded, -1 if file couldn't be loaded
    static int loadModel(std::vector<TexturedModel*>* models, std::string filePath, std::string fileName);

private:
    //Each TexturedModel contained within 'models' must be deleted later.
    //Returns 0 if successful, 1 if model is already loaded, -1 if file couldn't be loaded
    static int loadObjModel(std::vector<TexturedModel*>* models, std::string filePath, std::string fileName);

    //Each TexturedModel contained within 'models' must be deleted later.
    //Returns 0 if successful, 1 if model is already loaded, -1 if file couldn't be loaded
    static int loadBinaryModel(std::vector<TexturedModel*>* models, std::string filePath, std::string fileName);

    //Each TexturedModel contained within 'models' must be deleted later.
    //Returns 0 if successful, 1 if model is already loaded, -1 if file couldn't be loaded
    static int loadVclModel(std::vector<TexturedModel*>* models, std::string filePath, std::string fileName);

public:
    //Each TexturedModel contained within 'models' must be deleted later.
    //Returns 0 if successful, 1 if model is already loaded, -1 if file couldn't be loaded
    static int loadObjModelWithMTL(std::vector<TexturedModel*>* models, std::string filePath, std::string fileNameOBJ, std::string fileNameMTL);

    //Each TexturedModel contained within 'models' must be deleted later.
    //Returns 0 if successful, 1 if model is already loaded, -1 if file couldn't be loaded
    static int loadBinaryModelWithMTL(std::vector<TexturedModel*>* models, std::string filePath, std::string fileNameBin, std::string fileNameMTL);

    //The CollisionModel returned must be deleted later.
    static CollisionModel* loadCollisionModel(std::string filePath, std::string fileName);

    //The CollisionModel returned must be deleted later.
    static CollisionModel* loadBinaryCollisionModel(std::string filePath, std::string fileName);

    //The CollisionModel returned must be deleted later.
    static CollisionModel* loadBinaryQuadTree(std::string filePath, std::string fileName);
};
#endif
