/*
 * glInfo.h
 *
 *  Created on: Nov 10, 2016
 *      Author: hoover
 */

#ifndef GLINFO_H_
#define GLINFO_H_


#include <string>
#include <vector>

// struct variable to store OpenGL info
struct glInfo
{
    std::string vendor;
    std::string renderer;
    std::string version;
    std::string glslVersion;
    std::vector <std::string> extensions;
    int redBits;
    int greenBits;
    int blueBits;
    int alphaBits;
    int depthBits;
    int stencilBits;
    int maxTextureSize;
    int maxLights;
    int maxAttribStacks;
    int maxModelViewStacks;
    int maxProjectionStacks;
    int maxClipPlanes;
    int maxTextureStacks;

    // ctor, init all members
    glInfo() : redBits(0), greenBits(0), blueBits(0), alphaBits(0), depthBits(0),
               stencilBits(0), maxTextureSize(0), maxLights(0), maxAttribStacks(0),
               maxModelViewStacks(0), maxClipPlanes(0), maxTextureStacks(0) {}

    void getInfo(unsigned int param=0);         // extract info
    void printSelf();                           // print itself
    bool isExtensionSupported(const std::string& ext); // check if a extension is supported
};


#endif /* GLINFO_H_ */
