uniform mat4 uModelMatrix;
uniform mat4 uProjMatrix;
//uniform float uFilamentRadius;
const float uFilamentRadius = 1.5;

attribute vec2 aCurPos;
attribute vec2 aNextPos;
attribute vec2 aPrevPos;
attribute float aZ;
attribute float aSide;

varying vec4 vColor;
//varying vec3 vLightNorm;
//varying vec3 vPos;

const vec3 color = vec3(0.2, 0.2, 0.8);
const vec3 lightPos = vec3(50.0, 50.0, 150.0);

vec2 GetNormal(vec2 a, vec2 b)
{
    float dX = b.x - a.x;
    float dY = b.y - a.y;
    if (aSide > 0.0)
        dY *= -1.0;
    else
        dX *= -1.0;
    vec2 norm = vec2(dY, dX);
    return normalize(norm) * uFilamentRadius;
}

struct InterPoint
{
    vec2 point;
    bool does;
};

InterPoint Intersection(float x1, float x2, float x3, float x4,
                 float y1, float y2, float y3, float y4)
{
    float x12 = x1 - x2;
    float x34 = x3 - x4;
    float y12 = y1 - y2;
    float y34 = y3 - y4;

    float c = x12 * y34 - y12 * x34;

    InterPoint inter;

    if (abs(c) < 0.01)
    {
        // No intersection
        inter.does = false;
    }
    else
    {
        // Intersection
        float a = x1 * y2 - y1 * x2;
        float b = x3 * y4 - y3 * x4;

        inter.point.x = (a * x34 - b * x12) / c;
        inter.point.y = (a * y34 - b * y12) / c;
        inter.does = true;
    }

    return inter;
}

void main(void)
{
    // Project the positions into view space
    vec4 curPos = uModelMatrix * vec4(aCurPos, aZ, 1.0);
    vec4 prevPos = uModelMatrix * vec4(aPrevPos, aZ, 1.0);
    vec4 nextPos = uModelMatrix * vec4(aNextPos, aZ, 1.0);
    vec3 newPos;

    // Calculate the normals for the line segments
    vec2 normNext = GetNormal(curPos.xy, nextPos.xy);
    vec2 normPrev = GetNormal(prevPos.xy, curPos.xy);

    bool sndPoint = (abs(aSide) < 0.3);
    bool cnrPoint = sndPoint ? false : (abs(aSide) < 0.7);

    /*if (cnrPoint)
    {

    }
    else
    {

    }*/

    // Calculate the intersection if any
    vec2 a1 = prevPos.xy + normPrev;
    vec2 a2 = curPos.xy + normPrev;
    vec2 b1 = nextPos.xy + normNext;
    vec2 b2 = curPos.xy + normNext;

    float prevZ = (prevPos.z + curPos.z) / 2.0;
    float nextZ = (curPos.z + nextPos.z) / 2.0;
    bool prevFront = (prevZ > nextZ);

    InterPoint inter = Intersection(a1.x, a2.x, b1.x, b2.x, a1.y, a2.y, b1.y, b2.y);
    if (inter.does && distance(a1, inter.point) < distance(a1, a2))
    {
        // We need the z pos of the intersection
        float totalPrevD = distance(a1, a2);
        float totalNextD = distance(b1, b2);
        float z = curPos.z;

        if (totalPrevD != 0.0 && prevPos.z != curPos.z && (sndPoint || (cnrPoint && prevFront)))
        {
            float deltaD = distance(a1, inter.point);
            float deltaZ = curPos.z - prevPos.z;
            z = prevPos.z + (deltaD / totalPrevD * deltaZ);
        }
        else if (totalNextD != 0.0 && nextPos.z != curPos.z && !sndPoint)
        {
            float deltaD = distance(b1, inter.point);
            float deltaZ = curPos.z - nextPos.z;
            z = nextPos.z + (deltaD / totalNextD * deltaZ);
        }

        newPos = vec3(inter.point, z);
        //intersects = true;
    }
    else
    {
        if (sndPoint)
            newPos = vec3(a2, curPos.z);
        else
            newPos = vec3(b2, curPos.z);
    }
    gl_Position = uProjMatrix * vec4(newPos, 1.0);

    // Get the lighting normal which is the same as the next or prev normal
    // but raised slightly out of the screen
    vec2 temp;
    const float lift = 0.9;
    if (cnrPoint)
    {
        if (prevZ == nextZ)
            temp = (normPrev + normNext) / 2.0;
        else if (nextZ > prevZ)
            temp = normNext;
        else
            temp = normPrev;
    }
    else if (sndPoint)
        temp = normPrev;
    else
        temp = normNext;

    vec3 lightNorm = normalize(vec3(temp, lift));

    //vLightNorm = lightNorm;
    //vPos = newPos;

    // Calculate the lighting and colour
    vec3 lightVector = normalize(lightPos - newPos);
    float diffuse = max(dot(lightNorm, lightVector), 0.1);
    vColor = vec4(color * diffuse, 1.0);

}
