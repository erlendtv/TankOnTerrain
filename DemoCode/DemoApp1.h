#ifndef __DemoApp_h_
#define __DemoApp_h_
 
#include "BaseApplication.h"

#include <fstream>
#include <string>

class DemoApp : public BaseApplication
{
public:
    DemoApp(void);
    virtual ~DemoApp(void);
 
protected:
	virtual bool setup();
    virtual void createScene(void);

	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
    virtual bool keyPressed( const OIS::KeyEvent &arg );
    virtual bool keyReleased( const OIS::KeyEvent &arg );

private:
	// Scene nodes for the tank
	Ogre::SceneNode* mTankNode;

	// For tank movement and rotation
	float mMove;
	float mBodyRotate;
};
 
#endif // #ifndef __DemoApp_h_