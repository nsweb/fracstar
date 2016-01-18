#version 330

#define MinimumDistance 0.001
#define PI 3.14159265
#define NormalDistance  0.0001

in vec3 vs_fs_pos;
in vec3 vs_fs_view_dir;

layout (location = 0) out vec4 frag_color;

// The "Chamfer" flavour makes a 45-degree chamfered edge (the diagonal of a square of size <r>):
float fOpUnionChamfer(float a, float b, float r) {
    return min(min(a, b), (a - r + b)*sqrt(0.5));
}

// Intersection has to deal with what is normally the inside of the resulting object
// when using union, which we normally don't care about too much. Thus, intersection
// implementations sometimes differ from union implementations.
float fOpIntersectionChamfer(float a, float b, float r) {
    return max(max(a, b), (a + r + b)*sqrt(0.5));
}

// Difference can be built from Intersection or Union:
float fOpDifferenceChamfer (float a, float b, float r) {
    return fOpIntersectionChamfer(a, -b, r);
}

// The "Round" variant uses a quarter-circle to join the two objects smoothly:
float fOpUnionRound(float a, float b, float r) {
    vec2 u = max(vec2(r - a,r - b), vec2(0));
    return max(r, min (a, b)) - length(u);
}

float fOpIntersectionRound(float a, float b, float r) {
    vec2 u = max(vec2(r + a,r + b), vec2(0));
    return min(-r, max (a, b)) + length(u);
}

float fOpDifferenceRound (float a, float b, float r) {
    return fOpIntersectionRound(a, -b, r);
}

// Similar to fOpUnionRound, but more lipschitz-y at acute angles
// (and less so at 90 degrees). Useful when fudging around too much
// by MediaMolecule, from Alex Evans' siggraph slides
float fOpUnionSoft(float a, float b, float r) {
    float e = max(r - abs(a - b), 0);
    return min(a, b) - e*e*0.25/r;
}


// produces a cylindical pipe that runs along the intersection.
// No objects remain, only the pipe. This is not a boolean operator.
float fOpPipe(float a, float b, float r) {
    return length(vec2(a, b)) - r;
}

// first object gets a v-shaped engraving where it intersect the second
float fOpEngrave(float a, float b, float r) {
    return max(a, (a + r - abs(b))*sqrt(0.5));
}

// first object gets a capenter-style groove cut out
float fOpGroove(float a, float b, float ra, float rb) {
    return max(a, min(a + ra, rb - abs(b)));
}

// first object gets a capenter-style tongue attached
float fOpTongue(float a, float b, float ra, float rb) {
    return min(a, max(a - ra, abs(b) - rb));
}

float fCylinderZ(vec3 p, float r, float height) {
    float d = length(p.xy) - r;
    d = max(d, abs(p.z) - height);
    return d;
}

// Rotate around a coordinate axis (i.e. in a plane perpendicular to that axis) by angle <a>.
// Read like this: R(p.xz, a) rotates "x towards z".
// This is fast if <a> is a compile-time constant and slower (but still practical) if not.
void pRX(inout vec3 p, float a) {
    p.yz = cos(a)*p.yz + sin(a)*vec2(p.z, -p.y);
}

void pRY(inout vec3 p, float a) {
    p.xz = cos(a)*p.xz + sin(a)*vec2(p.z, -p.x);
}


float DE(in vec3 p)
{

    p *= 1.1;
    vec3 pa = p;
    pa.x = abs(pa.x);
    vec3 pry = pa;
    pRY(pry, -0.12 * PI);
    vec3 prx = pa;
    pRX(prx, -0.12 * PI);
    
    //float fCLeft = fCylinderZ( p - vec3(-0.5, -0.2, 0.2), 0.4, 0.7 );
    float fCMain = fCylinderZ( prx - vec3(0.0, 0.05, 0.3), 0.45, 0.6 );
    float fCUp = fCylinderZ( p - vec3(0.0, 0.45, 0.9), 0.45, 0.1 );
    float d = fOpUnionSoft(fCMain, fCUp, 0.3);
    float fCRight = fCylinderZ( pry - vec3(0.4, -0.2, 0.2), 0.4, 0.7 );
    float fCRightHole = fCylinderZ( pa - vec3(0.62, -0.2, 0.75), 0.33, 0.2 );
    fCRight = fOpDifferenceRound(fCRight, fCRightHole, 0.04);
    //return min( fCLeft, fCRight );
    d = /*min( fCUp,*/ fOpUnionSoft( d/*fCMain*/, fCRight, 0.2 );// );//, 0.1);
    d = fOpDifferenceRound(d, fCRightHole, 0.04);
    
    return d;
    //return length( p ) - 0.95f;
}

// Finite difference normal
vec3 getNormal(in vec3 pos)
{
    vec3 e = vec3(0.0,NormalDistance,0.0);
    
    return normalize(vec3(
                          DE(pos+e.yxx)-DE(pos-e.yxx),
                          DE(pos+e.xyx)-DE(pos-e.xyx),
                          DE(pos+e.xxy)-DE(pos-e.xxy)
                          )
                     );
}

vec2 rayMarch( in vec3 from, in vec3 dir, in float MaxDistance )
{
    vec3 pos;
    float dist = MinimumDistance;
    float sum_dist = 0.0;
    int sum_steps = 0;
    
    for( int i=0; i<20; i++ )
    {
        pos = from + sum_dist * dir;
        dist = DE(pos);
        if( dist < MinimumDistance )
            break;
        sum_dist += dist * 1.0;
        if( sum_dist >= MaxDistance )
            break;
        sum_steps++;
    }
    
    if( sum_dist >= MaxDistance )
        sum_dist = -1.0;
    
    float ao = float(sum_steps) / 20.0;
    return vec2(sum_dist, ao);
}

void main(void)
{
	vec3 rayDir = normalize( vs_fs_view_dir );
    
    vec3 t0 = (vec3( 1.f, 1.f, 1.f) - vs_fs_pos) / rayDir;
    vec3 t1 = (vec3(-1.f,-1.f,-1.f) - vs_fs_pos) / rayDir;
    vec3 tmin = min( t0, t1 );
    float dist = -max( max( tmin.x, tmin.y ), tmin.z );
    
    vec3 rayPos = vs_fs_pos + rayDir * (-dist);
    vec2 res = rayMarch( rayPos, rayDir, dist );
    if( res.x == -1.0 )
    {
        //frag_color = vec4(0.0);
        discard;
    }
    else
    {
        vec3 pos = rayPos + rayDir * res.x;
        //col = material(inters, dir);
        
        vec3 normal = getNormal(pos);//-dir*normalDistance*3.0);
        vec3 d0 = normalize( vec3(1.0, 1.0, 1.0) );
        vec3 col = vec3(1.0, 1.0, 1.0) * max( 0.0, 0.5 + 0.5*dot( d0, normal) );
        
        frag_color = vec4(col/*vec3(res.y)*/, 1.0);//mix( vec4( col, 1.0 ), bg_color, res.y );
        
        //float zeye = dot( camDir, rayDir ) * -res.x;
        //zn = z_var.x + z_var.y / zeye;
        //frag_color = vec4( vec3( clamp( 0.5 + 0.5*zn , 0.0, 1.0) ), 1 );
    }

    //color = vec4(vs_fs_pos * 0.5 + 0.5,1.0);//vs_fs_color;
    //frag_color = vec4(dist * 0.5, dist * 0.5, dist * 0.5, 1.0);//vs_fs_color;
    
    // output in gamma space
    frag_color.rgb = pow( frag_color.rgb, vec3( 1.0/2.2 ) );
}
