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

const vec3 color = vec3(0.2, 0.2, 0.8);
const vec3 lightPos = vec3(50.0, 50.0, 150.0);

vec2 GetNormal(vec2 a, vec2 b, bool outside)
{
    float dX = b.x - a.x;
    float dY = b.y - a.y;
    if (outside)
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

    bool outsidePoint = (aSide > 0.0);

    // Calculate the normals for the line segments
    vec2 normNext = GetNormal(curPos.xy, nextPos.xy, outsidePoint);
    vec2 normPrev = GetNormal(prevPos.xy, curPos.xy, outsidePoint);

    // If the normals have a positive cross then the direction will
    // be on the outside
    vec3 crossed = cross(vec3(normPrev, 0.0), vec3(normNext, 0.0));
    const vec3 ref = vec3(0.0, 0.0, 1.0);
    bool interOut = (dot(crossed, ref) > 0.0);

    bool sndPoint = (abs(aSide) < 0.3);
    bool cnrPoint = sndPoint ? false : (abs(aSide) < 0.7);

    // Invert the normals for a corner point if it is on the wrong side
    if (cnrPoint && outsidePoint && !interOut)
    {
        normNext *= -1.0;
        normPrev *= -1.0;
    }

    // Get the 4 points of the two lines
    vec2 a1 = prevPos.xy + normPrev;
    vec2 a2 = curPos.xy + normPrev;
    vec2 b1 = nextPos.xy + normNext;
    vec2 b2 = curPos.xy + normNext;

    // Calculate the intersection if any
    float prevZ = (prevPos.z + curPos.z) / 2.0;
    float nextZ = (curPos.z + nextPos.z) / 2.0;
    bool prevFront = (prevZ > nextZ);

    // We should only check for an intersection if it can be possible like when the interscetion is
    // on the current side
    if (cnrPoint || (outsidePoint && interOut))
    {
        // If the intersection is not on a horizontal edge then it should be on a vertical edge
        InterPoint inter = Intersection(a1.x, a2.x, b1.x, b2.x, a1.y, a2.y, b1.y, b2.y);
        if (inter.does && distance(a1, inter.point) < distance(a1, a2))
        {
            // We need the z pos of the intersection
            float totalPrevD = distance(a1, a2);
            float totalNextD = distance(b1, b2);
            float z = curPos.z;

            // The z of a 2nd point should lay on the previous line whilst the z of a 1st point should be on the next line
            // the z of the should be on whichever one is in front
            if ((sndPoint || (cnrPoint && prevFront)) && totalPrevD != 0.0 && prevPos.z != curPos.z)
            {
                float deltaD = distance(a1, inter.point);
                float deltaZ = curPos.z - prevPos.z;
                z = prevPos.z + (deltaD / totalPrevD * deltaZ);
            }
            else if (!sndPoint && totalNextD != 0.0 && nextPos.z != curPos.z)
            {
                float deltaD = distance(b1, inter.point);
                float deltaZ = curPos.z - nextPos.z;
                z = nextPos.z + (deltaD / totalNextD * deltaZ);
            }

            newPos = vec3(inter.point, z);
        }
        else
        {
            vec2 up, down;
            vec2 left, right;
            float deltaZ;
            if (prevFront)
            {
                up = prevPos.xy + normPrev;
                down = prevPos.xy - normPrev;
                left = b1;
                right = b2;
                deltaZ = prevPos.z - curPos.z;
            }
            else
            {
                up = nextPos.xy + normNext;
                down = nextPos.xy - normNext;
                left = a1;
                right = a2;
                deltaZ = nextPos.z - curPos.z;
            }

            InterPoint inter = Intersection(left.x, right.x, up.x, down.x, left.y, right.y, up.y, down.y);

            float totalD = distance(left, right);
            float z = curPos.z;

            if (totalD != 0.0 && deltaZ != 0.0)
            {
                float deltaD = distance(right, inter.point);
                z = curPos.z + (deltaD / totalD * deltaZ);
            }

            newPos = vec3(inter.point, z);
        }
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
        if (abs(prevZ - nextZ) < 1.0)
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

    // Calculate the lighting and colour
    vec3 lightVector = normalize(lightPos - newPos);
    float diffuse = max(dot(lightNorm, lightVector), 0.1);
    vColor = vec4(color * diffuse, 1.0);
}
