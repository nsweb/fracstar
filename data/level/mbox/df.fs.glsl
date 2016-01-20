#version 330

layout (location = 0) out vec4 frag_color;

const float c_normal_distance = 0.002;
const float c_minimun_distance = 0.001;
const int c_max_march_steps = 150;

#include "common/dftools.h"

smooth in vec2 vs_fs_texcoord;
smooth in vec4 vs_fs_color;

uniform mat4 viewinv_mat;
uniform mat4 proj_mat;
uniform vec3 camera_pos;
uniform float global_time;
// z_var.x = (far+near)/(far-near);
// z_var.y = 2.0*far*near/(far-near);
uniform vec2 z_var;	
uniform vec2 screen_res;

#define FudgeFactor 	0.6
#define Ambient 		0.32184

// 2.28 1.5 1.0
uniform float lvl_mb_scale = 2.5;//2.320;//-2.5;
uniform float lvl_fixed_radius2 = 1.0;//2.5;//1.0 * 1.0;
uniform float lvl_min_radius2 = 0.5;//0.1;//0.5 * 0.5;
const float folding_limit = 1.0;

//vec3 mtl = vec3(1.0, 1.3, 1.23)*0.8;

//void sphere_fold( inout vec3 z, inout float dz ) 
//{
//    float r2 = dot(z, z);
//    if(r2 < lvl_min_radius2) 
//    {
//        z *= fixed2_over_min2;
//        dz *= fixed2_over_min2;
//    }
//    else if(r2 < lvl_fixed_radius2)
//    {
//        float temp = (lvl_fixed_radius2 / r2);
//        z *= temp;
//        dz *= temp;
//    }
//}
//
//void box_fold( inout vec3 z, inout float dz ) 
//{
//    z = clamp(z, -folding_limit, folding_limit) * 2.0 - z;
//}

float mandelbox( in vec3 z )
{
	float fixed2_over_min2 = lvl_fixed_radius2 / lvl_min_radius2;
    vec3 offset = z;
    float dz = 1.0;
    for(int n = 0; n < 12; ++n)
    {
        //box_fold(z, dr);
        z = clamp(z, -folding_limit, folding_limit) * 2.0 - z;
        
        //sphere_fold(z, dr);
        float r2 = dot(z, z);
        if(r2 < lvl_min_radius2)
        {
            z *= fixed2_over_min2;
            dz *= fixed2_over_min2;
        }
        else if(r2 < lvl_fixed_radius2)
        {
            float temp = (lvl_fixed_radius2 / r2);
            z *= temp;
            dz *= temp;
        }
        else if( r2 > 1e20f )
        {
            // prevent z to go over control (nan)
            return 1000.0;
        }

        z = lvl_mb_scale * z + offset;
        dz = dz * abs(lvl_mb_scale) + 1.0;
    }
	
    float r = length(z);
    return r / abs(dz);
}

float DE(vec3 Pos)
{
    return mandelbox( Pos );
    
    // Folding 'tiling' of 3D space;
	//Pos  = abs(20.0-mod(Pos,20.0));
	// vec3 v = Pos - vec3(10.0, 0.0, 0.0);
	// float d = -5.0+length(v);
	
	// return d-0.001;
}

vec2 rayMarch( vec3 from, vec3 dir, float MaxDistance )
{
 	vec3 pos;
    float dist = c_minimun_distance*2.0;
    float sum_dist = 0.0;
    int sum_steps = 0;
    for( int i=0; i<c_max_march_steps; i++ )
    {
        pos = from + sum_dist * dir;
        dist = DE(pos);
        if( dist < c_minimun_distance ) 
            break;
        sum_dist += dist * 1.0;
        sum_steps++;
    }

    if( sum_dist >= MaxDistance ) 
        sum_dist = -1.0;

    float ao = float(sum_steps) / float(c_max_march_steps);
    return vec2(sum_dist, ao);   
}

void main(void)
{
    vec3 camPos     = viewinv_mat[3].xyz;
    vec3 camRight   = viewinv_mat[0].xyz;
    vec3 camUp      = viewinv_mat[1].xyz;
    vec3 camDir     = -viewinv_mat[2].xyz;
	
	vec2 screen_coord = vs_fs_texcoord.xy * screen_res * 1.0;
	
	// Get direction for this pixel
	vec3 rayDir = normalize(camDir + screen_coord.x*camRight + screen_coord.y*camUp);
    
    //
	vec3 l0 = vec3(1.0, 0.75, 0.05);
    vec3 d0 = normalize( vec3(1.0, 1.0, 1.0) );
	vec3 l1 = vec3(0.4, 0.8, 0.9);//vec3(0.8, 0.2, 0.1);//
    vec3 d1 = normalize( vec3(1.0, -0.5, -1.0) );
    vec4 bg_color = vec4( vec3(0.05, 0.14, 0.27)*0.5, 1.0 );
    
   	vec2 res = rayMarch( camPos, rayDir, 100.0 );
	float zn = 0.999;
    if( res.x == -1.0 ) 
    {
        frag_color = bg_color;
    }
    else
    {
    	vec3 pos = camPos + res.x * rayDir;
    	//col = material(inters, dir);
        
        vec3 normal = getNormal(pos);//-dir*normalDistance*3.0);
        vec3 col = l0 * max( 0.0, dot( d0, normal) ) + l1 * max( 0.0, dot( d1, normal) ); 
        
        frag_color = mix( vec4( col, 1.0 ), bg_color, res.y );
        
        float zeye = dot( camDir, rayDir ) * -res.x;
        zn = z_var.x + z_var.y / zeye;
		//frag_color = vec4( vec3( clamp( 0.5 + 0.5*zn , 0.0, 1.0) ), 1 );
    }
	
    //float zc = ( proj_mat * vec4( res.x * rayDir, 1.0 ) ).z;
    //float wc = ( proj_mat * vec4( res.x * rayDir, 1.0 ) ).w;
    //gl_FragDepth = zc/wc;
	gl_FragDepth = zn;
    
    // output in gamma space
    frag_color.rgb = pow( frag_color.rgb, vec3( 1.0/2.2 ) );
}
