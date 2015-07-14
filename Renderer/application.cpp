#include <list>
#include <SDL2/SDL_mixer.h>

const string MyAppsWindowTitle = "Our Game";

#include "render.cpp"

enum gameModes {playMode, menuMode};
int gameMode;
bool dragLook=false;  // Used to reset mouse movement looking when not full screen,

scene theScene;
camera theCam;

void haltCamMotion();  // Forward Declaration
void updateCameraPosition(camera* Camera);  // Forward Declaration

void SetUp(InfonPortal* portal){
    int W,H; SDL_GetWindowSize(portal->viewPorts->window, &W, &H);

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

    haltCamMotion();

    theScene.background=color(20,20,255);

for(int c=0; c<200; c++){
    thing* newThing=new thing;
    newThing->position=randVec4(100);
    newThing->orientation.set(1,0,0,0);
    newThing->size=fRand(5);
    newThing->Color.randomize();
    newThing->asCube();
    theScene.things.push_back(newThing);
}
/*
    newThing=new thing;
    newThing->position.set(0,0,0,1);
    newThing->orientation.set(1,0,0,0);
    newThing->Color=color(200,50,100);
    newThing->asTriangle();
    theScene.things.push_back(newThing);

    newThing=new thing;
    newThing->position.set(-90,-5,-400,1);
    newThing->orientation.set(1,0,0,0);
    newThing->Color=color(20,50,200);
    newThing->asTriangle();
    theScene.things.push_back(newThing);
*/

    theCam.Scene = &theScene;
    theCam.view.position.set(0,0,0,1);
    theCam.view.orientation.set(1,0,0,0);
    theCam.view.calcViewMatrix();
    theCam.view.setFrustum(pi/20.0, W/H, 1.5, 100000.0);


};

void update(InfonPortal* portal){
    int W,H; SDL_GetWindowSize(portal->viewPorts->window, &W, &H);
    updateCameraPosition(&theCam);

	// Notify game loop that a change was made
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    ev.user.code = 0;
    ev.user.data1 = 0;  ev.user.data2 = 0;
	SDL_PushEvent(&ev);
}

void DrawScreen(InfonPortal* portal){
    int W,H; SDL_GetWindowSize(portal->viewPorts->window, &W, &H);

    SDL_Renderer* r=portal->viewPorts[0].renderer;
    RenderH=H; RenderW=W;
    theCam.render(r);
}


////////////////////////////////////////////
///  H a n d l e   G a m e   E v e n t s
////////////////////////////////////////////

#define IS_CTRL (ev.key.keysym.mod&KMOD_CTRL)
bool camGoingForward, camGoingBack, camGoingLeft, camGoingRight, camGoingUp, camGoingDown, camRun;
Vector3 camVelocity;
float camMaxSpeed;

void updateCameraPosition(camera* Camera){
double_t timeSinceLastFrame=0.05;

    // build our acceleration vector based on keyboard input composite
    Vector3 accel = ZERO;
    if (camGoingForward) {accel += Camera->view.orientation * -UNIT_Z;}
    if (camGoingBack)    {accel -= Camera->view.orientation * -UNIT_Z;}
    if (camGoingRight)   {accel += Camera->view.orientation * -UNIT_X;}
    if (camGoingLeft)    {accel -= Camera->view.orientation * -UNIT_X;}
    if (camGoingUp)      {accel += Camera->view.orientation * -UNIT_Y;}
    if (camGoingDown)    {accel -= Camera->view.orientation * -UNIT_Y;}

    // if accelerating, try to reach top speed in a certain time
    float topSpeed = camRun ? camMaxSpeed * 3 : camMaxSpeed;
    if (accel.squaredLength() != 0)
    {
        accel.normalize();
        camVelocity += accel * topSpeed * timeSinceLastFrame * 10;
    }
    // if not accelerating, try to stop in a certain time
   else camVelocity -= camVelocity * timeSinceLastFrame * 10;

   float tooSmall = std::numeric_limits<float>::epsilon();

    // keep camera velocity below top speed and above epsilon
    if (camVelocity.squaredLength() > topSpeed * topSpeed)
    {
        camVelocity.normalize();
        camVelocity *= topSpeed;
    }
    else if (camVelocity.squaredLength() < tooSmall * tooSmall)
        camVelocity = ZERO;

    if (camVelocity != ZERO) {
        Camera->view.move(camVelocity * timeSinceLastFrame);
        theCam.view.calcViewMatrix();
 //       cout<<"\n>%%%%%%%%%%%%%%%%"<<camVelocity<<"\n";
    }
 //   else{cout<<"\n>>>>>>>>>>>>>>>>"<<camMaxSpeed<<"\n";}

}


void haltCamMotion(){
    camGoingForward=0; camGoingBack=0; camGoingLeft=0; camGoingRight=0; camGoingUp=0; camGoingDown=0;
    camRun=false;
    camMaxSpeed=5;
    camVelocity=ZERO;
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
 //   MainCamera->setAspectRatio((float)W/(float)H);
 //   OgrRenderWnd->resize(W,H);
 //   OgrRenderWnd->windowMovedOrResized();
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
        case SDL_WINDOWEVENT_RESIZED: portalView->parentPortal->viewsNeedRefactoring=true; break;//ResizeTurbulancePortal(portalView->parentPortal, ev.window.data1, ev.window.data2); break;
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
            if (gameMode==menuMode) {}
            portalView->parentPortal->needsToBeDrawn=true;
            break;
        case SDLK_s: case SDLK_DOWN:
            if (gameMode==menuMode) {}
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
            if(gameMode==menuMode ) doneYet=true;
        }break;
        case SDL_SCANCODE_ESCAPE:
        case SDLK_ESCAPE:
            if(gameMode == menuMode){gameMode=playMode;}
            else {gameMode=menuMode; }
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
            theCam.view.yaw(-ev.motion.xrel * 0.001f);
            theCam.view.pitch(-ev.motion.yrel * 0.001f);
            theCam.view.calcViewMatrix();
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
