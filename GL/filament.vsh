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

/*bool isNan(float val)
{
    return ( val < 0.0 || 0.0 < val || val == 0.0 ) ? false : true;
}*/

void main(void)
{
    // Project the positions into view space
    vec4 curPos = uModelMatrix * vec4(aCurPos, aZ, 1.0);
    vec4 prevPos = uModelMatrix * vec4(aPrevPos, aZ, 1.0);
    vec4 nextPos = uModelMatrix * vec4(aNextPos, aZ, 1.0);
    vec4 newPos;

    // Calculate the normals for the line segments
    vec2 normNext = GetNormal(curPos.xy, nextPos.xy);
    vec2 normPrev = GetNormal(prevPos.xy, curPos.xy);

    if (abs(aSide) < 0.6)
    {
        // This is a 2nd point
        newPos = vec4(curPos.xy + normPrev, curPos.z, 1.0);
    }
    else
    {
        // This is a 1st point
        newPos = vec4(curPos.xy + normNext, curPos.z, 1.0);
    }

    //newPos = curPos;

    gl_Position = uProjMatrix * newPos;
    vColor = vec4(color, 1.0);

    /*vec4 nextPos = uModelMatrix * vec4(aNextPos, aZ, 1.0);
    vec4 prevPos = uModelMatrix * vec4(aPrevPos, aZ, 1.0);

    // Calculate the normals for the line segments
    vec2 normNext = GetNormal(curPos.xy, nextPos.xy);
    vec2 normPrev = GetNormal(prevPos.xy, curPos.xy);

    // Calculate the expanded points of the two lines
    vec2 norm = (normPrev + normNext) / 2.0;
    //vec2 a1 = prevPos.xy + normPrev;
    //vec2 a2 = curPos.xy + normPrev;
    //vec2 b1 = curPos.xy + normNext;
    //vec2 b2 = nextPos.xy + normNext;

    //vec4 pos = vec4(a2, curPos.z, 1.0);
    vec4 pos = vec4(curPos.xy + norm, curPos.z, 1.0);
    //vec4 pos = vec4(prevPos.xy, curPos.z, 1.0);

    vColor = vec4(color, 1.0);
    //gl_Position = uProjMatrix * uModelMatrix * aCurPos;
    gl_Position = uProjMatrix * pos;*/
    //gl_Position = uProjMatrix * curPos;
}

// Determine if there is an intersection
/*float s1_x, s1_y, s2_x, s2_y;
s1_x = a2.x - a1.x;     s1_y = a2.y - a1.y;
s2_x = b2.x - b1.x;     s2_y = b2.y - b1.y;
float s, t;
// TODO: zero diff...
s = (-s1_y * (a1.x - b1.x) + s1_x * (a1.y - b1.y)) / (-s2_x * s1_y + s1_x * s2_y);
t = ( s2_x * (a1.y - b1.y) - s2_y * (a1.x - b1.x)) / (-s2_x * s1_y + s1_x * s2_y);
if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
{
    // Collision detected
    float i_x = a1.x + (t * s1_x);
    float i_y = a1.y + (t * s1_y);

    // Use nonzero delta to determine z
}*/
