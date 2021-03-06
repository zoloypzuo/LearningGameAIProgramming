/**
 * Copyright (c) 2013 David Young dayoung@goliathdesigns.com
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *  claim that you wrote the original software. If you use this software
 *  in a product, an acknowledgment in the product documentation would be
 *  appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be
 *  misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source
 *  distribution.
 */

#ifndef DEMO_FRAMEWORK_PHYSICS_DEBUG_DRAW_H
#define DEMO_FRAMEWORK_PHYSICS_DEBUG_DRAW_H

#include "bullet_linearmath/include/LinearMath/btIDebugDraw.h"

class PhysicsDebugDraw : public btIDebugDraw
{
public:
    PhysicsDebugDraw();

    virtual ~PhysicsDebugDraw();

    virtual void draw3dText(const btVector3& location, const char* textString);

    virtual void drawContactPoint(
        const btVector3& pointOnB,
        const btVector3& normalOnB,
        btScalar distance,
        int lifeTime,
        const btVector3& color);

    virtual void drawLine(
        const btVector3& from, const btVector3& to, const btVector3& color);

    virtual int getDebugMode() const;

    virtual void reportErrorWarning(const char* warningString);

    virtual void setDebugMode(int debugMode);

private:
    int debugMode_;

    PhysicsDebugDraw(const PhysicsDebugDraw&);

    PhysicsDebugDraw& operator=(const PhysicsDebugDraw&);
};

#endif  // DEMO_FRAMEWORK_PHYSICS_DEBUG_DRAW_H
