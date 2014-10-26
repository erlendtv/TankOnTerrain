#include "stdafx.h"
#include "DemoApp.h"
 
//-------------------------------------------------------------------------------------
DemoApp::DemoApp(void)
    :   mTerrainGlobals(0),
    mTerrainGroup(0),
    mTerrainsImported(false),
    mInfoLabel(0)
{
	mMove = 0;
	mBodyRotate = 0;
	mTurretRotate = 0;
	mBarrelRotate = 0;
	mBarrelPitch = 0;
	mHeightOffset = 18;
	cameraAttachedToNode = false;
	currentZoom = ZOOM1;
	tankBodyMoveFactor = 1.5;
	tankBodyRotFactor = 1;
	tankTurretRotFactor = 1;
	tankBarrelRotFactor = 1;
	tankCounter = 1;
}
//-------------------------------------------------------------------------------------
DemoApp::~DemoApp(void)
{
}
//-------------------------------------------------------------------------------------
void DemoApp::destroyScene(void)
{
    OGRE_DELETE mTerrainGroup;
    OGRE_DELETE mTerrainGlobals;
}
//-------------------------------------------------------------------------------------
void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
    img.load("terrainTest.png", Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
    if (flipX)
        img.flipAroundY();
    if (flipY)
        img.flipAroundX();
 
}
//-------------------------------------------------------------------------------------
void DemoApp::defineTerrain(long x, long y)
{
    Ogre::String filename = mTerrainGroup->generateFilename(x, y);
    if (Ogre::ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), filename))
    {
        mTerrainGroup->defineTerrain(x, y);
    }
    else
    {
        Ogre::Image img;
        getTerrainImage(x % 2 != 0, y % 2 != 0, img);
        mTerrainGroup->defineTerrain(x, y, &img);
        mTerrainsImported = true;
    }
}
//-------------------------------------------------------------------------------------
void DemoApp::initBlendMaps(Ogre::Terrain* terrain)
{
    Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
    Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
    Ogre::Real minHeight0 = 70;
    Ogre::Real fadeDist0 = 40;
    Ogre::Real minHeight1 = 70;
    Ogre::Real fadeDist1 = 15;
    float* pBlend0 = blendMap0->getBlendPointer();
    float* pBlend1 = blendMap1->getBlendPointer();
    for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
    {
        for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
        {
            Ogre::Real tx, ty;
 
            blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
            Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
            Ogre::Real val = (height - minHeight0) / fadeDist0;
            val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
            *pBlend0++ = val;
 
            val = (height - minHeight1) / fadeDist1;
            val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
            *pBlend1++ = val;
        }
    }
    blendMap0->dirty();
    blendMap1->dirty();
    blendMap0->update();
    blendMap1->update();
}
//-------------------------------------------------------------------------------------
void DemoApp::configureTerrainDefaults(Ogre::Light* light)
{
    // Configure global
    mTerrainGlobals->setMaxPixelError(8);
    // testing composite map
    mTerrainGlobals->setCompositeMapDistance(3000);
 
    // Important to set these so that the terrain knows what to use for derived (non-realtime) data
    mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
    mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
    mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());
 
    // Configure default import settings for if we use imported image
    Ogre::Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
    defaultimp.terrainSize = 513;
    defaultimp.worldSize = 12000.0f;
    defaultimp.inputScale = 600;
    defaultimp.minBatchSize = 33;
    defaultimp.maxBatchSize = 65;
    // textures
    defaultimp.layerList.resize(3);
    defaultimp.layerList[0].worldSize = 100;
    defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
    defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
    defaultimp.layerList[1].worldSize = 30;
    defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
    defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
    defaultimp.layerList[2].worldSize = 200;
    defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
    defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
}
//-------------------------------------------------------------------------------------
void DemoApp::createScene(void)
{
	// Init GODMODE camera holder
	mGodCameraHolder = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mGodCameraHolder->setPosition(Ogre::Vector3(1800,currentZoom, 1800));
	mGodCameraHolder->attachObject(mCamera);
	mCamera->lookAt(Ogre::Vector3(1800,0,1800));
    mCamera->setNearClipDistance(0.1);
    mCamera->setFarClipDistance(50000);
 
    if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_INFINITE_FAR_PLANE))
    {
        mCamera->setFarClipDistance(0);   // enable infinite far clip distance if we can
    }
 
// Play with startup Texture Filtering options
// Note: Pressing T on runtime will discarde those settings
//  Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
//  Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(7);
 
    Ogre::Vector3 lightdir(0.55, -0.3, 0.75);
    lightdir.normalise();
 
    Ogre::Light* light = mSceneMgr->createLight("tstLight");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(lightdir);
    light->setDiffuseColour(Ogre::ColourValue::White);
    light->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));
 
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2));
 
    mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
 
    mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mSceneMgr, Ogre::Terrain::ALIGN_X_Z, 513, 12000.0f);
    mTerrainGroup->setFilenameConvention(Ogre::String("DemoAppTerrain"), Ogre::String("dat"));
    mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);
 
    configureTerrainDefaults(light);
 
    for (long x = 0; x <= 0; ++x)
        for (long y = 0; y <= 0; ++y)
            defineTerrain(x, y);
 
    // sync load since we want everything in place when we start
    mTerrainGroup->loadAllTerrains(true);
 
    if (mTerrainsImported)
    {
        Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
        while(ti.hasMoreElements())
        {
            Ogre::Terrain* t = ti.getNext()->instance;
            initBlendMaps(t);
        }
    }
 
    mTerrainGroup->freeTemporaryResources();
 
    Ogre::ColourValue fadeColour(0.9, 0.9, 0.9);
    mSceneMgr->setFog(Ogre::FOG_LINEAR, fadeColour, 0.0, 10, 1200);
    mWindow->getViewport(0)->setBackgroundColour(fadeColour);
 
    Ogre::Plane plane;
    plane.d = 100;
    plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Y;
 
    //mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8, 500);
    mSceneMgr->setSkyPlane(true, plane, "Examples/CloudySky", 500, 20, true, 0.5, 150, 150);

	addNewTank(Ogre::Vector3(1800, 0, 1800));

	// Water
	Ogre::Entity *pWaterEntity;
	Ogre::Plane nWaterPlane;
 
	// create a water plane/scene node
	nWaterPlane.normal = Ogre::Vector3::UNIT_Y;
	nWaterPlane.d = -1.5;
	Ogre::MeshManager::getSingleton().createPlane(
		"WaterPlane",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		nWaterPlane,
		14000, 14000,
		20, 20,
		true, 1,
		10, 10,
		Ogre::Vector3::UNIT_Z);

	pWaterEntity = mSceneMgr->createEntity("water", "WaterPlane");
	pWaterEntity->setMaterialName("Examples/TextureEffect4");
	Ogre::SceneNode *waterNode =
	mSceneMgr->getRootSceneNode()->createChildSceneNode("WaterNode");
	waterNode->attachObject(pWaterEntity);
	waterNode->translate(-1000, 200, -1000);

	// TESTING PURPOSE
	// Create a manual object to show the normal vector
	mNormalLine = mSceneMgr->createManualObject("Normal Vector");
	mNormalLine->clear();
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(mNormalLine);
}
//-------------------------------------------------------------------------------------
void DemoApp::createFrameListener(void)
{
    BaseApplication::createFrameListener();
 
    mInfoLabel = mTrayMgr->createLabel(OgreBites::TL_TOP, "TInfo", "", 350);
}
//-------------------------------------------------------------------------------------
bool DemoApp::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    bool ret = BaseApplication::frameRenderingQueued(evt);
 
    if (mTerrainGroup->isDerivedDataUpdateInProgress())
    {
        mTrayMgr->moveWidgetToTray(mInfoLabel, OgreBites::TL_TOP, 0);
        mInfoLabel->show();
        if (mTerrainsImported)
        {
            mInfoLabel->setCaption("Building terrain, please wait...");
        }
        else
        {
            mInfoLabel->setCaption("Updating textures, patience...");
        }
    }
    else
    {
        mTrayMgr->removeWidgetFromTray(mInfoLabel);
        mInfoLabel->hide();
        if (mTerrainsImported)
        {
            mTerrainGroup->saveAllTerrains(true);
            mTerrainsImported = false;
        }
    }

	// Move and rotate the tank
	mTankBodyNode->translate(mMove, 0, 0, Ogre::Node::TransformSpace::TS_LOCAL);
	mTankBodyNode->yaw(Ogre::Degree(mBodyRotate));

	// Get tank's current position
	Ogre::Vector3 tankPosition = mTankBodyNode->getPosition();
	// Move it above the ground
	tankPosition.y = mTerrain->getHeightAtWorldPosition(tankPosition) + mHeightOffset;
	mTankBodyNode->setPosition(tankPosition);

	// Get current tank orientation
	Ogre::Quaternion tankOrientation = mTankBodyNode->getOrientation();

	// Get point on ground where the tank is
	tankPosition.y = mTerrain->getHeightAtWorldPosition(tankPosition);

	// Get a vector pointing in the local x direction
	Ogre::Vector3 v1 = tankPosition + tankOrientation.xAxis();
	v1.y = mTerrain->getHeightAtWorldPosition(v1);
	v1 -= tankPosition;

	// Get a vector pointing in the local -z direction
	Ogre::Vector3 v2 = tankPosition - tankOrientation.zAxis();
	v2.y = mTerrain->getHeightAtWorldPosition(v2);
	v2 -= tankPosition;
	
	// Find the normal vector
	Ogre::Vector3 normal = v1.crossProduct(v2);
	normal.normalise();

	// Rotate the tank turret
	mTankTurretNode->yaw(Ogre::Degree(mTurretRotate));

	// Calculate the tank barrel's current pitch
	mBarrelPitch += mBarrelRotate;

	// Clamp tank barrel rotation between 0 and 30 degrees
	if(mBarrelPitch > 30)
		mBarrelPitch = 30;
	else if(mBarrelPitch < 0)
		mBarrelPitch = 0;
	else
		mTankBarrelNode->roll(Ogre::Degree(-mBarrelRotate));


/*
	// Get the tank's facing direction
	Ogre::Vector3 facing = tankOrientation.xAxis();

	// Find the local -z direction
	Ogre::Vector3 right = facing.crossProduct(normal);
	right.normalise();

	// Find the local x direction
	Ogre::Vector3 forward = normal.crossProduct(right);
	forward.normalise();

	// Orientate the tank
	mTankNode->setOrientation(Ogre::Quaternion(forward, normal, right));
*/

//////////////////////////////////////////////////////////////////////////////////
	// Smoothened the change in orientation by a certain weightage each frame
	float weight = 0.0001;  // Weight of the new normal

	// Get current orientation and local y direction
	Ogre::Quaternion currentOrientation = mTankBodyNode->getOrientation();
	Ogre::Vector3 localY = currentOrientation.yAxis();

	// Compute a small amount to rotate based on weight
	Ogre::Vector3 newNormal = localY * ( 1 - weight ) + normal * weight;

	// Calculate the angle to rotate
	Ogre::Radian inclinationAngle = Ogre::Math::ACos(localY.dotProduct(newNormal));

	// If angle is not 0
	if(inclinationAngle.valueRadians() != 0.0f)
	{
		// Get rotation quaternion
		Ogre::Vector3 inclinationAxis = ( localY.crossProduct( newNormal) ).normalisedCopy();
		Ogre::Quaternion inclination = Ogre::Quaternion(inclinationAngle, inclinationAxis);

		// Orientate entity based on rotation quaternion
		mTankBodyNode->setOrientation( inclination * currentOrientation );
	}
//////////////////////////////////////////////////////////////////////////////////
	
	// CAMERA ATTACHED TO OBJECT?
	if(cameraAttachedToNode){
		Ogre::Vector3 point = mTankBodyNode->getPosition();
		mGodCameraHolder->setPosition(point.x + currentZoom, point.y + currentZoom, point.z + currentZoom);
		mCamera->lookAt(point);
	} else {
		// FIND HEIGHT FOR CAMERA AT NEW POSITION
		Ogre::Vector3 oldPos = mGodCameraHolder->getPosition();
		float heightAtNew = 0;
		
		if(mMouse->getMouseState().X.abs > (mWindow->getWidth() - 20)){
			heightAtNew = mTerrain->getHeightAtWorldPosition(oldPos.x + 10, 0, oldPos.z);
			mGodCameraHolder->setPosition(oldPos.x + 10,heightAtNew + currentZoom, oldPos.z);
		}
		else if(mMouse->getMouseState().X.abs < 20){
			heightAtNew = mTerrain->getHeightAtWorldPosition(oldPos.x - 10, 0, oldPos.z);
			mGodCameraHolder->setPosition(oldPos.x - 10, heightAtNew + currentZoom, oldPos.z);
		}
		else if(mMouse->getMouseState().Y.abs > (mWindow->getHeight() - 20)){
			heightAtNew = mTerrain->getHeightAtWorldPosition(oldPos.x, 0, oldPos.z + 10);
			mGodCameraHolder->setPosition(oldPos.x,heightAtNew + currentZoom, oldPos.z + 10);
		}
		else if(mMouse->getMouseState().Y.abs < 20){
			heightAtNew = mTerrain->getHeightAtWorldPosition(oldPos.x, 0, oldPos.z - 10);
			mGodCameraHolder->setPosition(oldPos.x, heightAtNew + currentZoom, oldPos.z - 10);
		} else {
			heightAtNew = mTerrain->getHeightAtWorldPosition(oldPos.x, 0, oldPos.z);
			mGodCameraHolder->setPosition(oldPos.x, heightAtNew + currentZoom, oldPos.z);
		}

	}
	
	return ret;
}

void DemoApp::selectTank(){
	Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(
	static_cast<float>(mMouse->getMouseState().X.abs)/mMouse->getMouseState().width, 
	static_cast<float>(mMouse->getMouseState().Y.abs)/mMouse->getMouseState().height);
	Ogre::RaySceneQuery * mRaySceneQuery = mSceneMgr->createRayQuery(mouseRay);
	// Set type of objects to query
	mRaySceneQuery->setQueryTypeMask(Ogre::SceneManager::ENTITY_TYPE_MASK);
	mRaySceneQuery->setSortByDistance(true);
	// Ray-cast and get first hit
	Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
	Ogre::RaySceneQueryResult::iterator itr = result.begin();
	// If hit a movable object
	if(itr != result.end() && itr->movable && itr->movable->getName() != "water" ){
		cameraAttachedToNode = true;
		if(mCamera->isAttached()){
			mCamera->detachFromParent();
		}
		selectedTank = mTanks.at(0);
		selectedTank.mCameraHolder->attachObject(mCamera);
		Ogre::Quaternion selectedOrientation = selectedTank.mTankTurretNode->getOrientation();
		Ogre::Vector3 localX = selectedTank.mTankBodyNode->getPosition() + selectedOrientation.xAxis();
		localX.y = mTerrain->getHeightAtWorldPosition(localX);
		localX -= selectedTank.mTankBodyNode->getPosition();
		Ogre::Vector3 nLocalZ = selectedTank.mTankBodyNode->getPosition() - selectedOrientation.zAxis();
		nLocalZ.y = mTerrain->getHeightAtWorldPosition(nLocalZ);
		nLocalZ -= selectedTank.mTankBodyNode->getPosition();
		Ogre::Vector3 normal = localX.crossProduct(nLocalZ);
		normal.normalise();
		// FOR TESTING
	// Update the normal vector's display
	mNormalLine->clear();
	// Specify the material and rendering type
	mNormalLine->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);

	mNormalLine->position(selectedTank.mTankBodyNode->getPosition());
	mNormalLine->colour(1, 0, 0);
	mNormalLine->position(selectedTank.mTankBodyNode->getPosition() + normal*50);
	mNormalLine->colour(1, 0, 0);

	// Finished defining line
	mNormalLine->end();

	}			
}

bool DemoApp::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ){
	switch(id){
	case OIS::MB_Left:
		selectTank();
		break;
	}
	return true;
}

 
// OIS::KeyListener
bool DemoApp::keyPressed( const OIS::KeyEvent &arg )
{
	BaseApplication::keyPressed(arg);

	selectedTank.keyPressed(arg);

    switch (arg.key)
	{ 
		case OIS::KC_ESCAPE: 
			mShutDown = true;
			break;

		default:
			break;
	}

	return true;
}


bool DemoApp::keyReleased( const OIS::KeyEvent &arg )
{
	BaseApplication::keyReleased(arg);

	selectedTank.keyRealesed(arg);

	switch (arg.key)
	{
		default:
			break;
	}

	return true;
}

bool DemoApp::mouseMoved( const OIS::MouseEvent &arg )
{
    if (mTrayMgr->injectMouseMove(arg)) return true;

	// zoom?
	if (mMouse->getMouseState().Z.rel < 0) {

		switch (currentZoom) {
			case ZOOM1:
				currentZoom = ZOOM2;
			break;
			case ZOOM2:
				currentZoom = ZOOM3;
			break;
			case ZOOM3:
				currentZoom = ZOOM4;
			break;
			case ZOOM4:
				currentZoom = ZOOM5;
			break;
			case ZOOM5:
				// do nothing
			break;
		}
	} else if (mMouse->getMouseState().Z.rel > 0) {
			switch (currentZoom) {
			case ZOOM1:
				// do nothing
			break;
			case ZOOM2:
				currentZoom = ZOOM1;
			break;
			case ZOOM3:
				currentZoom = ZOOM2;
			break;
			case ZOOM4:
				currentZoom = ZOOM3;
			break;
			case ZOOM5:
				currentZoom = ZOOM4;
				// do nothing
			break;
		}
	}

	return true;
}

bool DemoApp::addNewTank(const Ogre::Vector3 spawnPoint) {

	// Create tank body entity
	Ogre::Entity* tankBody = mSceneMgr->createEntity("chbody" + tankCounter, "chbody.mesh");
	tankBody->setCastShadows(true);
	tankBody->setMaterialName("ch_tank_material");

	// Create tank turret entity
	Ogre::Entity* tankTurret = mSceneMgr->createEntity("chturret" + tankCounter, "chturret.mesh");
	tankTurret->setCastShadows(true);
	tankTurret->setMaterialName("ch_tank_material");

	// Create tank barrel entity
	Ogre::Entity* tankBarrel = mSceneMgr->createEntity("chbarrel" + tankCounter, "chbarrel.mesh");
	tankBarrel->setCastShadows(true);
	tankBarrel->setMaterialName("ch_tank_material");

	// Create a child scene node and attach tank body to it
	mTankBodyNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mTankBodyNode->attachObject(tankBody);

	// Get the height of the terrain at a certain point
	mTerrain = mTerrainGroup->getTerrain(0, 0);
	float height = mTerrain->getHeightAtWorldPosition(spawnPoint.x, 0, spawnPoint.y);
	// Move it above the ground
	mTankBodyNode->translate(1800, height + mHeightOffset, 1800);

	// Create a child scene node from tank body's scene node and attach the tank turret to it
	mTankTurretNode = mTankBodyNode->createChildSceneNode();
	mTankTurretNode->attachObject(tankTurret);
	// Move it above tank body
	mTankTurretNode->translate(0, 3, 0);

	// Create a child scene node from tank turret's scene node and attach the tank barrel to it
	mTankBarrelNode = mTankTurretNode->createChildSceneNode();
	mTankBarrelNode->attachObject(tankBarrel);
	// Move it to the appropriate position on the turret
	mTankBarrelNode->translate(-30, 10, 0);

	Tank tank;
	tank.mTankBarrelNode = mTankBarrelNode;
	tank.mTankTurretNode = mTankTurretNode;
	tank.mTankBodyNode = mTankBodyNode;
	tank.mCameraHolder = tank.mTankTurretNode->createChildSceneNode();
	tank.mCameraHolder->translate(Ogre::Vector3(300,300,0));

	mTanks.push_back(tank);

	tankCounter++;

	return true;
}
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif
 
#ifdef __cplusplus
extern "C" {
#endif
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        DemoApp app;

        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }
 
        return 0;
    }
 
#ifdef __cplusplus
}
#endif