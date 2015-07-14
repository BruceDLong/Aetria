#include <list>
#include <SDL2/SDL_mixer.h>

const string MyAppsWindowTitle = "Our Game";

using namespace Ogre;

enum gameModes {playMode, menuMode};
int gameMode;
bool dragLook=false;  // Used to reset mouse movement looking when not full screen,

int crntMenuItem;
Ogre::Overlay* menuOverlay;
Ogre::TexturePtr MenuTexture;

Ogre::Real mFallVelocity;
void haltCamMotion();  // Forward Declaration
void updateCameraPosition(Ogre::Camera* Camera);  // Forward Declaration

//DotSceneLoader* pDotSceneLoader;
bool mTerrainImported;

//=============================  This function is for use with Ogre-Procedural
#include "ProceduralStableHeaders.h"
#include "Procedural.h"
#include "ProceduralUtils.h"
#include <ProceduralSphereGenerator.h>

void putMeshMat(const std::string& meshName, const std::string& matName, const Ogre::Vector3& position = Ogre::Vector3::ZERO, bool castShadows=true)
{
	Ogre::Entity* ent2 = MainSceneMgr->createEntity(meshName);
	Ogre::SceneNode* sn = MainSceneMgr->getRootSceneNode()->createChildSceneNode();
	sn->attachObject(ent2);
	sn->setPosition(position);
	ent2->setMaterialName(matName);
	ent2->setCastShadows(castShadows);
}
//==============================

void SetUp(InfonPortal* portal){
	crntMenuItem=0; // "Load"
	gameMode=playMode;

	// init audio
	int audio_rate = 22050;
	Uint16 audio_format = AUDIO_S16SYS;
	int audio_channels = 2;
	int audio_buffers = 4096;

	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
		fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
		exit(1);
	}
	// load sounds
	//LaserSound = Mix_LoadWAV("laserSound.wav");
	//if(LaserSound==0) fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());

MainSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
//MainSceneMgr->setSkyBox(true, "Examples/TrippySkyBox", 5, 8);
//MainSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox", 5, 8);
//MainSceneMgr->setSkyBox(true, "Examples/SceneSkyBox1", 5, 8);
//MainSceneMgr->setSkyBox(true, "Examples/SceneSkyBox2", 5, 8);
//MainSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox", 5, 8);
//MainSceneMgr->setSkyBox(true, "Examples/StormySkyBox", 5, 8);
//MainSceneMgr->setSkyBox(true, "Examples/EarlyMorningSkyBox", 5, 8);
//MainSceneMgr->setSkyBox(true, "Examples/EveningSkyBox", 5, 8);
//MainSceneMgr->setSkyPlane(true, "Examples/SpaceSkyPlane", 5, 8);
	int W,H; SDL_GetWindowSize(portal->viewPorts->window, &W, &H);

    // Set up the main camera

	MainSceneMgr->createLight("Light")->setPosition(75,75,75);
    MainSceneMgr->setAmbientLight(Ogre::ColourValue(0.2f, 0.3f, 0.7f));

    MainCamera = MainSceneMgr->createCamera("MainCamera");
    MainCamera->setPosition(Ogre::Vector3(0,15.0,40));
    MainCamera->lookAt(Ogre::Vector3(0,0,-300));
    MainCamera->setNearClipDistance(0.1);

    OgrViewport = OgrRenderWnd->addViewport(MainCamera);
    OgrViewport->setBackgroundColour(Ogre::ColourValue(0.3f, 0.3f, 0.6f, 0.5f));
    MainCamera->setAspectRatio(Ogre::Real(OgrViewport->getActualWidth()) / Ogre::Real(OgrViewport->getActualHeight()));
    OgrViewport->setCamera(MainCamera);

    haltCamMotion();

/*
    // DotSceneLoader stuff
    pDotSceneLoader = new DotSceneLoader();
    //pDotSceneLoader->parseDotScene("CubeScene.xml", "General", MainSceneMgr, MainSceneMgr->getRootSceneNode());
    pDotSceneLoader->parseDotScene("testScene.scene", "General", MainSceneMgr, MainSceneMgr->getRootSceneNode());
    mTerrainImported=true;
    for(unsigned int ij = 0;ij < pDotSceneLoader->mPGHandles.size();ij++) {
        pDotSceneLoader->mPGHandles[ij]->setCamera(MainCamera);
    }
*/

    { // Prepare the menu overlay
    MenuTexture = Ogre::TextureManager::getSingleton().createManual( "MenuTexture", // name
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, // type
        500, 500, // width & height of the render window
        0, // number of mipmaps
        Ogre::PF_A8R8G8B8, // pixel format chosen to match a format Cairo can use
        Ogre::TU_DEFAULT  // usage
        ,0,false, 8);
        //);  // PF_R8G8B8, TU_RENDERTARGET,0,false, fsaa);


//RenderTarget *rttTex = MenuTexture->getBuffer()->getRenderTarget();
 Ogre::HardwarePixelBufferSharedPtr pixelBuffer = MenuTexture->getBuffer();
 pixelBuffer->lock( Ogre::HardwareBuffer::HBL_NORMAL );
 const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
 uint32_t* pDest = static_cast<uint32_t*> ( pixelBox.data );
 int size=MenuTexture->getWidth() * MenuTexture->getHeight() ;
 for(int i=0; i<size; ++i){
     pDest[i]=0x0;
 }
// memset( pDest, 0xffff81ff,  MenuTexture->getWidth() * MenuTexture->getHeight() );
 pixelBuffer->unlock();

     // Create an overlay
    Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();

    menuOverlay = overlayManager.create( "MenuOverlay" );

    Ogre::MaterialPtr MenuMaterial = Ogre::MaterialManager::getSingleton().create( "MenuMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
    MenuMaterial->getTechnique( 0 )->getPass( 0 )->createTextureUnitState( "MenuTexture" );
    MenuMaterial->getTechnique( 0 )->getPass( 0 )->setSceneBlending( Ogre::SBT_TRANSPARENT_ALPHA );

     // Create a panel
    // Ogre::OverlayContainer* panel = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", "PanelName" ) );
     Ogre::OverlayElement* panel = overlayManager.createOverlayElement("Panel", "menuPanel");
     panel->setMetricsMode(Ogre::GMM_RELATIVE);
     panel->setPosition( 0.0, 0.0 );
     panel->setDimensions( 0.4, 1.0 );
     panel->setMaterialName( "MenuMaterial" );
     // Add the panel to the overlay
     menuOverlay->add2D( panel );

     // Show the overlay
     //menuOverlay->show();
     }

 // -- Ground plane
Procedural::PlaneGenerator().setNumSegX(20).setNumSegY(20).setSizeX(300).setSizeY(150).setUTile(1.0).setVTile(1.0).realizeMesh("planeMesh");
putMeshMat("planeMesh", "rock/0");  // putMesh2
/*
Procedural::CapsuleGenerator (5, 8, 200, 100, 4).realizeMesh("cap");
putMeshMat("cap", "a29/lava", Ogre::Vector3(0,7,70));  // putMesh2

Procedural::SphereGenerator (1.0, 100, 100).realizeMesh("sphere");
putMeshMat("sphere", "earth/7", Ogre::Vector3(0,5,7));




 // -- Road
// The path of the road, generated from a simple spline
Procedural::Path p = Procedural::CatmullRomSpline3().setNumSeg(8).addPoint(5,10,15).addPoint(0,0,10).addPoint(10,0,10).addPoint(20,0,0).close().realizePath().scale(2);
// The shape that will be extruded along the path
Procedural::Shape s = Procedural::Shape().addPoint(-1.0f,.2f).addPoint(-0.0,.2f).addPoint(-0.0f,.1f).addPoint(0.0f,.1f).addPoint(0.0,.2f).addPoint(1.0f,.2f).scale(2).setOutSide(Procedural::SIDE_LEFT);
// This is an example use of a shape texture track,
// which specifies how texture coordinates should be mapped relative to the shape points
Procedural::Track textureTrack = Procedural::Track(Procedural::Track::AM_POINT).addKeyFrame(0,0).addKeyFrame(2,.2f).addKeyFrame(3,.8f).addKeyFrame(5,1);
// The extruder actually creates the road mesh from all parameters
Procedural::Extruder().setExtrusionPath(&p).setShapeToExtrude(&s).setShapeTextureTrack(&textureTrack).setUTile(20.f).realizeMesh("extrudedMesh");
putMeshMat("extrudedMesh", "Examples/Road");
/*
 // -- Pillar
// The path of the pillar, just a straight line
Procedural::Path pillarBodyPath = Procedural::LinePath().betweenPoints(Vector3(0,0,0), Vector3(0,5,0)).realizePath();
// We're doing something custom for the shape to extrude
Procedural::Shape pillarBodyShape;
const int pillarSegs=64;
for (int i=0;i<pillarSegs;i++)
	pillarBodyShape.addPoint(.5*(1-.15*Math::Abs(Math::Sin(i/(float)pillarSegs*8.*Math::TWO_PI))) *
	Vector2(Math::Cos(i/(float)pillarSegs*Math::TWO_PI), Math::Sin(i/(float)pillarSegs*Math::TWO_PI)));
pillarBodyShape.close();
// We're also setting up a scale track, as traditionnal pillars are not perfectly straight
Procedural::Track pillarTrack = Procedural::CatmullRomSpline2().addPoint(0,1).addPoint(0.5f,.95f).addPoint(1,.8f).realizeShape().convertToTrack(Procedural::Track::AM_RELATIVE_LINEIC);
// Creation of the pillar body
Procedural::TriangleBuffer pillarTB;
Procedural::Extruder().setExtrusionPath(&pillarBodyPath).setShapeToExtrude(&pillarBodyShape).setScaleTrack(&pillarTrack).setCapped(false).setPosition(0,1,0).addToTriangleBuffer(pillarTB);
// Creation of the top and the bottom of the pillar
Procedural::Shape s3 = Procedural::RoundedCornerSpline2().addPoint(-1,-.25f).addPoint(-1,.25f).addPoint(1,.25f).addPoint(1,-.25f).close().realizeShape().setOutSide(Procedural::SIDE_LEFT);
Procedural::Path p3;
for (int i=0;i<32;i++)
	{
	Ogre::Radian r = (Ogre::Radian) (Ogre::Math::HALF_PI-(float)i/32.*Ogre::Math::TWO_PI);
	p3.addPoint(0,-.5+.5*i/32.*Math::Sin(r),-1+.5*i/32.*Math::Cos(r));
	}
p3.addPoint(0,0,-1).addPoint(0,0,1);
for (int i=0;i<32;i++)
	{
	Ogre::Radian r = (Ogre::Radian) (Ogre::Math::HALF_PI-(float)i/32.*Ogre::Math::TWO_PI);
	p3.addPoint(0,-.5+.5*(1-i/32.)*Math::Sin(r),1+.5*(1-i/32.)*Math::Cos(r));
	}
Procedural::Extruder().setExtrusionPath(&p3).setShapeToExtrude(&s3).setPosition(0,6.,0).addToTriangleBuffer(pillarTB);
//Procedural::BoxGenerator().setPosition(0,6.5,0).addToTriangleBuffer(pillarTB);
Procedural::BoxGenerator().setPosition(0,.5,0).addToTriangleBuffer(pillarTB);
pillarTB.transformToMesh("pillar");
// We put the pillars on the side of the road
for (int i=0;i<p.getSegCount();i++)
	if (i%2==0)
		putMeshMat("pillar", "Examples/Marble", p.getPoint(i)+4*p.getAvgDirection(i).crossProduct(Ogre::Vector3::UNIT_Y).normalisedCopy());*/
/*
 // -- Jarre
//
Procedural::TriangleBuffer tb;
// Body
Procedural::Shape jarreShape = Procedural::CubicHermiteSpline2().addPoint(Ogre::Vector2(0,0), Ogre::Vector2::UNIT_X, Ogre::Vector2::UNIT_X)
.addPoint(Ogre::Vector2(2,3))
.addPoint(Ogre::Vector2(.5,5), Ogre::Vector2(-1,1).normalisedCopy(), Ogre::Vector2::UNIT_Y)
.addPoint(Ogre::Vector2(1,7), Ogre::Vector2(1,1).normalisedCopy()).realizeShape().thicken(.1f).getShape(0);
Procedural::Lathe().setShapeToExtrude(&jarreShape).addToTriangleBuffer(tb);
// Handle 1
Procedural::Shape jarreHandleShape = Procedural::CircleShape().setRadius(.2f).realizeShape();

Procedural::Extruder().setShapeToExtrude(&jarreHandleShape).setExtrusionPath(&jarreHandlePath).addToTriangleBuffer(tb);
// Handle2
jarreHandlePath.reflect(Ogre::Vector3::UNIT_Z);
Procedural::Extruder().setShapeToExtrude(&jarreHandleShape).setExtrusionPath(&jarreHandlePath).addToTriangleBuffer(tb);
tb.transformToMesh("jarre");
putMeshMat("jarre", "Examples/Marble", Vector3(5,0,5));
*/



////////////////////////////////////////////////////////////////////////////////////////////////////
#include "barrel1.cpp"
#include "apple1.cpp"

//===================================================================================================
///==============================================================================
/*
Procedural::Shape wallNoHoles = Procedural::RectangleShape().setHeight(40.0).setWidth(30.0).realizeShape();
Procedural::Shape window1 = Procedural::RectangleShape().setHeight(4.0).setWidth(6.0).realizeShape();
window1.translate(5,2);
Procedural::MultiShape wall = window1.booleanDifference(wallNoHoles);
/*
Procedural::Shape window2 = Procedural::CircleShape().setRadius(6.0).realizeShape();
window1.translate(3,1);
Procedural::MultiShape wall = window2.booleanDifference(wallA);

Procedural::Path wallPath = Procedural::Path().addPoint(Ogre::Vector3(0,0,0)).addPoint(Ogre::Vector3(0,0,0.1));
Procedural::Extruder().setMultiShapeToExtrude(&wall).setExtrusionPath(&wallPath).realizeMesh("wall");
putMeshMat("wall", "Examples/Rockwall", Vector3(1,20,10));


//Procedural::MultiShape wall2 = window2.booleanDifference(window1.booleanDifference(wallNoHoles));
*/ 
};
///===================================================================================
void update(InfonPortal* portal){
    int W,H;
    SDL_GetWindowSize(portal->viewPorts->window, &W, &H);

    updateCameraPosition(MainCamera);


	// Notify game loop that a change was made
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    ev.user.code = 0;
    ev.user.data1 = 0;  ev.user.data2 = 0;
	SDL_PushEvent(&ev);
}

void DrawScreen(InfonPortal* portal){
    int W,H; SDL_GetWindowSize(portal->viewPorts->window, &W, &H);

    // Draw the menu
    if(gameMode==menuMode){
 Ogre::HardwarePixelBufferSharedPtr pixelBuffer = MenuTexture->getBuffer();
 pixelBuffer->lock( Ogre::HardwareBuffer::HBL_NORMAL );
 const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
  uint32_t* pDest = static_cast<uint32_t*> ( pixelBox.data );
 int size=MenuTexture->getWidth() * MenuTexture->getHeight() ;
 for(int i=0; i<size; ++i){
     pDest[i]=0x885599ee;
 }
 pixelBuffer->unlock();
        cairo_t *cr = cairo_create(portal->cairoSurf);
        //cairo_set_antialias(portal->cr,CAIRO_ANTIALIAS_GRAY);

        cairo_set_line_width (cr, 0.1);
        cairo_set_source_rgba (cr, 0, 0, 0, 0.55);
        cairo_rectangle (cr, 0, 0, W/3, H);
        cairo_fill (cr);

        std::vector<string> menuItems={"Load","Save","Settings","Help","Quit"};
        const unsigned int itemHeight=60;
        int menuTop=H/2-(menuItems.size()*itemHeight/2);

        cairo_set_source_rgba (cr, 1, 1, 1, 0.40);
        cairo_rectangle (cr, W/3-300, menuTop+(crntMenuItem*itemHeight),300 , itemHeight+5);
        cairo_fill (cr);

        for(uint i=0; i<menuItems.size(); ++i){
            cairo_set_source_rgba(cr, 0.94,0.92,0.92, 0.8);
            cairo_move_to(cr, W/3-250, menuTop+(i*itemHeight));
            renderText(cr, menuItems[i].data(), "times 40");
            cairo_fill(cr);
        }

        displayImage(cr, "./media/editedsymbol.png", W/3-295,menuTop+(crntMenuItem*itemHeight)+5, 2.2);


        cairo_destroy(cr);
    }

    // Render a frame of the scene.
    OgrRenderWnd->update(false);
}
   

////////////////////////////////////////////
///  H a n d l e   G a m e   E v e n t s
////////////////////////////////////////////

#define IS_CTRL (ev.key.keysym.mod&KMOD_CTRL)
bool camGoingForward, camGoingBack, camGoingLeft, camGoingRight, camGoingUp, camGoingDown, camRun;
Ogre::Vector3 camVelocity;
Ogre::Real camMaxSpeed;

void updateCameraPosition(Ogre::Camera* Camera){
double_t timeSinceLastFrame=0.05;

    // build our acceleration vector based on keyboard input composite
    Ogre::Vector3 accel = Ogre::Vector3::ZERO;
    if (camGoingForward) accel += Camera->getDirection();
    if (camGoingBack) accel -= Camera->getDirection();
    if (camGoingRight) accel += Camera->getRight();
    if (camGoingLeft) accel -= Camera->getRight();
    if (camGoingUp) accel += Camera->getUp();
    if (camGoingDown) accel -= Camera->getUp();

    // if accelerating, try to reach top speed in a certain time
    Ogre::Real topSpeed = camRun ? camMaxSpeed * 3 : camMaxSpeed;
    if (accel.squaredLength() != 0)
    {
        accel.normalise();
        camVelocity += accel * topSpeed * timeSinceLastFrame * 10;
    }
    // if not accelerating, try to stop in a certain time
   else camVelocity -= camVelocity * timeSinceLastFrame * 10;

    Ogre::Real tooSmall = std::numeric_limits<Ogre::Real>::epsilon();

    // keep camera velocity below top speed and above epsilon
    if (camVelocity.squaredLength() > topSpeed * topSpeed)
    {
        camVelocity.normalise();
        camVelocity *= topSpeed;
    }
    else if (camVelocity.squaredLength() < tooSmall * tooSmall)
        camVelocity = Ogre::Vector3::ZERO;

    if (camVelocity != Ogre::Vector3::ZERO) Camera->move(camVelocity * timeSinceLastFrame);


    //////////
/*     if (1) {// (!flyingMode)
        // clamp to terrain
        Ogre::Vector3 camPos = MainCamera->getPosition();
        Ogre::Ray ray;
        ray.setOrigin(Ogre::Vector3(camPos.x, 10000, camPos.z));
        ray.setDirection(Ogre::Vector3::NEGATIVE_UNIT_Y);

        Ogre::TerrainGroup::RayResult rayResult = pDotSceneLoader->getTerrainGroup()->rayIntersects(ray);
        Ogre::Real distanceAboveTerrain = 2.4f;
        Ogre::Real fallSpeed = 200;
        Ogre::Real newy = camPos.y;
        if (rayResult.hit)
        {
            if (camPos.y > rayResult.position.y + distanceAboveTerrain)
            {
                mFallVelocity += timeSinceLastFrame * 10;
                mFallVelocity = std::min(mFallVelocity, fallSpeed);
                newy = camPos.y - mFallVelocity * timeSinceLastFrame;

            }
            newy = std::max(rayResult.position.y + distanceAboveTerrain, newy);
            MainCamera->setPosition(camPos.x, newy, camPos.z);
        }
    }

    // Update DotScene stuff
    if (!pDotSceneLoader->getTerrainGroup()->isDerivedDataUpdateInProgress()){
        if (mTerrainImported){
            pDotSceneLoader->getTerrainGroup()->saveAllTerrains(true);
            mTerrainImported = false;
        }
    }
    for(unsigned int ij = 0;ij < pDotSceneLoader->mPGHandles.size();ij++){
        pDotSceneLoader->mPGHandles[ij]->update();
    }
*/
}
    
void haltCamMotion(){
    camGoingForward=0; camGoingBack=0; camGoingLeft=0; camGoingRight=0; camGoingUp=0; camGoingDown=0;
    camRun=false;
    camMaxSpeed=5;
    camVelocity=Ogre::Vector3::ZERO;
}

bool handleCameraKeyEvent(SDL_Keysym* keysym, bool KeyDown){
    camRun = (keysym->mod&KMOD_SHIFT)?true:false;
    switch(keysym->sym){
        case SDLK_w: case SDLK_UP:   camGoingForward = KeyDown; return true;
        case SDLK_s: case SDLK_DOWN: camGoingBack = KeyDown; return true;
        case SDLK_a: case SDLK_LEFT: camGoingLeft = KeyDown; return true;
        case SDLK_d: case SDLK_RIGHT: camGoingRight = KeyDown; return true;
        case SDLK_PAGEUP:   camGoingUp = KeyDown; return true;
        case SDLK_PAGEDOWN: camGoingDown = KeyDown; return true;
    }
    return false;
}

void NotifyOgreOfScreenResize(InfonPortal* portal){
    int W,H; SDL_GetWindowSize(portal->viewPorts->window, &W, &H);
    MainCamera->setAspectRatio((float)W/(float)H);
    OgrRenderWnd->resize(W,H);
    OgrRenderWnd->windowMovedOrResized();
}


void HandleEvent(InfonViewPort *portalView, SDL_Window *window, SDL_Event &ev){
    switch (ev.type) {
    case SDL_USEREVENT:
        --numEvents;
        switch(ev.user.code){
            case 0:
///                 portals[(UInt)ev.user.data1]->crntFrame=(infon*)ev.user.data2;
              portals[0]->needsToBeDrawn=true;
              break;
            case TURB_ADD_SCREEN: break;
            case TURB_DEL_SCREEN: break;
            case TURB_ADD_WINDOW: break;
            case TURB_DEL_WINDOW: break;
        }
        break;
    case SDL_WINDOWEVENT:
        switch (ev.window.event) {
        case SDL_WINDOWEVENT_MOVED: portalView->parentPortal->viewsNeedRefactoring=true; break;
        case SDL_WINDOWEVENT_RESIZED: NotifyOgreOfScreenResize(portalView->parentPortal); portalView->parentPortal->viewsNeedRefactoring=true; break;//ResizeTurbulancePortal(portalView->parentPortal, ev.window.data1, ev.window.data2); break;
        case SDL_WINDOWEVENT_MINIMIZED: portalView->isMinimized=true;  break;  // Stop rendering
        case SDL_WINDOWEVENT_MAXIMIZED: break;
        case SDL_WINDOWEVENT_RESTORED: portalView->isMinimized=false;  break;  // Resume rendering
        case SDL_WINDOWEVENT_FOCUS_GAINED:break; // Move window to front position
        case SDL_WINDOWEVENT_FOCUS_LOST:break;
        case SDL_WINDOWEVENT_CLOSE:  CloseTurbulanceViewport(portalView); break;
        case SDL_WINDOWEVENT_ENTER:break; // Mouse enters window
        case SDL_WINDOWEVENT_LEAVE:break; // Mouse leaves window
        }
        break;
    case SDL_KEYDOWN:
        if(gameMode==playMode && handleCameraKeyEvent(&ev.key.keysym, true)){break;}
        switch (ev.key.keysym.sym) {
        case SDLK_PRINTSCREEN: break; // see sdl_common.c
        case SDLK_w: case SDLK_UP:
            if (gameMode==menuMode) {crntMenuItem--; if(crntMenuItem<0) crntMenuItem=4;}
            portalView->parentPortal->needsToBeDrawn=true;
            break;
        case SDLK_s: case SDLK_DOWN:
            if (gameMode==menuMode) {crntMenuItem++; if(crntMenuItem>4) crntMenuItem=0;}
            portalView->parentPortal->needsToBeDrawn=true;
            break;

        case SDLK_a: case SDLK_LEFT:  break;
        case SDLK_d: case SDLK_RIGHT: break;

        case SDLK_SPACE:  {
            doneYet=true;
        } break;
        case SDLK_f: portalView->toggleFullScreen(); break; // Toggle fullscreen.
        case SDLK_o: break;
        case SDLK_RETURN: {
            if(gameMode==menuMode && crntMenuItem==4) doneYet=true;
        }break;
        case SDL_SCANCODE_ESCAPE:
        case SDLK_ESCAPE:
            if(gameMode == menuMode){gameMode=playMode; menuOverlay->hide();}
            else {gameMode=menuMode; menuOverlay->show();}
            break;
        //case SDLK_UP:    break;
        //case SDLK_DOWN:  break;
        //case SDLK_LEFT:  break;
        //case SDLK_RIGHT: break;
        case SDLK_PAGEUP:  break;
        case SDLK_PAGEDOWN:break;
        case SDLK_KP_PLUS: break;
        case SDLK_KP_MINUS: break;
        }
        break;
    case SDL_KEYUP:
        if(gameMode==playMode && handleCameraKeyEvent(&ev.key.keysym, false)){break;}

        break;
    case SDL_TEXTEDITING:       break;
    case SDL_TEXTINPUT:         break;

    case SDL_MOUSEMOTION:
        if(dragLook || portalView->isFullScreen){
            MainCamera->yaw(Ogre::Degree(-ev.motion.xrel * 0.15f));
            MainCamera->pitch(Ogre::Degree(-ev.motion.yrel * 0.15f));
        }
        break;
    case SDL_MOUSEBUTTONDOWN: dragLook = true;  break;
    case SDL_MOUSEBUTTONUP:   dragLook = false; break;
    case SDL_MOUSEWHEEL:{}
    case SDL_FINGERDOWN:
    case SDL_FINGERUP:{}
    case SDL_FINGERMOTION:
        portalView->parentPortal->needsToBeDrawn=true;
    case SDL_MULTIGESTURE:      break;
    case SDL_DOLLARGESTURE:     break;
    case SDL_DOLLARRECORD:      break;

    case SDL_JOYAXISMOTION:     break;
    case SDL_JOYBALLMOTION:     break;
    case SDL_JOYHATMOTION:      break;
    case SDL_JOYBUTTONDOWN:     break;
    case SDL_JOYBUTTONUP:       break;

    case SDL_CLIPBOARDUPDATE:   break;
    case SDL_DROPFILE:          break;
    case SDL_QUIT: doneYet=true; break;

    }
}

void shutDown(){
	//SDL_Delay(5000);
}




