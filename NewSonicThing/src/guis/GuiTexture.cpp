#include "guitexture.hpp"

#include <glad/glad.h>
#include "../toolbox/vector.hpp"

GuiTexture::GuiTexture()
{
    
}

GuiTexture::GuiTexture(GLuint textureId, Vector2f* position, Vector2f* size, float rotation)
{
    this->textureId = textureId;
    this->position.x = (position->x*2.0f)-1;
    this->position.y = -((position->y*2.0f)-1);
    this->sizeRaw.set(size);
    this->sizeScaled.set(size);
    this->visible = true;
    this->scale = 1;
    this->rotation = rotation;
}

GuiTexture::GuiTexture(GLuint textureId, float posX, float posY, float sizeX, float sizeY, float rotation)
{
    this->textureId = textureId;
    this->position.x = (posX*2.0f)-1;
    this->position.y = -((posY*2.0f)-1);
    this->sizeRaw.x = sizeX;
    this->sizeRaw.y = sizeY;
    this->sizeScaled.x = sizeX;
    this->sizeScaled.y = sizeY;
    this->visible = true;
    this->scale = 1;
    this->rotation = rotation;
}

GLuint GuiTexture::getTexture()
{
    return textureId;
}

Vector2f* GuiTexture::getPosition()
{
    return &position;
}

Vector2f* GuiTexture::getSizeRaw()
{
    return &sizeRaw;
}

void GuiTexture::setTexture(GLuint newTextureId)
{
    textureId = newTextureId;
}

void GuiTexture::setX(float newX)
{
    position.x = (newX*2.0f)-1;
}

void GuiTexture::setY(float newY)
{
    position.y = -((newY*2.0f)-1);
}

bool GuiTexture::getVisible()
{
    return visible;
}

void GuiTexture::setVisible(bool newVisible)
{
    visible = newVisible;
}

float GuiTexture::getScale()
{
    return scale;
}

void GuiTexture::setScale(float newScale)
{
    scale = newScale;
    sizeScaled.x = sizeRaw.x*scale;
    sizeScaled.y = sizeRaw.y*scale;
}

Vector2f* GuiTexture::getSizeScaled()
{
    return &sizeScaled;
}

void GuiTexture::setSizeScaled(float scaleX, float scaleY)
{
    sizeScaled.x = scaleX;
    sizeScaled.y = scaleY;
}

float GuiTexture::getRotation()
{
    return rotation;
}

void GuiTexture::setRotation(float newRotation)
{
    rotation = newRotation;
}
