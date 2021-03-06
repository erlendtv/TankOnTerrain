#include "stdafx.h"
#include "DemoApp.h"

#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <string>
 
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
	currentZoom = ZOOM3;
	tankBodyMoveFactor = 1.5;
	tankBodyRotFactor = 1;
	tankTurretRotFactor = 1;
	tankBarrelRotFactor = 1;
	tankCounter = 1;
	isTankSelected = false;
	insertBtnIsDown = false;
	mExplosionCount = 0;

	mPhysicsEngine = new PhysicsEngine();
	mPhysicsEngine->initPhysics();

	mBoxCount = 0;
	projectiles.reserve(10);
	mTanks.reserve(100);
}
//-------------------------------------------------------------------------------------
DemoApp::~DemoApp(void)
{
	if(mPhysicsEngine){
		delete mPhysicsEngine;
	}
	mSceneMgr->destroyQuery(mRaySceneQuery);
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
 
    Ogre::Vector3 lightdir(0.7, 0.1, 0.3);
    lightdir.normalise();
 
    Ogre::Light* light = mSceneMgr->createLight("tstLight");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(lightdir);
    light->setDiffuseColour(Ogre::ColourValue::White);
    light->setSpecularColour(Ogre::ColourValue(0.1, 0.1, 0.1));
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.4, 0.4, 0.4));
 
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

	// Add physics to terrain
	Ogre::Terrain* terrain = mTerrainGroup->getTerrain(0,0);
	mPhysicsEngine->createTerrainData(terrain->getHeightData(), terrain->getSize(), terrain->getWorldSize()/terrain->getSize());
 
    Ogre::ColourValue fadeColour(0.9, 0.9, 0.9);
    mSceneMgr->setFog(Ogre::FOG_LINEAR, fadeColour,0.0, 10, 5000);
    mWindow->getViewport(0)->setBackgroundColour(fadeColour);
 
    Ogre::Plane plane;
    plane.d = 100;
    plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Y;
 
    //mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8, 500);
    mSceneMgr->setSkyPlane(true, plane, "Examples/CloudySky", 500, 20, true, 0.5, 150, 150);

	addNewTank(Ogre::Vector3(1800, 0, 1800));
	addNewTank(Ogre::Vector3(2000, 0, 2000));
	addNewTank(Ogre::Vector3(2300, 0, 1500));
	addNewTank(Ogre::Vector3(0, 0, 0));
	addNewTank(Ogre::Vector3(-1000, 0, -1000));

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
		12000, 12000,
		20, 20,
		true, 1,
		10, 10,
		Ogre::Vector3::UNIT_Z);

	pWaterEntity = mSceneMgr->createEntity("water", "WaterPlane");
	pWaterEntity->setMaterialName("Examples/TextureEffect2");
	Ogre::SceneNode *waterNode =
	mSceneMgr->getRootSceneNode()->createChildSceneNode("WaterNode");
	waterNode->attachObject(pWaterEntity);
	waterNode->translate(0, 200, 0);

	// Create some obstacles
	createWorldObstacles();

}

//-------------------------------------------------------------------------------------
void DemoApp::createFrameListener(void)
{
    BaseApplication::createFrameListener();
 
    mInfoLabel = mTrayMgr->createLabel(OgreBites::TL_TOP, "TInfo", "", 350);

	mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());
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

	for(std::vector<Tank>::iterator it = mTanks.begin(); it != mTanks.end(); ++it) {

		it->frameRenderingQueued(evt);

	}

	/* add time to projectiles */
	for(std::vector<Projectile*>::iterator it = projectiles.begin(); it != projectiles.end(); ++it) {
		(*it)->Lived += evt.timeSinceLastFrame;
	}
	
	// CAMERA ATTACHED TO OBJECT?
	if(cameraAttachedToNode && isTankSelected){
		Ogre::Vector3 point = selectedTank->mTankBodyNode->getPosition();
		mGodCameraHolder->setPosition(point.x, point.y + currentZoom, point.z);
		point.y = point.y + 100;
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

	mPhysicsEngine->update(evt.timeSinceLastFrame);

	checkProjectileCollision();

	updateDetailsPanel(selectedTankForGUI);

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
	if(itr != result.end() && itr->movable && itr->movable->getName() != "water" && itr->movable->getName().find("house") == std::string::npos){
		string tankName = "chbo";
		string barrelName = "chba";
		string turretName = "chtu";
		for(std::vector<Tank>::iterator it = mTanks.begin(); it != mTanks.end(); ++it) {
			if (itr->movable->getName().compare(tankName + to_string(it->getId())) == 0) {
				selectedTank = &*it;
				selectedTankForGUI = (&*it);
				break;
			} else if (itr->movable->getName().compare(barrelName + to_string(it->getId())) == 0) {
				selectedTank = &*it;
				selectedTankForGUI = (&*it);
				break;
			} else if (itr->movable->getName().compare(turretName + to_string(it->getId())) == 0) {
				selectedTank = &*it;
				selectedTankForGUI = (&*it);
				break;
			}
		}
		cameraAttachedToNode = true;
		if(mCamera->isAttached()){
			mCamera->detachFromParent();
		}
		selectedTank->mCameraHolder->attachObject(mCamera);
		selectedTank->setTankStateToAI(false);

		// Remove mouse cursor
		mTrayMgr->hideCursor();

		isTankSelected = true;
	}			
}

bool DemoApp::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ){
	switch(id){
	case OIS::MB_Left:
		selectTank();
		if (!isTankSelected && insertBtnIsDown) {
			if(tankCounter < 100){
			Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(
					static_cast<float>(mMouse->getMouseState().X.abs)/mMouse->getMouseState().width, 
					static_cast<float>(mMouse->getMouseState().Y.abs)/mMouse->getMouseState().height);				
			Ogre::TerrainGroup::RayResult res = mTerrainGroup->rayIntersects(mouseRay);
			addNewTank(res.position);
			}
		}
		break;
	case OIS::MB_Right:
		if(!cameraAttachedToNode){
			getTankInfoForGUI();
		}
		return true;
	}
}
void DemoApp::updateDetailsPanel(Tank* tank){
	mDetailsPanel->setParamValue(0,Ogre::StringConverter::toString(tank->getId()));
	mDetailsPanel->setParamValue(1,Ogre::StringConverter::toString(tank->getHp()));
	Ogre::String state;
	if(tank->tank_state == 2){
		if(tank->ai_state == 1){state="ROAMING";}
			else {state="ATTACKING";}
	} else {state = "USER CONTROL";}
	mDetailsPanel->setParamValue(2,state);
	mDetailsPanel->setParamValue(3,Ogre::StringConverter::toString(tank->getKills()));
	mDetailsPanel->setParamValue(4,Ogre::StringConverter::toString(tank->getDeaths()));
	mDetailsPanel->setParamValue(5,"CURRENT");
	if(!mDetailsPanel->isVisible()){
		mDetailsPanel->show();
	}
		
}

bool DemoApp::keyPressed( const OIS::KeyEvent &arg )
{
	BaseApplication::keyPressed(arg);

	if (selectedTank != nullptr && isTankSelected)
		selectedTank->keyPressed(arg);

    switch (arg.key)
	{ 
		case OIS::KC_1:
			if (mCamera->isAttached()) {
				mCamera->detachFromParent();
			}
			mGodCameraHolder->attachObject(mCamera);
			mCamera->setOrientation(mGodCameraHolder->getOrientation());
			mCamera->lookAt(selectedTank->mTankBodyNode->getPosition());

			isTankSelected = false;
			cameraAttachedToNode = false;
			selectedTank->setTankStateToAI(true);
			selectedTank = nullptr;

			// show cursor
			mTrayMgr->showCursor();
			break;

		case OIS::KC_I:
			insertBtnIsDown = true;
			break;
		case OIS::KC_ESCAPE: 
			mShutDown = true;
			break;
		case OIS::KC_H:
			break;
	}
	return true;
}


bool DemoApp::keyReleased( const OIS::KeyEvent &arg )
{
	BaseApplication::keyReleased(arg);

	if (selectedTank != nullptr && isTankSelected)
		selectedTank->keyRealesed(arg);

	switch (arg.key)
	{
		case OIS::KC_INSERT:
			insertBtnIsDown = false;
			break;
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
	Ogre::Entity* tankBody = mSceneMgr->createEntity("chbo" + to_string(tankCounter), "chbody.mesh");
	tankBody->setCastShadows(true);
	tankBody->setMaterialName("ch_tank_material");

	// Create tank turret entity
	Ogre::Entity* tankTurret = mSceneMgr->createEntity("chtu" + to_string(tankCounter), "chturret.mesh");
	tankTurret->setCastShadows(true);
	tankTurret->setMaterialName("ch_tank_material");

	// Create tank barrel entity
	Ogre::Entity* tankBarrel = mSceneMgr->createEntity("chba" + to_string(tankCounter), "chbarrel.mesh");
	tankBarrel->setCastShadows(true);
	tankBarrel->setMaterialName("ch_tank_material");

	// Create a child scene node and attach tank body to it
	mTankBodyNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mTankBodyNode->attachObject(tankBody);

	// Get the height of the terrain at a certain point
	mTerrain = mTerrainGroup->getTerrain(0, 0);
	float height = mTerrain->getHeightAtWorldPosition(spawnPoint.x, 0, spawnPoint.z);
	// Move it above the ground
	mTankBodyNode->translate(spawnPoint.x, height + mHeightOffset, spawnPoint.z);

	// Create a child scene node from tank body's scene node and attach the tank turret to it
	mTankTurretNode = mTankBodyNode->createChildSceneNode();
	mTankTurretNode->attachObject(tankTurret);
	// Move it above tank body
	mTankTurretNode->translate(0, 3, 0);
	mTankTurretNode->setInheritOrientation(false);

	// Create a child scene node from tank turret's scene node and attach the tank barrel to it
	mTankBarrelNode = mTankTurretNode->createChildSceneNode();
	mTankBarrelNode->attachObject(tankBarrel);
	// Move it to the appropriate position on the turret
	mTankBarrelNode->translate(-30, 10, 0);

	mProjectileSpawnNode = mTankBarrelNode->createChildSceneNode();
	mProjectileSpawnNode->translate(-90,0,0);

	// Create a BillboardSet to represent a health bar and set its properties
	mHealthBar = mSceneMgr->createBillboardSet("Healthbar" + to_string(tankCounter));
	mHealthBar->setCastShadows(false);
	mHealthBar->setDefaultDimensions(100, 10);
	mHealthBar->setMaterialName("myMaterial/HealthBar");
	// Create a billboard for the health bar BillboardSet
	mHealthBarBB = mHealthBar->createBillboard(Ogre::Vector3(0, 100, 0));
	// Set it to always draw on top of other objects
	mHealthBar->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
	// Create a BillboardSet for a selection circle and set its properties
	mSelectionCircle = mSceneMgr->createBillboardSet("SelectionCircle" + to_string(tankCounter));
	mSelectionCircle->setCastShadows(false);
	mSelectionCircle->setDefaultDimensions(60, 60);
	mSelectionCircle->setMaterialName("myMaterial/SelectionCircle");
	mSelectionCircle->setBillboardType(Ogre::BillboardType::BBT_PERPENDICULAR_COMMON);
	mSelectionCircle->setCommonDirection(Ogre::Vector3(0, 1, 0));
	mSelectionCircle->setCommonUpVector(Ogre::Vector3(0, 0, -1));
	// Create a billboard for the selection circle BillboardSet
	mSelectionCircleBB = mSelectionCircle->createBillboard(Ogre::Vector3(0, 1, 0));
	mSelectionCircleBB->setTexcoordRect(0.0, 0.0, 1.0, 1.0);

	// Attach the healthbar and selection circle
	// Calculate the health bar adjustments
	float healthBarAdjuster = (1.0 - 1)/2;	// This must range from 0.0 to 0.5
	// Set the health bar to the appropriate level
	mHealthBarBB->setTexcoordRect(0.0 + healthBarAdjuster, 0.0, 0.5 + healthBarAdjuster, 1.0);
	mTankBodyNode->attachObject(mHealthBar);
	mTankBodyNode->attachObject(mSelectionCircle);

	mHealthBar->setBillboardType(Ogre::BillboardType::BBT_POINT);

	Tank tank(tankCounter);

	tank.mTankBarrelNode = mTankBarrelNode;
	tank.mTankTurretNode = mTankTurretNode;
	tank.mTankBodyNode = mTankBodyNode;
	tank.mCameraHolder = tank.mTankTurretNode->createChildSceneNode();
	tank.mCameraHolder->translate(Ogre::Vector3(300,200,0));
	tank.mTerrain = mTerrain;
	tank.mPhysicsEngine = mPhysicsEngine;
	tank.mSceneMgr = mSceneMgr;
	tank.mBoxCount = mBoxCount;
	tank.projectiles = &projectiles;
	tank.mProjectileSpawnNode = mProjectileSpawnNode;

	tank.setTankStateToAI(true);

	tank.mHealthBar = mHealthBar;
	tank.mHealthBarBB = mHealthBarBB;
	tank.mSelectionCircle = mSelectionCircle;
	tank.mSelectionCircleBB = mSelectionCircleBB;
	tank.setTankStateToAI(true);
	tank.mTanks = &mTanks;

	mTanks.push_back(tank);
	for(std::vector<Tank>::iterator iTank = mTanks.begin(); iTank != mTanks.end(); ++iTank){
		iTank->mTanks = &mTanks;
	}

	tankCounter++;

	return true;
}

void DemoApp::checkProjectileCollision(){
	for(std::vector<Tank>::iterator iTank = mTanks.begin(); iTank != mTanks.end(); ++iTank){
		for(std::vector<Projectile*>::iterator it = projectiles.begin(); it != projectiles.end(); ++it) {
			if(iTank->mTankBodyNode->_getDerivedPosition().distance((*it)->node->_getDerivedPosition()) < 50  || abs((*it)->node->_getDerivedPosition().y - getProjectileHeightAtXZ((*it)->node->_getDerivedPosition())) < 1){
				if((*it)->node->getAttachedObjectIterator().hasMoreElements()){
					Ogre::Vector3 projectilePos = (*it)->node->getPosition();
					projectilePos.y = getProjectileHeightAtXZ(projectilePos);
					spawnExplosionParticleSystem(projectilePos);
					Ogre::MovableObject* obj = static_cast<Ogre::MovableObject*>((*it)->node->getAttachedObject(0));
					(*it)->node->getCreator()->destroyMovableObject(obj);
					(*it)->exist = false;
					iTank->tankGotHit((*it)->Lived);
				}
			}	
		}
	}

	// get rid of removed projectiles
	projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(), 
		[](Projectile *p) { return !p->exist; }), projectiles.end());
}

void DemoApp::createWorldObstacles(){
	for(int i = 0; i < 20; i ++){
		Ogre::Entity* house = mSceneMgr->createEntity("house" + to_string(i), "tudorhouse.mesh");
		house->setCastShadows(true);
		house->setMaterialName("Examples/TudorHouse");
		Ogre::SceneNode* houseNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		houseNode->attachObject(house);
		houseNode->scale(0.4,0.4,0.4);
		float x = (rand() % 9000)-4000;
		float z = (rand() % 8000)-4000;
		float y = mTerrain->getHeightAtWorldPosition(x,0,z);
		houseNode->translate(x,y+100,z);
	}
}

float DemoApp::getProjectileHeightAtXZ(Ogre::Vector3 position){
	float height = mTerrain->getHeightAtWorldPosition(position.x,0,position.z);	
	return height;
}

void DemoApp::spawnExplosionParticleSystem(Ogre::Vector3 position){
	// Create unique name
	std::ostringstream oss;
	oss << mExplosionCount;
	std::string entityName =  to_string(rand() % 10) + "explosion" + oss.str();
	// Increment box count
	mExplosionCount++;

	Ogre::ParticleSystem* particleSystem = mSceneMgr->createParticleSystem(entityName,"Examples/Explode");
	Ogre::SceneNode* particleSysNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	particleSystem->fastForward(1.0);
	particleSysNode->translate(position);
	particleSysNode->attachObject(particleSystem);
}

void DemoApp::getTankInfoForGUI(){
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
		string tankName = "chbo";
		string barrelName = "chba";
		string turretName = "chtu";
		for(std::vector<Tank>::iterator it = mTanks.begin(); it != mTanks.end(); ++it) {
			if (itr->movable->getName().compare(tankName + to_string(it->getId())) == 0) {
				selectedTankForGUI = (&*it);
				mDetailsPanel->show();
			} else if (itr->movable->getName().compare(barrelName + to_string(it->getId())) == 0) {
				selectedTankForGUI = (&*it);
				mDetailsPanel->show();
			} else if (itr->movable->getName().compare(turretName + to_string(it->getId())) == 0) {
				selectedTankForGUI = (&*it);
				mDetailsPanel->show();
			}
		}
	}
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