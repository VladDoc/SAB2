#include <cmath>
#include <random>
#include <chrono>

#include "matrix.hpp"
#include "vector.hpp"
#include "../entities/camera.hpp"
#include "maths.hpp"
#include "../engineTester/main.hpp"
#include "../renderEngine/masterrenderer.hpp"

std::mt19937 Maths::generatorUniform(0);
std::uniform_real_distribution<float> Maths::distributionUniform(0.0f, 1.0f);

std::default_random_engine Maths::generatorNormal(0);
std::normal_distribution<float> Maths::distributionNormal(0.0f, 1.0f);

/*
** Blank Matrix4f should have already been created. This function does not allocate any memory to matrix.
*/
void Maths::createTransformationMatrix(Matrix4f* matrix, Vector3f* translation, float rx, float ry, float rz, float rs, float scale)
{
    matrix->setIdentity();
    matrix->translate(translation);

    matrix->rotate(toRadians(ry), &Y_AXIS);
    matrix->rotate(toRadians(rz), &Z_AXIS);
    matrix->rotate(toRadians(rx), &X_AXIS);
    matrix->rotate(toRadians(rs), &Z_AXIS);

    matrix->scale(scale);
}

void Maths::createTransformationMatrix(Matrix4f* matrix, Vector3f* translation, float rx, float ry, float rz, float rs, float scaleX, float scaleY, float scaleZ)
{
    matrix->setIdentity();
    matrix->translate(translation);

    matrix->rotate(toRadians(ry), &Y_AXIS);
    matrix->rotate(toRadians(rz), &Z_AXIS);
    matrix->rotate(toRadians(rx), &X_AXIS);
    matrix->rotate(toRadians(rs), &Z_AXIS);

    Vector3f vec(scaleX, scaleY, scaleZ);
    matrix->scale(&vec);
}

void Maths::createTransformationMatrixYXZY(Matrix4f* matrix, Vector3f* translation, float rx, float ry, float rz, float rs, float scale)
{
    matrix->setIdentity();
    matrix->translate(translation);

    matrix->rotate(toRadians(ry), &Y_AXIS);
    matrix->rotate(toRadians(rz), &Z_AXIS);
    matrix->rotate(toRadians(rx), &X_AXIS);
    matrix->rotate(toRadians(rs), &Y_AXIS);

    matrix->scale(scale);
}

void Maths::createTransformationMatrix(Matrix4f* result, Vector2f* translation, float rotation, Vector2f* scale)
{
    result->setIdentity();
    result->translate(translation);
    Vector3f axis(0, 0, 1);
    result->rotate(toRadians(rotation), &axis);
    result->scale(scale);
}

//Based on how sadx calculates rotations
void Maths::createTransformationMatrixYXZ(Matrix4f* matrix, Vector3f* translation, float rx, float ry, float rz, float scale)
{
    matrix->setIdentity();
    matrix->translate(translation);

    matrix->rotate(toRadians(rz), &Z_AXIS);
    matrix->rotate(toRadians(rx), &X_AXIS);
    matrix->rotate(toRadians(ry), &Y_AXIS);

    matrix->scale(scale);
}


void Maths::createViewMatrix(Matrix4f* matrix, Camera* cam)
{
    Matrix4f viewMatrix = Maths::lookAt(&cam->eye, &cam->target, &cam->up);
    
    matrix->set(&viewMatrix);
}

Matrix4f Maths::lookAt(Vector3f* eye, Vector3f* focus, Vector3f* up)
{
    Vector3f f(focus);
    f = f-eye;
    f.normalize();

    Vector3f s = f.cross(up);
    s.normalize();

    Vector3f u = s.cross(&f);

    Matrix4f result;

    result.m00 =  s.x;
    result.m10 =  s.y;
    result.m20 =  s.z;
    result.m01 =  u.x;
    result.m11 =  u.y;
    result.m21 =  u.z;
    result.m02 = -f.x;
    result.m12 = -f.y;
    result.m22 = -f.z;
    result.m30 = -s.dot(eye);
    result.m31 = -u.dot(eye);
    result.m32 =  f.dot(eye);

    return result;
}

//Given two vectors that are perpencicular to each other, calculate
// a third vector that is perpendicular to both
Vector3f Maths::calcThirdAxis(Vector3f* A, Vector3f* B)
{
    //float third[3];
    //rotatePoint(third, 0, 0, 0, A->x, A->y, A->z, B->x, B->y, B->z, -(M_PI/2));

    //return Vector3f(third[0], third[1], third[2]);

    return B->cross(A);
}

//Given a basis {v1, v2, v3}, computes the coordinate vector
// of a point a in the new basis
Vector3f Maths::coordinatesRelativeToBasis(Vector3f* v1, Vector3f* v2, Vector3f* v3, Vector3f* a)
{
    // Set up the augmented matrix
    float A[3][4] = {{v1->x, v2->x, v3->x, a->x},
                     {v1->y, v2->y, v3->y, a->y},
                     {v1->z, v2->z, v3->z, a->z}};

    const int nrows = 3; // number of rows
    const int ncols = 4; // number of columns

    int lead = 0; 

    // Row reduce
    while (lead < nrows)
    {
        float d, m;

        for (int r = 0; r < nrows; r++)
        {
            d = A[lead][lead];
            m = A[r][lead] / A[lead][lead];

            for (int c = 0; c < ncols; c++)
            {
                if (r == lead)
                {
                    A[r][c] /= d;
                }
                else
                {
                    A[r][c] -= A[lead][c] * m;
                }
            }
        }

        lead++;
    }

    return Vector3f(A[0][3], A[1][3], A[2][3]);
}

//Returns the difference between the two angles
//ang1 and ang2 should be in degrees
float Maths::compareTwoAngles(float origAng1, float origAng2)
{
    float ang1 = fmodf(fmodf(origAng1, 360) + 360, 360);
    float ang2 = fmodf(fmodf(origAng2, 360) + 360, 360);

    float d = fmodf(std::abs(ang1 - ang2), 360);
    float r = d > 180 ? 360 - d : d;

    //calculate sign
    int sign = (ang1 - ang2 >= 0 && ang1 - ang2 <= 180) || (ang1 - ang2 <= -180 && ang1 - ang2 >= -360) ? 1 : -1;
    r *= sign;

    return r;
}

float Maths::interpolate(float a, float b, float percent)
{
    return (a*(1.0f - percent)) + (b*percent);
}

int Maths::sign(float value)
{
    //int v = *(int*)&value; //get bits of value casted as an int

    //return 1 - 2*((v>>31) & 0x01);

    if (value > 0)
    {
        return 1;
    }
    else if (value < 0)
    {
        return -1;
    }
    return 0;
}

//clamps value between lower and higher
float Maths::clamp(float lower, float value, float higher)
{
    return fmaxf(lower, fminf(value, higher));
}

//clamps value between lower and higher
int Maths::clamp(int lower, int value, int higher)
{
    return std::max(lower, std::min(value, higher));
}

unsigned int Maths::getSignBit(float value)
{
    void* vp = (void*)(&value);
    unsigned int v = *((unsigned int*)vp); //get bits of value casted as an int

    return v & 0b10000000000000000000000000000000;
}

int Maths::numDigits(int number)
{
    int numChars = (int)ceil(log10f((float)(number+1)));
    if (number == 0)
    {
        numChars = 1;
    }
    return numChars;
}

float Maths::approach(float initialValue, float terminalValue, float approachConstant, float timePassed)
{
    //return ((initialValue-terminalValue)*powf(Maths::E, -approachConstant*timePassed) + terminalValue);

    float answer1 = ((initialValue-terminalValue)*powf(Maths::E, -approachConstant*timePassed) + terminalValue);

    //float A = approachConstant*2;
    //float T = terminalValue;
    //float v0 = initialValue;
    //float dt = timePassed;
    //float e = Maths::E;
    //
    //float posDelta = ((powf(e, -A*dt)*(T - v0)) + A*T*dt + v0 - T)/A;
    //float answer2 = posDelta/timePassed;

    //float A = approachConstant;
    //float T = terminalValue;
    //float v0 = initialValue;
    //float dt = timePassed;
    //float e = Maths::E;
    //
    //float pos1 = (powf(e, -A*0)*(A*0*T*powf(e, A*0) - v0 + T))/A;
    //float pos2 = (powf(e, -A*dt)*(A*dt*T*powf(e, A*dt) - v0 + T))/A;
    //
    //float answer3 = (pos2-pos1)/timePassed;

    //printf("diff = %f\n", (answer2-answer1));
    return answer1;
}

float Maths::positionDeltaFromApproach(float initialVelValue, float terminalVelValue, float approachConstant, float timePassed)
{
    float A = approachConstant*2; //not sure why i needed to add this
    float T = terminalVelValue;
    float v0 = initialVelValue;
    float dt = timePassed;
    float e = Maths::E;

    return ((powf(e, -A*dt)*(T - v0)) + A*T*dt + v0 - T)/A;
}

Vector3f Maths::applyDrag(Vector3f* velocity, float drag, float deltaTime)
{
    float length = velocity->length();
    if (length < 0.0001f)
    {
        return Vector3f(velocity);
    }
    float newLength = length*powf(Maths::E, drag*deltaTime);
    return velocity->scaleCopy(newLength/length);
}

float Maths::applyDrag(float velocity, float drag, float deltaTime)
{
    float length = std::abs(velocity);
    if (length < 0.0001f)
    {
        return velocity;
    }
    float newLength = length*powf(Maths::E, drag*deltaTime);
    return velocity*(newLength/length);
}

/**
* Calculate the x and z speeds relative to a plane based off
* the previous position you are coming in from
*
* @param xspd the x speed that you are going at before collision
* @param yspd the y speed that you are going at before collision
* @param zspd the z speed that you are going at before collision
* @param A the collision point on the triangle
* @param normal the normal of the triangle
*/
Vector3f Maths::calculatePlaneSpeed(float xspd, float yspd, float zspd, Vector3f* normal)
{
    Vector3f A(xspd, yspd, zspd);
    Vector3f Blue = projectOntoPlane(&A, normal);

    //Rotate normal along y axis 90 degrees
    float CDir = atan2f(-normal->z, -normal->x);
    CDir += (Maths::PI / 2);
    float Cx = cosf(CDir);
    float Cz = sinf(CDir);

    float CDist = sqrtf(normal->x*normal->x + normal->z*normal->z);
    float CPitch = ((Maths::PI / 2) + atan2f(-normal->y, CDist));

    float result[3] = { 0, 0, 0 }; //storage for the answer
    rotatePoint(result, Cx, 0, Cz, Blue.x, Blue.y, Blue.z, -CPitch);

    return Vector3f(result[0], result[1], result[2]);
}

//Equation from https://sites.google.com/site/glennmurray/Home/rotation-matrices-and-formulas
//Point that axis goes through,
//direction of axis,
//point to rotate, 
//angle of rotation
void Maths::rotatePoint(float result[],
    const float a, const float b, const float c,
    const float u, const float v, const float w,
    const float x, const float y, const float z,
    const float theta)
{
    //if (sqrtf(u*u + v*v + w*w) < 0.000000001f)
    if (u*u + v*v + w*w < 1e-18f)
    {
        //printf("Warning: trying to rotate by a very small axis [%f %f %f]\n", u, v, w);
        result[0] = x;
        result[1] = y;
        result[2] = z;
        return;
    }

    const float u2 = u*u;
    const float v2 = v*v;
    const float w2 = w*w;
    const float l2 = u2 + v2 + w2;
    const float l = sqrtf(l2);

    const float cosT = cosf(theta);
    const float oneMinusCosT = 1 - cosT;
    const float sinT = sinf(theta);

    result[0] = ((a*(v2 + w2) - u*(b*v + c*w - u*x - v*y - w*z)) * oneMinusCosT
        + l2*x*cosT
        + l*(-c*v + b*w - w*y + v*z)*sinT) / l2;

    result[1] = ((b*(u2 + w2) - v*(a*u + c*w - u*x - v*y - w*z)) * oneMinusCosT
        + l2*y*cosT
        + l*(c*u - a*w + w*x - u*z)*sinT) / l2;

    result[2] = ((c*(u2 + v2) - w*(a*u + b*v - u*x - v*y - w*z)) * oneMinusCosT
        + l2*z*cosT
        + l*(-b*u + a*v - v*x + u*y)*sinT) / l2;
}

//Equation from https://sites.google.com/site/glennmurray/Home/rotation-matrices-and-formulas
//direction of axis,
//point to rotate, 
//angle of rotation
void Maths::rotatePoint(float result[],
    const float u, const float v, const float w,
    const float x, const float y, const float z,
    const float theta)
{
    if (u * u + v * v + w * w < 1e-18f)
    {
        result[0] = x;
        result[1] = y;
        result[2] = z;
        return;
    }

    const float u2 = u * u;
    const float v2 = v * v;
    const float w2 = w * w;
    const float l2 = u2 + v2 + w2;
    const float l = sqrtf(l2);

    const float cosT = cosf(theta);
    const float oneMinusCosT = 1 - cosT;
    const float sinT = sinf(theta);

    result[0] = ((- u * (- u * x - v * y - w * z)) * oneMinusCosT
        + l2 * x * cosT
        + l * (- w * y + v * z) * sinT) / l2;

    result[1] = ((- v * (- u * x - v * y - w * z)) * oneMinusCosT
        + l2 * y * cosT
        + l * (  w * x - u * z) * sinT) / l2;

    result[2] = ((- w * (- u * x - v * y - w * z)) * oneMinusCosT
        + l2 * z * cosT
        + l * (- v * x + u * y) * sinT) / l2;
}

//Point to rotate
//direction of axis,
//angle of rotation, in radians
Vector3f Maths::rotatePoint(
    const Vector3f* pointToRotate,
    const Vector3f* axisOfRotation,
    const float theta)
{
    float result[3];
    Maths::rotatePoint(result,
        axisOfRotation->x, 
        axisOfRotation->y, 
        axisOfRotation->z, 
        pointToRotate->x, 
        pointToRotate->y, 
        pointToRotate->z, 
        theta);

    return Vector3f(result[0], result[1], result[2]);
}

Vector3f Maths::interpolateVector(const Vector3f* A, const Vector3f* B, const float percent)
{
    Vector3f perpen = A->cross(B);
    float dotProduct = A->dot(B);
    float mag = A->length()*B->length();

    if (mag < 0.0000001f)
    {
        //printf("Warning: Trying to interpolate between small vectors\n");
        return Vector3f(A);
    }

    float similatiry = dotProduct/mag;

    if (similatiry > 0.9999999f) //Vectors are extremely similar already, just return A
    {
        return Vector3f(A);
    }

    float angle = acos(similatiry);
    float perc = fminf(1.0f, fmaxf(0.0f, percent));
    return Maths::rotatePoint(A, &perpen, angle*perc);
}

Vector3f Maths::interpolateVectorDebug(Vector3f* A, Vector3f* B, float percent)
{
    Vector3f perpen = A->cross(B);
    float dotProduct = A->dot(B);
    float mag = A->length()*B->length();

    if (mag < 0.0000001f)
    {
        printf("1\n");
        return Vector3f(A);
    }

    if (dotProduct/mag > 0.9999999f)
    {
        printf("2\n");
        return Vector3f(A);
    }

    float angle = acos(dotProduct/mag);
    percent = fminf(1.0f, fmaxf(0.0f, percent));
    printf("percent=%f\n", percent);
    return Maths::rotatePoint(A, &perpen, angle*percent);
}

float Maths::angleBetweenVectors(const Vector3f* A, const Vector3f* B)
{
    float mag = A->length()*B->length();

    if (mag < 0.0000001f)
    {
        return 0;
    }

    Vector3f A2(A);
    Vector3f B2(B);
    A2.normalize();
    B2.normalize();

    float dotProduct = A2.dot(&B2);

    if (dotProduct >= 1.0f) //Vectors are extremely similar, return 0
    {
        return 0;
    }
    else if (dotProduct <= -1.0f) //Vectors are opposite direction, return pi
    {
        return Maths::PI;
    }

    return acosf(dotProduct);
}

Vector3f Maths::getCloserPoint(Vector3f* A, Vector3f* B, Vector3f* testPoint)
{
    Vector3f deltaA(A);
    deltaA = deltaA - testPoint;
    Vector3f deltaB(B);
    deltaB = deltaB - testPoint;
    float distA = deltaA.lengthSquared();
    float distB = deltaB.lengthSquared();
    if (distA < distB)
    {
        return Vector3f(A);
    }
    return Vector3f(B);
}

//https://stackoverflow.com/questions/11132681/what-is-a-formula-to-get-a-vector-perpendicular-to-another-vector
Vector3f Maths::calculatePerpendicular(const Vector3f* vec)
{
    bool b0 = (vec->x <  vec->y) && (vec->x <  vec->z);
    bool b1 = (vec->y <= vec->x) && (vec->y <  vec->z);
    bool b2 = (vec->z <= vec->x) && (vec->z <= vec->y);
    Vector3f differentVec((float)(b0), (float)(b1), (float)(b2));
    differentVec.setLength(1);
    Vector3f perpen = vec->cross(&differentVec);
    perpen.setLength(1);
    return perpen;
}

Vector3f Maths::projectAlongLine(const Vector3f* A, const Vector3f* line)
{
    Vector3f master(A);
    //printf("master = %f %f %f\n", master.x, master.y, master.z);
    Vector3f perpen1 = Maths::calculatePerpendicular(line);
    perpen1.normalize();
    //printf("perpen1 = %f %f %f\n", perpen1.x, perpen1.y, perpen1.z);
    Vector3f perpen2 = perpen1.cross(line);
    perpen2.normalize();
    //printf("perpen2 = %f %f %f\n", perpen2.x, perpen2.y, perpen2.z);
    master = Maths::projectOntoPlane(&master, &perpen1);
    //printf("master = %f %f %f\n", master.x, master.y, master.z);
    master = Maths::projectOntoPlane(&master, &perpen2);
    //printf("master = %f %f %f\n", master.x, master.y, master.z);
    return master;
}

/**
* @param initialVelocity
* @param surfaceNormal
* @param elasticity Scale of the resulting vector relative to the original velocity
*/
Vector3f Maths::bounceVector(Vector3f* initialVelocity, Vector3f* surfaceNormal, float elasticity)
{
    Vector3f twoNtimesVdotN(surfaceNormal);
    twoNtimesVdotN.scale(-2 * initialVelocity->dot(surfaceNormal));

    Vector3f Vnew = (twoNtimesVdotN + initialVelocity);
    Vnew.scale(elasticity);

    return Vnew;
}


//Projects vector A to be perpendicular to vector normal
Vector3f Maths::projectOntoPlane(const Vector3f* A, const Vector3f* normal)
{
    Vector3f B(0, 0, 0);
    Vector3f C(A);
    Vector3f N(normal->x, normal->y, normal->z);
    N.normalize();

    N.scale(C.dot(&N));
    B = C - N;

    return B;
}

/** Returns the point on a sphere that has the given angles from the center
* @param angH in radians
* @param angV in radians
* @param radius
* @return
*/
Vector3f Maths::spherePositionFromAngles(float angH, float angV, float radius)
{
    float y   = (radius*sinf(angV));
    float hpt = (radius*cosf(angV));

    float x = (hpt*cosf(angH));
    float z = (hpt*sinf(angH));

    return Vector3f(x, y, z);
}

void Maths::sphereAnglesFromPosition(Vector3f* dir, float* outAngY, float* outAngZ)
{
    *outAngY = Maths::toDegrees(atan2f(-dir->z, dir->x));
    float distH = sqrtf(dir->z*dir->z + dir->x*dir->x);
    *outAngZ = Maths::toDegrees(atan2f(dir->y, distH));
}

Vector3f Maths::randomPointOnSphere()
{
    float z   = Maths::nextUniform()*2 - 1;
    float lng = Maths::nextUniform()*2*Maths::PI;

    float radius = sqrtf(1-(z)*(z));

    float x = radius*cosf(lng);
    float y = radius*sinf(lng);

    return Vector3f(x, y, z);


    //Makes a 3D Bernoulli Lemniscate ???

    //float theta = 2 * M_PI * Maths::nextUniform();
    //float phi = acosf(1 - 2 * Maths::nextUniform());
    //float x = sinf(phi) * cosf(theta);
    //float y = sinf(phi) * sinf(theta);
    //float z = cosf(phi);

    //return spherePositionFromAngles(x, y, z);
}

float Maths::random()
{
    return (rand() % RAND_MAX) / ((float)(RAND_MAX));
}

float Maths::nextGaussian()
{
    return Maths::distributionNormal(Maths::generatorNormal);
}

float Maths::nextUniform()
{
    return Maths::distributionUniform(Maths::generatorUniform);
}

Vector4f Maths::calcPlaneValues(Vector3f* p1, Vector3f* p2, Vector3f* p3)
{
    Vector3f vec1(p1->x - p3->x, p1->y - p3->y, p1->z - p3->z);
    Vector3f vec2(p2->x - p3->x, p2->y - p3->y, p2->z - p3->z);

    Vector3f cross = vec1.cross(&vec2);

    float newD = cross.x*p3->x + cross.y*p3->y + cross.z*p3->z;

    return Vector4f(cross.x, cross.y, cross.z, -newD);
}

Vector4f Maths::calcPlaneValues(Vector3f* point, Vector3f* normal)
{
    Vector3f perp1 = Maths::calculatePerpendicular(normal);
    Vector3f perp2 = perp1.cross(normal);

    Vector3f p1(point);
    Vector3f p2(point);
    Vector3f p3(point);
    p2 = p2 + perp1;
    p3 = p3 + perp2;

    return Maths::calcPlaneValues(&p1, &p2, &p3);
}

bool Maths::pointIsInCylinder(Vector3f* point, Vector3f* c1, Vector3f* c2, float cRadius)
{
    Vector3f cDiff(c2);
    cDiff = cDiff - c1;

    float cLength = cDiff.length();
    float angDiff = Maths::angleBetweenVectors(&cDiff, &X_AXIS);

    Vector3f p(point);
    p = p - c1; //move c1 to origin

    if (std::abs(angDiff) < 0.0001f)
    {
        //skip rotate if the cylinder already is aligned with x axis
    }
    else if (std::abs(angDiff) > Maths::PI - 0.0001f)
    {
        //rotate 180 degrees
        p = Maths::rotatePoint(&p, &Y_AXIS, Maths::toRadians(180));
    }
    else
    {
        Vector3f perpen = X_AXIS.cross(&cDiff);
        p = Maths::rotatePoint(&p, &perpen, -angDiff); //rotate so cylinder faces x axis
    }

    return (p.x >= 0 &&
            p.x < cLength &&
            p.y*p.y + p.z*p.z < cRadius*cRadius);
}

//coordinates are 0,0 for middle of screen, -1, -1 for top left, 1,1 for bot right
Vector2f Maths::calcScreenCoordsOfWorldPoint(Vector3f* worldPoint)
{
    Matrix4f viewMatrix;
    Maths::createViewMatrix(&viewMatrix, Global::gameCamera);

    Matrix4f modelMatrix;
    modelMatrix.translate(worldPoint);
    modelMatrix.m00 = viewMatrix.m00;
    modelMatrix.m01 = viewMatrix.m10;
    modelMatrix.m02 = viewMatrix.m20;
    modelMatrix.m10 = viewMatrix.m01;
    modelMatrix.m11 = viewMatrix.m11;
    modelMatrix.m12 = viewMatrix.m21;
    modelMatrix.m20 = viewMatrix.m02;
    modelMatrix.m21 = viewMatrix.m12;
    modelMatrix.m22 = viewMatrix.m22;
    //Vector3f axis(0, 0, 1);
    //modelMatrix.rotate(Maths::toRadians(0), &axis);
    //Vector3f scaleVec(1, 1, 1);
    //modelMatrix.scale(&scaleVec);
    Matrix4f modelViewMatrix = Matrix4f(modelMatrix);
    viewMatrix.multiply(&modelViewMatrix, &modelViewMatrix);

    Matrix4f result;
    MasterRenderer::projectionMatrix->multiply(&modelViewMatrix, &result);
    Vector4f vec4(0, 0, 0, 1);
    Vector4f gl_Position = result.transform(&vec4);
    float scl = gl_Position.w;
    gl_Position.scale(1/scl);

    return Vector2f(gl_Position.x, gl_Position.y);
}
