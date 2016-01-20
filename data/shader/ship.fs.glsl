#version 330

const float c_normal_distance = 0.0001;
const float c_minimun_distance = 0.001;
const int c_max_march_steps = 20;

#include "common/dftools.h"

in vec3 vs_fs_pos;
in vec3 vs_fs_view_dir;

layout (location = 0) out vec4 frag_color;

float DE(vec3 p)
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

vec2 rayMarch( in vec3 from, in vec3 dir, in float MaxDistance )
{
    vec3 pos;
    float dist = c_minimun_distance;
    float sum_dist = 0.0;
    int sum_steps = 0;
    
    for( int i=0; i<c_max_march_steps; i++ )
    {
        pos = from + sum_dist * dir;
        dist = DE(pos);
        if( dist < c_minimun_distance )
            break;
        sum_dist += dist * 1.0;
        if( sum_dist >= MaxDistance )
            break;
        sum_steps++;
    }
    
    if( sum_dist >= MaxDistance )
        sum_dist = -1.0;
    
    float ao = float(sum_steps) / float(c_max_march_steps);
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
