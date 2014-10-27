#ifndef __UTILITIES_H_
#define __UTILITIES_H_

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include "stdafx.h"

inline Ogre::Vector3 convert(const btVector3& vec3)
{
    return Ogre::Vector3(vec3.x(), vec3.y(), vec3.z());
}

inline btVector3 convert(const Ogre::Vector3& vec3)
{
    return btVector3(vec3.x, vec3.y, vec3.z);
}

inline btQuaternion convert(const Ogre::Quaternion &Q)
{
    return btQuaternion(Q.x, Q.y, Q.z, Q.w);
}
 
inline Ogre::Quaternion convert(const btQuaternion &Q)
{
    return Ogre::Quaternion(Q.w(), Q.x(), Q.y(), Q.z());
}

class MyMotionState : public btMotionState {
public:
    MyMotionState(const btTransform &initialpos, Ogre::SceneNode *node) {
        mVisibleobj = node;
        mPos1 = initialpos;
    }
 
    virtual ~MyMotionState() {
    }
 
    void setNode(Ogre::SceneNode *node) {
        mVisibleobj = node;
    }
 
    virtual void getWorldTransform(btTransform &worldTrans) const {
        worldTrans = mPos1;
    }
 
    virtual void setWorldTransform(const btTransform &worldTrans) {
        if(NULL == mVisibleobj)
            return; // silently return before we set a node
        btQuaternion rot = worldTrans.getRotation();
        mVisibleobj->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
        btVector3 pos = worldTrans.getOrigin();
        mVisibleobj->setPosition(pos.x(), pos.y(), pos.z());
    }
 
protected:
    Ogre::SceneNode *mVisibleobj;
    btTransform mPos1;
};

#endif