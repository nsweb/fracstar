
#ifdef SHADER_SECTION
	$input v_texcoord0

	#include <bgfx_shader.sh>
	#define UNIFORM_LEVEL(a,b,c) uniform a b = (a)c;
#endif

#include "common/dftools.h"

#ifdef SHADER_SECTION
    uniform vec4 u_camera_pos;
    //uniform float global_time;
    //uniform vec2 z_var;
    //uniform vec2 screen_res;
    uniform vec4 u_z_var_screen_res;
#endif

// 2.28 1.5 1.0
//UNIFORM_LEVEL( vec4, u_lvl_mb_scale, 2.5f )
//UNIFORM_LEVEL( vec4, u_lvl_fixed_radius2, 1.0f )
//UNIFORM_LEVEL( vec4, u_lvl_min_radius2, 0.5f )

#define Ambient 		0.32184

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
//    z = clamp(z, -c_folding_limit, c_folding_limit) * 2.0 - z;
//}

vec2 mandelbox( vec3 z )
{
    vec4 u_lvl_mb_scale = (vec4) 2.5f;
    vec4 u_lvl_fixed_radius2 = (vec4) 1.0f;
    vec4 u_lvl_min_radius2 = (vec4) 0.5f;
    const float c_fudge_factor = 0.7f;
    const float c_folding_limit = 1.0f;

	float fixed2_over_min2 = vec4(u_lvl_fixed_radius2).x / vec4(u_lvl_min_radius2).x;
    vec3 offset = z;
    float dz = 1.0;
    for(int n = 0; n < 11; ++n)
    {
        //box_fold(z, dr);
        z = clamp(z, -c_folding_limit, c_folding_limit) * 2.0f - z;
        
        //sphere_fold(z, dr);
        float r2 = dot(z, z);
        if(r2 < vec4(u_lvl_min_radius2).x)
        {
            z *= fixed2_over_min2;
            dz *= fixed2_over_min2;
        }
        else if(r2 < vec4(u_lvl_fixed_radius2).x)
        {
            float temp = (vec4(u_lvl_fixed_radius2).x / r2);
            z *= temp;
            dz *= temp;
        }
        else if( r2 > 1e20f )
        {
            // prevent z to go over control (nan)
            return vec2(1000.0, 0.0);
        }

        z = vec4(u_lvl_mb_scale).x * z + offset;
        dz = dz * abs(vec4(u_lvl_mb_scale).x) + 1.0f;
    }
	
    float r = length(z);
    float mat = 0.0f;//abs( clamp(z, -2.0f*c_folding_limit, 2.0f*c_folding_limit) ) / 2.0f;
    return vec2( r / abs(dz), mat );
}


vec2 map(vec3 Pos)
{
    return mandelbox( Pos );
    
    // Folding 'tiling' of 3D space;
	//Pos  = abs(20.0-mod(Pos,20.0));
	// vec3 v = Pos - vec3(10.0, 0.0, 0.0);
	// float d = -5.0+length(v);
	
	// return d-0.001;
}

vec3 rayMarch( vec3 from, vec3 dir, float MaxDistance )
{
    const float c_minimun_distance = 0.001f;
 	vec3 pos;
    vec2 dist_mat = vec2( c_minimun_distance*2.0f, 0.0f );
    float sum_dist = 0.0;
    int sum_steps = 0;
    const int c_max_march_steps = 250;
    for( int i=0; i<c_max_march_steps; i++ )
    {
        pos = from + sum_dist * dir;
        dist_mat = map(pos);
        if( dist_mat.x < c_minimun_distance ) 
            break;
        sum_dist += dist_mat.x * 1.0f;
        sum_steps++;
    }

    if( sum_dist >= MaxDistance ) 
        sum_dist = -1.0;

    float ao = float(sum_steps) / float(c_max_march_steps);
    return vec3( sum_dist, dist_mat.y, ao );   
}

#ifdef SHADER_SECTION
	void main()
	{
        mat4 invView    = u_view;// transpose(u_invView);
		vec3 camPos     = u_camera_pos.xyz; //u_invView[3].xyz;
		vec3 camRight   = invView[0].xyz;
		vec3 camUp      = invView[1].xyz;
		vec3 camDir     = -invView[2].xyz;
	
		vec2 screen_coord = v_texcoord0.xy * u_z_var_screen_res.zw * 1.0;
	
		// Get direction for this pixel
		vec3 rayDir = normalize(camDir + screen_coord.x*camRight + screen_coord.y*camUp);
    
		//
		vec3 l0 = vec3(1.0, 0.75, 0.05);
		vec3 d0 = normalize( vec3(1.0, 1.0, 1.0) );
		vec3 l1 = vec3(0.4, 0.8, 0.9);//vec3(0.8, 0.2, 0.1);//
		vec3 d1 = normalize( vec3(1.0, -0.5, -1.0) );
		vec4 bg_color = vec4( vec3(0.05, 0.14, 0.27)*0.5, 1.0 );
    
   		vec3 res = rayMarch( camPos, rayDir, 100.0 );
		float zn = 0.999;
		if( res.x == -1.0 ) 
		{
			gl_FragColor = bg_color;
		}
		else
		{
    		vec3 pos = camPos + res.x * rayDir;
    		//col = material(inters, dir);
        
			vec3 normal = getNormal(pos);//-dir*normalDistance*3.0);
			vec3 col = l0 * max( 0.0, dot( d0, normal) ) + l1 * max( 0.0, dot( d1, normal) ); 
			col = mix( col, vec3( 1.0, 0.5, 0.25 ), res.y );
        
			gl_FragColor = mix( vec4( col, 1.0 ), bg_color, res.z ); //vec4(res, 1.0); //
            //gl_FragColor = vec4( normal.xyz * 0.5 + (vec3)0.5, 1.0);
            //gl_FragColor = vec4(1,0,0,1);
        
			float zeye = dot( camDir, rayDir ) * -res.x;
            vec4 zproj_vec = mul(u_proj, vec4(0.0, 0.0, zeye, 1.0));
			//zn = u_z_var_screen_res.x + u_z_var_screen_res.y / zeye;
            zn = zproj_vec.z / zproj_vec.w;
		}
        
        // need to convert to window coord [0;1], *not* in normalized device coordinate [-1;1]
        //gl_FragDepth = (zn + 1.0) / 2.0;
		gl_FragDepth = zn;
    
		// output in gamma space
		gl_FragColor.rgb = pow( gl_FragColor.rgb, (vec3) 1.0f/2.2f );
	}
#endif	// SHADER_SECTION
 


/*void main()
{
    // need to convert to window coord [0;1], *not* in normalized device coordinate [-1;1]
	float zn = 0.5;
    gl_FragDepth = (zn + 1.0) / 2.0;
	//gl_FragDepth = zn;
    
	// output in gamma space
	float val = 0.5;
	gl_FragColor = u_color; //vec4(val, 0.0, 0.0, 1.0);
	//frag_color.rgb = pow( frag_color.rgb, vec3_splat( 1.0/2.2 ) );
}*/