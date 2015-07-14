/// Bruce Render Engine

#include "Vectors.h"
#include "Matrices.h"


const float DEG2RAD = 3.141593f / 180;
const float EPSILON = 0.00001f;
const double pi = 3.1415926535897;


    const Vector3 ZERO( 0, 0, 0 );
    const Vector3 UNIT_X( 1, 0, 0 );
    const Vector3 UNIT_Y( 0, 1, 0 );
    const Vector3 UNIT_Z( 0, 0, 1 );
    const Vector3 NEGATIVE_UNIT_X( -1,  0,  0 );
    const Vector3 NEGATIVE_UNIT_Y(  0, -1,  0 );
    const Vector3 NEGATIVE_UNIT_Z(  0,  0, -1 );
    const Vector3 UNIT_SCALE(1, 1, 1);

enum Mat3Cells {M3_0_0=0, M3_1_0, M3_2_0,     M3_0_1, M3_1_1, M3_2_1,     M3_0_2, M3_1_2, M3_2_2};
enum Mat4Cells {M4_0_0=0, M4_1_0, M4_2_0, M4_3_0,     M4_0_1, M4_1_1, M4_2_1, M4_3_1,     M4_0_2, M4_1_2, M4_2_2, M4_3_2,     M4_0_3, M4_1_3, M4_2_3, M4_3_3,};

struct Quaternion{
    scalar w,x,y,z;
    Quaternion(scalar W=1, scalar X=0, scalar Y=0, scalar Z=0):w(W), x(X), y(Y), z(Z){};
    void set(scalar W=1, scalar X=0, scalar Y=0, scalar Z=0){w=W; x=X; y=Y; z=Z;}
    void ToRotationMatrix (Matrix3& kRot) const{
        scalar fTx  = x+x;
        scalar fTy  = y+y;
        scalar fTz  = z+z;
        scalar fTwx = fTx*w;
        scalar fTwy = fTy*w;
        scalar fTwz = fTz*w;
        scalar fTxx = fTx*x;
        scalar fTxy = fTy*x;
        scalar fTxz = fTz*x;
        scalar fTyy = fTy*y;
        scalar fTyz = fTz*y;
        scalar fTzz = fTz*z;

        kRot[M3_0_0] = 1.0f-(fTyy+fTzz);
        kRot[M3_0_1] = fTxy-fTwz;
        kRot[M3_0_2] = fTxz+fTwy;
        kRot[M3_1_0] = fTxy+fTwz;
        kRot[M3_1_1] = 1.0f-(fTxx+fTzz);
        kRot[M3_1_2] = fTyz-fTwx;
        kRot[M3_2_0] = fTxz-fTwy;
        kRot[M3_2_1] = fTyz+fTwx;
        kRot[M3_2_2] = 1.0f-(fTxx+fTyy);
    }
    void FromAngleAxis (const scalar& rfAngle, const Vector3& rkAxis) {
        // assert:  axis[] is unit length
        //
        // The quaternion representing the rotation is
        //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

        scalar fHalfAngle ( 0.5*rfAngle );
        scalar fSin = sin(fHalfAngle);
        w = cos(fHalfAngle);
        x = fSin*rkAxis.x;
        y = fSin*rkAxis.y;
        z = fSin*rkAxis.z;
    }
    inline bool operator== (const Quaternion& rhs) const {return (rhs.x == x) && (rhs.y == y) && (rhs.z == z) && (rhs.w == w);};
    inline bool operator!= (const Quaternion& rhs) const {return !operator==(rhs);};
    Quaternion operator+ (const Quaternion& rkQ) const {return Quaternion(w+rkQ.w,x+rkQ.x,y+rkQ.y,z+rkQ.z);};
    Quaternion operator- (const Quaternion& rkQ) const {return Quaternion(-w,-x,-y,-z);};
    Quaternion operator* (scalar fScalar) const{return Quaternion(fScalar*w,fScalar*x,fScalar*y,fScalar*z);};

    scalar Dot (const Quaternion& rkQ) const {return w*rkQ.w+x*rkQ.x+y*rkQ.y+z*rkQ.z;};
    Quaternion operator* (const Quaternion& rkQ) const { // NOTE:  p*q != q*p
        return Quaternion(
            w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z,
            w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
            w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
            w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x
        );};

    scalar Norm () const {return w*w+x*x+y*y+z*z;};
    scalar normalize(void){ // normalize and return previous length
        scalar len = Norm();
        scalar factor = 1.0f / sqrt(len);
        *this = *this * factor;
        return len;
    }
    Vector3 operator* (const Vector3& v) const{
		// nVidia SDK implementation
		Vector3 uv, uuv;
		Vector3 qvec(x, y, z);
		uv = qvec.cross(v);
		uuv = qvec.cross(uv);
		uv *= (2.0f * w);
		uuv *= 2.0f;

		return v + uv + uuv;

    }
};
   Quaternion operator* (scalar fScalar, const Quaternion& rkQ){return Quaternion(fScalar*rkQ.w, fScalar*rkQ.x, fScalar*rkQ.y, fScalar*rkQ.z);};

struct thing; // Forward

struct color{
    int r,g,b; // The color which is not absorbed.
    int reflectPercent, transmitPercent;
    color(int red=1, int green=1, int blue=1):r(red), g(green), b(blue){};
    void randomize(){r=rand()%256; g=rand()%256; b=rand()%256;};
};

float fRand(float max){
    return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/max));
}

Vector4 randVec4(float max){
    return Vector4(fRand(max*2)-1, fRand(max*2)-max, fRand(max*2)-max, 1);
};

struct plane{
    Vector3 normal;
    scalar dist;
    plane():normal(0,0,0), dist(0) {};
    plane(const plane& rhs): normal(rhs.normal), dist(rhs.dist) {}
    plane(const Vector3& Normal, scalar D): normal(Normal), dist(D){};
    plane(scalar A, scalar B, scalar C, scalar D):normal(A,B,C), dist(D) {};
};

    Matrix4 makeViewMatrix(const Vector4& Position, const Quaternion& Orientation, int scale=1){ // From Ogre::Math
		Matrix4 viewMatrix;

		// View matrix is:
		//
		//  [ Lx  Uy  Dz  Tx  ]
		//  [ Lx  Uy  Dz  Ty  ]
		//  [ Lx  Uy  Dz  Tz  ]
		//  [ 0   0   0   1   ]
		//
		// Where T = -(Transposed(Rot) * Pos)

		Matrix3 rot;
		Orientation.ToRotationMatrix(rot);

		// Make the translation relative to new axes
		Matrix3 rotT = rot; rotT.transpose();
        Vector3 pos3(Position.x, Position.y, Position.z);
		Vector3 trans = -(rotT * pos3);

		// Make final matrix
		viewMatrix.identity();
		viewMatrix[M4_0_0] = rotT[M3_0_0]*scale; viewMatrix[M4_0_1] = rotT[M3_0_1]; viewMatrix[M4_0_2] = rotT[M3_0_2];
        viewMatrix[M4_1_0] = rotT[M3_1_0]; viewMatrix[M4_1_1] = rotT[M3_1_1]*scale; viewMatrix[M4_1_2] = rotT[M3_1_2];
        viewMatrix[M4_2_0] = rotT[M3_2_0]; viewMatrix[M4_2_1] = rotT[M3_2_1]; viewMatrix[M4_2_2] = rotT[M3_2_2]*scale;

		viewMatrix[M4_0_3] = trans.x;
		viewMatrix[M4_1_3] = trans.y;
		viewMatrix[M4_2_3] = trans.z;

        return viewMatrix;
	};

enum FrustumPlane  {
    FRUSTUM_PLANE_NEAR   = 0,
    FRUSTUM_PLANE_FAR    = 1,
    FRUSTUM_PLANE_LEFT   = 2,
    FRUSTUM_PLANE_RIGHT  = 3,
    FRUSTUM_PLANE_TOP    = 4,
    FRUSTUM_PLANE_BOTTOM = 5
};

struct frustum{
    Matrix4 mProjMatrix, mViewMatrix;
    scalar FovY; scalar Aspect; scalar Front; scalar Back;
    plane mFrustumPlanes[6];
    Vector4 position;
    Quaternion orientation;  //position: center of the viewPort
    void calcViewMatrix(){mViewMatrix=makeViewMatrix(position, orientation);};
    ///////////////////////////////////////////////////////////////////////////////
    // glFrustum()
    ///////////////////////////////////////////////////////////////////////////////
    void setFrustum(scalar left, scalar right, scalar bottom, scalar top, scalar near, scalar far) {
        Matrix4 &mat=mProjMatrix;
        mat[0]  = 2 * near / (right - left);
        mat[5]  = 2 * near / (top - bottom);
        mat[8]  = (right + left) / (right - left);
        mat[9]  = (top + bottom) / (top - bottom);
        mat[10] = -(far + near) / (far - near);
        mat[11] = -1;
        mat[14] = -(2 * far * near) / (far - near);
        mat[15] = 0;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // gluPerspective()
    ///////////////////////////////////////////////////////////////////////////////
    void setFrustum(float fovY, float aspect, float front, float back) {
        FovY=fovY; Aspect=aspect; Front=front; Back=back;
        float tangent = tanf(fovY/2 * DEG2RAD); // tangent of half fovY
        float height = front * tangent;         // half height of near plane
        float width = height * aspect;          // half width of near plane

        // params: left, right, bottom, top, near, far
        setFrustum(-width, width, -height, height, front, back);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // glOrtho()
    ///////////////////////////////////////////////////////////////////////////////
    void setOrthoFrustum(scalar l, scalar r, scalar b, scalar t, scalar n, scalar f) {
        Matrix4 &mat=mProjMatrix;
        mat[0]  = 2 / (r - l);
        mat[5]  = 2 / (t - b);
        mat[10] = -2 / (f - n);
        mat[12] = -(r + l) / (r - l);
        mat[13] = -(t + b) / (t - b);
        mat[14] = -(f + n) / (f - n);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Update Frustum planes   (From Ogre)
    ///////////////////////////////////////////////////////////////////////////////
    void updateFrustumPlanesImpl(void) {
		Matrix4 combo = mProjMatrix * mViewMatrix;

		mFrustumPlanes[FRUSTUM_PLANE_LEFT].normal.x = combo[M4_3_0] + combo[M4_0_0];
		mFrustumPlanes[FRUSTUM_PLANE_LEFT].normal.y = combo[M4_3_1] + combo[M4_0_1];
		mFrustumPlanes[FRUSTUM_PLANE_LEFT].normal.z = combo[M4_3_2] + combo[M4_0_2];
		mFrustumPlanes[FRUSTUM_PLANE_LEFT].dist = combo[M4_3_3] + combo[M4_0_3];

		mFrustumPlanes[FRUSTUM_PLANE_RIGHT].normal.x = combo[M4_3_0] - combo[M4_0_0];
		mFrustumPlanes[FRUSTUM_PLANE_RIGHT].normal.y = combo[M4_3_1] - combo[M4_0_1];
		mFrustumPlanes[FRUSTUM_PLANE_RIGHT].normal.z = combo[M4_3_2] - combo[M4_0_2];
		mFrustumPlanes[FRUSTUM_PLANE_RIGHT].dist = combo[M4_3_3] - combo[M4_0_3];

		mFrustumPlanes[FRUSTUM_PLANE_TOP].normal.x = combo[M4_3_0] - combo[M4_1_0];
		mFrustumPlanes[FRUSTUM_PLANE_TOP].normal.y = combo[M4_3_1] - combo[M4_1_1];
		mFrustumPlanes[FRUSTUM_PLANE_TOP].normal.z = combo[M4_3_2] - combo[M4_1_2];
		mFrustumPlanes[FRUSTUM_PLANE_TOP].dist = combo[M4_3_3] - combo[M4_1_3];

		mFrustumPlanes[FRUSTUM_PLANE_BOTTOM].normal.x = combo[M4_3_0] + combo[M4_1_0];
		mFrustumPlanes[FRUSTUM_PLANE_BOTTOM].normal.y = combo[M4_3_1] + combo[M4_1_1];
		mFrustumPlanes[FRUSTUM_PLANE_BOTTOM].normal.z = combo[M4_3_2] + combo[M4_1_2];
		mFrustumPlanes[FRUSTUM_PLANE_BOTTOM].dist = combo[M4_3_3] + combo[M4_1_3];

		mFrustumPlanes[FRUSTUM_PLANE_NEAR].normal.x = combo[M4_3_0] + combo[M4_2_0];
		mFrustumPlanes[FRUSTUM_PLANE_NEAR].normal.y = combo[M4_3_1] + combo[M4_2_1];
		mFrustumPlanes[FRUSTUM_PLANE_NEAR].normal.z = combo[M4_3_2] + combo[M4_2_2];
		mFrustumPlanes[FRUSTUM_PLANE_NEAR].dist = combo[M4_3_3] + combo[M4_2_3];

		mFrustumPlanes[FRUSTUM_PLANE_FAR].normal.x = combo[M4_3_0] - combo[M4_2_0];
		mFrustumPlanes[FRUSTUM_PLANE_FAR].normal.y = combo[M4_3_1] - combo[M4_2_1];
		mFrustumPlanes[FRUSTUM_PLANE_FAR].normal.z = combo[M4_3_2] - combo[M4_2_2];
		mFrustumPlanes[FRUSTUM_PLANE_FAR].dist = combo[M4_3_3] - combo[M4_2_3];

		// Renormalise any normals which were not unit length
		for(int i=0; i<6; i++ ) {
			scalar length = mFrustumPlanes[i].normal.length();
            mFrustumPlanes[i].normal.normalize();
			mFrustumPlanes[i].dist /= length;
		}
	}

    void move(const Vector3& vec){position.x+=vec.x;  position.y+=vec.y; position.z+=vec.z; calcViewMatrix();};
    void rotate(const Vector3& axis, const scalar& angle){Quaternion q; q.FromAngleAxis(angle,axis); rotate(q);};
    void rotate(const Quaternion& q){Quaternion qnorm = q; qnorm.normalize(); orientation = qnorm * orientation;};

    void lookAt( const Vector3& targetPoint );
    void lookAt(scalar x, scalar y, scalar z);

    void pitch(const scalar& angle)  {Vector3 axis = orientation * UNIT_X; rotate(axis, angle);};
    void yaw(const scalar& angle)    {Vector3 axis = orientation * UNIT_Y; rotate(axis, angle);};
    void roll(const scalar& angle)   {Vector3 axis = orientation * UNIT_Z; rotate(axis, angle);};
};

struct frustIterator{
    thing* crntThing;
    frustIterator(frustum* frust=0){};
    bool fetchNext(){return 0;};
};

int RenderW,RenderH;
struct thing{
    Vector4 position, extent;    // position is one corner and extent is far corner of non-axis aligned bounding-box.
    Quaternion orientation;
    scalar size;
    color Color;
    list<Vector4> pointsToConnect;
    void asTriangle(){
      //  pointsToConnect.push_back(Vector3(0,0,0));
        pointsToConnect.push_back(Vector4(.1,.7,.8,1));
        pointsToConnect.push_back(Vector4(.30,.40,.50,1));
        pointsToConnect.push_back(Vector4(1.5,1.6,.10,1));
        pointsToConnect.push_back(Vector4(0,0,0,1));
    };
    void asCube(){
      //  pointsToConnect.push_back(Vector3(0,0,0));
        pointsToConnect.push_back(Vector4(0,1,0,1));
        pointsToConnect.push_back(Vector4(0,1,1,1));
        pointsToConnect.push_back(Vector4(0,0,1,1));
        pointsToConnect.push_back(Vector4(0,0,0,1));

        pointsToConnect.push_back(Vector4(1,0,0,1));
        pointsToConnect.push_back(Vector4(1,0,1,1));
        pointsToConnect.push_back(Vector4(1,1,1,1));
        pointsToConnect.push_back(Vector4(1,1,0,1));
        pointsToConnect.push_back(Vector4(1,0,0,1));

        pointsToConnect.push_back(Vector4(1,1,0,1));
        pointsToConnect.push_back(Vector4(0,1,0,1));
        pointsToConnect.push_back(Vector4(0,1,1,1));
        pointsToConnect.push_back(Vector4(1,1,1,1));
        pointsToConnect.push_back(Vector4(1,0,1,1));
        pointsToConnect.push_back(Vector4(0,0,1,1));
    };

    void draw(SDL_Renderer* r, Matrix4& toScreen){
        int W,H; W=RenderW/50; H=RenderH/50;
        SDL_SetRenderDrawColor(r, Color.r, Color.g, Color.b,255);
        Matrix4 ModMat=makeViewMatrix(position, orientation, size);  //ModMat.identity();
 //       printf("POS: [%f, %f, %f] ", position.x, position.y, position.z);
 //       cout<<"\n"<<ModMat<<"\n";
        Matrix4 xMat= toScreen * ModMat;
        Vector4 fromPoint= xMat * Vector4(0,0,0,1);
        fromPoint.x /= fromPoint.w; fromPoint.y /= fromPoint.w; fromPoint.z /= fromPoint.w;
        fromPoint.x+=RenderW/2;fromPoint.y+=RenderH/2; //fromPoint.x*=W;fromPoint.y*=H;
        scalar S=1;
        for(Vector4 &P:pointsToConnect){
            Vector4 toPoint = xMat *  P;// toScreen * ModMat * P; //xMat;
            toPoint.x /= toPoint.w; toPoint.y /= toPoint.w; toPoint.z /= toPoint.w;
            toPoint.x+=RenderW/2;  toPoint.y+=RenderH/2;  //toPoint.x*=W; toPoint.y*=H;
            if(toPoint.z <= 1 && toPoint.z >= -1)
                SDL_RenderDrawLine(r, fromPoint.x*S,fromPoint.y*S, toPoint.x*S, toPoint.y*S);
 //           printf("X1:%f \tY1:%f \tX3:%f \tY2:%f \n", fromPoint.x*S,fromPoint.y*S, toPoint.x*S, toPoint.y*S);
            fromPoint=toPoint;
        }
    };
 //   list<thing*> attachments;
 //   thing* parent;
 //   frustIterator FrustItr;
};

struct scene{
    color background;
    list<thing*> things;
    frustIterator FrustItr;
};

struct camera{
    scene* Scene;
    frustum view;
    Matrix4 ViewProjMatrix;

    void render(SDL_Renderer* r);
};

void camera::render(SDL_Renderer* r){
    SDL_SetRenderDrawColor(r, 0,0,20,255);
    SDL_RenderClear(r);
    ViewProjMatrix= view.mProjMatrix * view.mViewMatrix;
 //   cout<<"\nProjMatrix\n"<<view.mProjMatrix<<"\n\nViewMatrix:\n"<<view.mViewMatrix<<"\n";
    for(auto thing:Scene->things){
        thing->draw(r, ViewProjMatrix);
    }
    SDL_SetRenderDrawColor(r, 20,20,200,255);
SDL_RenderDrawPoint(r,500,500);
//SDL_RenderDrawPoints(SDL_Renderer* renderer,  const SDL_Point* points,  int count)
    SDL_RenderDrawLine(r, 10,10,300,400);
}

