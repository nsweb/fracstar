
const float c_normal_distance = 0.0001;
const float c_minimun_distance = 0.001;
const int c_max_march_steps = 20;

#include "common/dftools.h"

uniform float collision_dist = 1.0;

in vec3 vs_fs_pos;
in vec3 vs_fs_view_dir;

layout (location = 0) out vec4 frag_color;


//----------------------------------------------------------------------

float opS( float d1, float d2 )
{
    return max(-d2,d1);
}

vec2 opU( vec2 d1, vec2 d2 )
{
	return (d1.x<d2.x) ? d1 : d2;
}

vec3 opRep( vec3 p, vec3 c )
{
    return mod(p,c)-0.5*c;
}

vec3 opTwist( vec3 p )
{
    float  c = cos(10.0*p.y+10.0);
    float  s = sin(10.0*p.y+10.0);
    mat2   m = mat2(c,-s,s,c);
    return vec3(m*p.xz,p.y);
}


vec2 map(vec3 p)
{
    p *= 1.1;
    vec3 pa = p;
    pa.x = abs(pa.x);
    vec3 pry = pa;
    pry = pRY(pry, -0.12 * GL_PI);
    vec3 prx = pa;
    prx = pRX(prx, -0.12 * GL_PI);
    
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

	//d = min( d, length(p) - 0.9 );
	//d = length(p) - 0.4;
    
    return vec2( d, 0.0 );
    //return length( p ) - 0.95f;
}

vec3 rayMarch( vec3 from, vec3 dir, float MaxDistance )
{
 	vec3 pos;
    vec2 dist_mat = vec2( c_minimun_distance, 0.0 );
    float sum_dist = 0.0;
    int sum_steps = 0;
    for( int i=0; i<c_max_march_steps; i++ )
    {
        pos = from + sum_dist * dir;
        dist_mat = map(pos);
        if( dist_mat.x < c_minimun_distance ) 
            break;
        sum_dist += dist_mat.x * 1.0;
        sum_steps++;
    }

    if( sum_dist >= MaxDistance ) 
        sum_dist = -1.0;

    float ao = float(sum_steps) / float(c_max_march_steps);
    return vec3( sum_dist, dist_mat.y, ao );   
}


void main(void)
{
	vec3 rayDir = normalize( vs_fs_view_dir );
    
    vec3 t0 = (vec3( 1.f, 1.f, 1.f) - vs_fs_pos) / rayDir;
    vec3 t1 = (vec3(-1.f,-1.f,-1.f) - vs_fs_pos) / rayDir;
    vec3 tmin = min( t0, t1 );
    float dist = -max( max( tmin.x, tmin.y ), tmin.z );
    
    vec3 rayPos = vs_fs_pos + rayDir * (-dist);
    vec3 res = rayMarch( rayPos, rayDir, dist );
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
        
        frag_color = vec4(col/*vec3(res.y)*/, 1.0);//mix( vec4( col, 1.0 ), bg_color, res.z );
        
        //float zeye = dot( camDir, rayDir ) * -res.x;
        //zn = z_var.x + z_var.y / zeye;
        //frag_color = vec4( vec3( clamp( 0.5 + 0.5*zn , 0.0, 1.0) ), 1 );
    }

	frag_color = mix( vec4(1.0,0.0,0.0,1.0), frag_color, collision_dist );

    //color = vec4(vs_fs_pos * 0.5 + 0.5,1.0);//vs_fs_color;
    //frag_color = vec4(dist * 0.5, dist * 0.5, dist * 0.5, 1.0);//vs_fs_color;
    
    // output in gamma space
    frag_color.rgb = pow( frag_color.rgb, vec3( 1.0/2.2 ) );
}
