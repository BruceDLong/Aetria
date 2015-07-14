// Text-gui
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <map>
#include <algorithm>

#include <memory>
#include <cstdarg>
#include <pthread.h>

#include <pango/pangocairo.h>

using namespace std;


#define DEB(msg)  {cout<< msg;}
#define DEBl(msg) {cout<< msg << "\n";}
#define DEB2(msg) //{cout<< msg;}  // Use to show drawing commands
#define MSG(msg)  {cout<< msg << flush;}
#define MSGl(msg) {cout<< msg << "\n" << flush;}
#define ERR(msg)  {cout<< msg;}
#define ERRl(msg) {cout<< msg << "\n";}
#define Indent {for (int x=0;x<indent;++x) indentStr+="   |";}

#define MAX_PORTALS 24
static int doneYet=0, numEvents=0, numPortals=0;

static inline uint32_t map_value(uint32_t val, uint32_t max, uint32_t tomax){
    return((uint32_t)((double)val * (double)tomax/(double)max));
}

std::string sFmt(const std::string fmt_str, ...) {
    int final_n, n = fmt_str.size() * 2; /* reserve 2 times as much as the length of the fmt_str */
    std::string str;
    std::unique_ptr<char[]> formatted;
    va_list ap;
    while(1) {
        formatted.reset(new char[n]); /* wrap the plain char array into the unique_ptr */
        strcpy(&formatted[0], fmt_str.c_str());
        va_start(ap, fmt_str);
        final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
        va_end(ap);
        if (final_n < 0 || final_n >= n)
            n += abs(final_n - n + 1);
        else
            break;
    }
    return std::string(formatted.get());
}

static inline SDL_Surface *load_image(const char *filename){
    SDL_Surface* loadedImage = NULL;
  //  SDL_Surface* optimizedImage = NULL;

    if((loadedImage = IMG_Load(filename))) {
       // optimizedImage = SDL_DisplayFormat(loadedImage);
      //  SDL_FreeSurface(loadedImage);
    }
    return loadedImage; //optimizedImage;
}

SDL_Texture *LoadTexture(SDL_Renderer *renderer, char *file, SDL_bool transparent){
    SDL_Surface *temp=IMG_Load(file);;
    SDL_Texture *texture;

    if (temp == NULL) {fprintf(stderr, "Couldn't load %s: %s", file, SDL_GetError()); return NULL;}

    if (transparent) { //Set transparent pixel as the pixel at (0,0)
        if (temp->format->palette) {
            SDL_SetColorKey(temp, SDL_TRUE, *(Uint8 *) temp->pixels);
        } else {
            switch (temp->format->BitsPerPixel) {
            case 15: SDL_SetColorKey(temp, SDL_TRUE, (*(Uint16 *) temp->pixels) & 0x00007FFF); break;
            case 16: SDL_SetColorKey(temp, SDL_TRUE, *(Uint16 *) temp->pixels); break;
            case 24: SDL_SetColorKey(temp, SDL_TRUE, (*(Uint32 *) temp->pixels) & 0x00FFFFFF); break;
            case 32: SDL_SetColorKey(temp, SDL_TRUE, *(Uint32 *) temp->pixels); break;
            }
        }
    }

    texture = SDL_CreateTextureFromSurface(renderer, temp);
    if (!texture) {fprintf(stderr, "Couldn't create texture: %s\n", SDL_GetError()); SDL_FreeSurface(temp);  return NULL; }
    SDL_FreeSurface(temp);
    return texture;
}


struct InfonPortal; //forward
struct InfonViewPort {
    SDL_Window *window;
    SDL_GLContext GLContext;
    SDL_Renderer *renderer;
    bool isMinimized;
    bool isFullScreen;
    int posX, posY; // location in parent window.
    double scale; // MAYBE: Make views scalable?
    InfonPortal *parentPortal;
    InfonViewPort *next;


    void toggleFullScreen(){
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) {
            SDL_SetWindowFullscreen(window, SDL_FALSE);
        } else {SDL_SetWindowFullscreen(window, SDL_TRUE);};
        isFullScreen=(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN);
        SDL_SetRelativeMouseMode(SDL_bool(isFullScreen));
    }
};

struct InfonPortal {
    InfonPortal(){surface=0; memset(this, 0, sizeof(InfonPortal));};
    ~InfonPortal(){if(surface) SDL_FreeSurface(surface);};
    SDL_Surface *surface;
    cairo_surface_t *cairoSurf;
    cairo_t *cr;
    SDL_Texture* Tex;
    SDL_Surface *SDL_background;
    cairo_surface_t *cairo_background;
//    infonPtr theme, stuff, crntFrame;
//    User *user;
    bool needsToBeDrawn, viewsNeedRefactoring, isLocked;
    InfonViewPort *viewPorts;
};

InfonPortal *portals[MAX_PORTALS];


string fontDescription="Sans 5";

void renderText(cairo_t *cr, const char* text, char* fontDesc){ DEB2("'"<<text<<"'");
    PangoLayout *layout=pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, text, -1);

    PangoFontDescription *desc = pango_font_description_from_string(fontDesc);
    pango_layout_set_font_description(layout, desc);
    pango_font_description_free(desc);

    cairo_set_line_width(cr, 0.5);
    pango_cairo_update_layout(cr, layout);
    pango_cairo_layout_path(cr, layout);
    g_object_unref(layout);
}


void CreateLinearGradient(cairo_t *cr,double x1, double y1, double x2, double y2){
    cairo_pattern_t *pattern=0;
    pattern=cairo_pattern_create_linear(x1,y1,x2,y2);
    cairo_set_source(cr, pattern);
    cairo_pattern_destroy(pattern);
}

void CreateRadialGradient(cairo_t *cr,double cx1, double cy1, double r1, double cx2, double cy2, double r2){
    cairo_pattern_t *pattern=0;
    pattern=cairo_pattern_create_radial(cx1,cy1,r1,cx2,cy2,r2);
    cairo_set_source(cr, pattern);
    cairo_pattern_destroy(pattern);
}


void AddColorStop(cairo_t *cr, double offset, double r, double g, double b, double alpha){
    cairo_pattern_add_color_stop_rgba(cairo_get_source(cr), offset, r, g, b, alpha);
}

void loadImageSurface(cairo_t *cr, char* filename, double x, double y){
    cairo_surface_t *image=cairo_image_surface_create_from_png(filename);
    cairo_set_source_surface(cr, image, x, y);
    cairo_surface_destroy(image);
}

void getImageProperties(const char* filename, int &width, int &height){
    cairo_surface_t *image=cairo_image_surface_create_from_png(filename);
    width=cairo_image_surface_get_width(image);
    height=cairo_image_surface_get_height(image);
    cairo_surface_destroy(image);
}

void roundedRectangle(cairo_t *cr, double x, double y, double w, double h, double r){
    cairo_move_to(cr,x+r,y);                      //# Move to A
    cairo_line_to(cr,x+w-r,y);                    //# Straight line to B
    cairo_curve_to(cr,x+w,y,x+w,y,x+w,y+r);       //# Curve to C, Control points are both at Q
    cairo_line_to(cr,x+w,y+h-r);                  //# Move to D
    cairo_curve_to(cr,x+w,y+h,x+w,y+h,x+w-r,y+h); //# Curve to E
    cairo_line_to(cr,x+r,y+h);                    //# Line to F
    cairo_curve_to(cr,x,y+h,x,y+h,x,y+h-r);       //# Curve to G
    cairo_line_to(cr,x,y+r);                      //# Line to H
    cairo_curve_to(cr,x,y,x,y,x+r,y);             //# Curve to A;
}

typedef map<string, cairo_surface_t*> picCache_t;
picCache_t picCache;

void displayImage(cairo_t *cr, const char* filename, double x, double y, double scale){
    cairo_surface_t* pic=0;
    picCache_t::iterator picPtr=picCache.find(filename);
    if (picPtr==picCache.end()) {picCache[filename]=pic=cairo_image_surface_create_from_png(filename);}
    else pic=picPtr->second;
    {//pic=cairo_image_surface_create_from_png(filename);
  /*      int w = cairo_image_surface_get_width (pic);
        int h = cairo_image_surface_get_height (pic);
        double pw = portal->surface->w;
        double ph = portal->surface->h;  */
        cairo_save(cr);

        cairo_scale(cr,1/scale,1/scale);
        cairo_set_source_surface(cr,pic,x*scale,y*scale);
        cairo_paint(cr);
        cairo_restore(cr);
    }
}

enum dTools{rectangle=1, curvedRect, circle, lineTo, lineRel, moveTo, moveRel, curveTo, curveRel, arcClockWise, arcCtrClockW, text,
            strokePreserve=14, fillPreserve, strokePath=16, fillPath, paintSurface, closePath,
            inkColor=20, inkColorAlpha, inkLinearGrad, inkRadialGrad, inkImage, inkDrawing, inkSetColorPt,
            lineWidth=40, lineStyle, fontFace, fontSize,
            drawToScrnN=50, drawToWindowN, drawToMemory, drawToPDF, drawToPS, drawToSVG, drawToPNG,
            shiftTo=60, scaleTo, rotateTo,
            loadImage=70, setBackgndImg, dispBackgnd, cachePic, dispPic,
            drawItem=100
};


/////////////////////////////////////
/////////////////// End of Slip Drawing, Begin Interface to Proteus Engine

enum userActions {TURB_UPDATE_SURFACE=1, TURB_ADD_SCREEN, TURB_DEL_SCREEN, TURB_ADD_WINDOW, TURB_DEL_WINDOW};

void CloseTurbulanceViewport(InfonViewPort* viewPort){
    if(viewPort->renderer) SDL_DestroyRenderer(viewPort->renderer);
    if(viewPort->GLContext) SDL_GL_DeleteContext(viewPort->GLContext);
    if(viewPort->window) SDL_DestroyWindow(viewPort->window);
    InfonPortal *portal=viewPort->parentPortal;
        // Remove this portal from parent
        InfonViewPort *currP, *prevP=0;
        for(currP=portal->viewPorts; currP != NULL; prevP=currP, currP=currP->next) {
            if (currP == viewPort) {
                if (prevP == NULL) portal->viewPorts = currP->next;
                else prevP->next = currP->next;
            }
        }
        delete(viewPort);
        if (portal->viewPorts==0){
            if(portal->cairoSurf) cairo_surface_destroy(portal->cairoSurf);
            if(portal->surface) {SDL_FreeSurface(portal->surface); portal->surface=0;}
 //           delete(portal->theme); delete(portal->crntFrame);
   //         delete(portal->user);

            // remove self from Portals[]
            int p;
            for(p=0; p<numPortals; ++p){if(portals[p]==portal) break;}
            while(++p<numPortals) portals[p-1]=portals[p];
            --numPortals;
            delete(portal);
        }
}

void ResizeTurbulancePortal(InfonPortal* portal, int w, int h){
    if(portal->cairoSurf) cairo_surface_destroy(portal->cairoSurf);
    if(portal->surface) {SDL_FreeSurface(portal->surface); portal->surface=0;}
    portal->needsToBeDrawn=true;
    portal->surface = SDL_CreateRGBSurface (0, w, h, 32,0x00ff0000,0x0000ff00,0x000000ff,0);
    portal->cairoSurf = cairo_image_surface_create_for_data((unsigned char*)portal->surface->pixels, CAIRO_FORMAT_RGB24, w, h,portal->surface->pitch);
    for(InfonViewPort *portView=portal->viewPorts; portView; portView=portView->next) // for each view
        SDL_RenderSetViewport(portView->renderer, NULL);
}

void RefactorPortal(InfonPortal *portal){
    int viewX, viewY, viewW, viewH, portalW, portalH;
    int windowsHere=0, currPos=0, prevPos=0, gap=0;
    map <int, InfonViewPort*> sides;
    map <int, InfonViewPort*>::iterator it;
    InfonViewPort *currView=0; InfonViewPort *portView=0;
    // Find X positions for all this portal's views
    for(portView=portal->viewPorts; portView; portView=portView->next){ // for each view
        SDL_GetWindowPosition(portView->window, &viewX, &viewY);
        SDL_GetWindowSize(portView->window, &viewW, &viewH);
        sides[viewX]=portView;
        sides[viewX+viewW]=portView;
    }
    for (it=sides.begin() ; it != sides.end(); it++ ){
        currPos=(*it).first;
        currView=(*it).second;
        if(windowsHere==0) gap+=(currPos-prevPos);
        SDL_GetWindowPosition(currView->window, &viewX, &viewY);
        if(currPos==viewX) {windowsHere++; currView->posX=(currPos-gap);} else windowsHere--;
        prevPos=currPos;
    }
    portalW=(currPos-gap);

    // Find Y positions for all this portal's views
    sides.clear(); windowsHere=0; currPos=0; prevPos=0; gap=0;
    for(portView=portal->viewPorts; portView; portView=portView->next){ // for each view
        SDL_GetWindowPosition(portView->window, &viewX, &viewY);
        SDL_GetWindowSize(portView->window, &viewW, &viewH);
        sides[viewY]=portView;
        sides[viewY+viewH]=portView;
    }
    for (it=sides.begin() ; it != sides.end(); it++ ){
        currPos=(*it).first;
        currView=(*it).second;
        if(windowsHere==0) gap+=(currPos-prevPos);
        SDL_GetWindowPosition(currView->window, &viewX, &viewY);
        if(currPos==viewY) {windowsHere++; currView->posY=(currPos-gap);} else windowsHere--;
        prevPos=currPos;
    }
    portalH=(currPos-gap);

    ResizeTurbulancePortal(portal, portalW, portalH);
}

void AddViewToPortal(InfonPortal* portal, const char* title, int x, int y, int w, int h){
    InfonViewPort* VP=new InfonViewPort;
    VP->window=SDL_CreateWindow(title, x,y,w,h, SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
    SDL_SetWindowData(VP->window, "portalView", VP);
    VP->GLContext=SDL_GL_CreateContext(VP->window);
    //TODO: Hardcode: icon, bpp-depth=SDL_PIXELFORMAT_RGB24 / vid_mode
//    SDL_SetWindowDisplayMode(VP->window, NULL); // Mode for use during fullscreen mode
//    LoadIcon()
    VP->renderer=SDL_CreateRenderer(VP->window, -1, SDL_RENDERER_ACCELERATED);// SDL_RENDERER_SOFTWARE);//SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(VP->renderer, 0xA0, 0xA0, 0xf0, 255);
    VP->isMinimized=false;
    VP->isFullScreen=(SDL_GetWindowFlags(VP->window) & SDL_WINDOW_FULLSCREEN);
    VP->posX=0; VP->posY=0; // location in parent window.
    VP->scale=1.0;
    VP->parentPortal=portal;
    VP->next=portal->viewPorts;
    portal->viewPorts=VP;
}

string AppWindowTitle;  // E.g. "Our Game"

bool CreateTurbulancePortal(const char* title, int x, int y, int w, int h){
    if(numPortals >= MAX_PORTALS) return 1;
    char winTitle[1024];
    strcpy(winTitle, title);

    InfonPortal* portal=new InfonPortal;
    portal->isLocked=false;
    AddViewToPortal(portal, title, x,y,w,h);
    ResizeTurbulancePortal(portal, w, h);

    portals[numPortals++]=portal;
    return 0;
}

void DestroyTurbulancePortal(InfonPortal *portal){
    //TODO slide portals items down
    cairo_surface_destroy(portal->cairoSurf);
    SDL_FreeSurface(portal->surface); portal->surface=0;
    for(InfonViewPort *portView=portal->viewPorts; portView; portView=portView->next) // for each view
        SDL_DestroyRenderer(portView->renderer);
}

#include "application.cpp"

/////////////////////////////////////////////////////////////////////
//  EVENT PRODUCER AND CONSUMER
/////////////////////////////////////////////////////////////////////

int secondsToRun=0; //1200; // time until the program exits automatically. 0 = don't exit.
void EXIT(char* errmsg){ERRl(errmsg << "\n"); exit(1);}
void cleanup(void){SDL_Quit();}   //TODO: Add items to cleanup routine

/*
static int SimThread(void *nothing){
    SDL_Event ev; InfonPortal* portal;
    ev.type = SDL_USEREVENT;  ev.user.code = TURB_UPDATE_SURFACE;  ev.user.data1 = 0;  ev.user.data2 = 0;
    while (!doneYet) {
        for (int i = 0; i < numPortals; ++i) {  // Collect and dispatch frames for portals.
            portal=portals[i];
         //   nextFrame=new infon;
         //   theAgent.deepCopy(portal->stuff, nextFrame);
          //  theAgent.normalize(nextFrame);
         //   ev.user.data1 = (void *)i;
         //   ev.user.data2 = nextFrame;
         portal=portal;
            SDL_PushEvent(&ev);
            ++numEvents;
        }
        SDL_Delay(100);
    }
  return 0;
}
*/
void StreamEvents(){
    Uint32 frames=0, then=SDL_GetTicks(), now=0;
    SDL_Thread *simulationThread =0;// SDL_CreateThread(SimThread, "Proteus", NULL);
    SDL_Event ev;
    InfonViewPort *portalView=0; SDL_Window *window=0;
    while (!doneYet){
         ++frames;
        while (SDL_PollEvent(&ev)) {
            window = SDL_GetWindowFromID(ev.key.windowID);
            if(window) portalView=(InfonViewPort*)SDL_GetWindowData(window, "portalView");
            HandleEvent(portalView, window, ev);
        }
        if(doneYet) continue;
        int i=0;{ //for (int i = 0; i < numPortals; ++i) {
            InfonPortal* portal=portals[i];
            if(portal->needsToBeDrawn) portal->needsToBeDrawn=false; else continue;
            if(portal->viewsNeedRefactoring && !portal->isLocked) RefactorPortal(portal);

            DrawScreen(portal);

			SDL_RenderPresent(portal->viewPorts->renderer); //OgrRenderWnd->swapBuffers(false);
        }

		update(portals[0]);
        SDL_Delay(10);
    }
    doneYet=true;
    if ((now = SDL_GetTicks()) > then) printf("\nFrames per second: %2.2f\n", (((double) frames * 1000) / (now - then)));

    SDL_WaitThread(simulationThread, NULL);
}


void InitializePortalSystem(int argc, char** argv){
    numPortals=0;
    for (int i=1; i<argc;) {
        int consumed = 0;
        if (consumed == 0) {
            consumed = -1;
            if (SDL_isdigit(*argv[i])) {secondsToRun = SDL_atoi(argv[i]); consumed = 1;}
        }
        if (consumed < 0) {
            fprintf(stderr, "Usage: %s <seconds to run> [--world <filename>] [--theme <filename>] [--user <username>]\n", argv[0]);
            exit(1);
        }
        i += consumed;
    }

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER) != 0) {MSGl("Couldn't initialize SDL: "<<SDL_GetError()); exit(2);}

    atexit(cleanup);

    AppWindowTitle=MyAppsWindowTitle;
    CreateTurbulancePortal(AppWindowTitle.data(), 150, 100 /*1200*/,1200,700);
}

int main(int argc, char *argv[]){
    MSGl("\n\n         * * * * * Starting Viewer * * * * *\n");
    //MSGl("SDL Revision " << SDL_GetRevisionNumber()<<",  "<<"\n");
    InitializePortalSystem(argc, argv);


    SetUp(portals[0]);
    StreamEvents();
    shutDown();
    return (0);
}
