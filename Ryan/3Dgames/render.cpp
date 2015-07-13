
struct vector3D{
    int64_t x,y,z;
};
struct color{
    int r,g,b; // The color which is not absorbed.
    int reflectPercent, transmitPercent;
    color(int red, int green, int blue):r(red), g(green), b(blue){};
};

struct frustrum{
    int spread;   // 10000=no spread.  20000=one unit out means 2 units of spread.
    vector3D location, extent, orientation;  //location: center of the viewPort, extent: size of viewPort + farCullDistance, orientation: direction + roll.
};

struct thing{
    vector3D location, extent, orientation;    // location is one corner and extent is far corner of non-axis aligned bounding-box.
    color Color;
    list<thing*> attachments;
    thing* parent;
    void iterateItemsInFrustrum();
};

struct scene{
    color background;
    list<thing*> things;
};


struct camera{
    scene Scene;
    frustrum view;

    void render();

};

void camera::render(){
    // for each thing in frustrum, from near to far:
    //  find its footprint in viewPort and record color expression.
    //  Remove that footprint from active frustrum viewPort.

    // render all the expressions.. Non-expressed pixels get scene's background.
}
