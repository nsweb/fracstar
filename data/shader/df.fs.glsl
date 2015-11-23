#version 330

layout (location = 0) out vec4 frag_color;
 
smooth in vec2 vs_fs_texcoord;
smooth in vec4 vs_fs_color;

uniform mat4 model2cam_mat;
uniform float global_time;

#define MaxSteps 		50
#define PI 				3.141592
#define FieldOfView 	1.0
#define FudgeFactor 	0.6
#define normalDistance  0.002
#define MinimumDistance 0.001
#define Ambient 		0.32184

// 2.28 1.5 1.0
const float mb_scale = 2.5;//2.320;//-2.5;
const float fixed_radius2 = 1.0;//2.5;//1.0 * 1.0;
const float min_radius2 = 0.5;//0.1;//0.5 * 0.5;
const float fixed2_over_min2 = fixed_radius2 / min_radius2;
const float folding_limit = 1.0;

//vec3 mtl = vec3(1.0, 1.3, 1.23)*0.8;

void sphere_fold( inout vec3 z, inout float dz ) 
{
    float r2 = dot(z, z);
    if(r2 < min_radius2) 
    {
        z *= fixed2_over_min2;
        dz *= fixed2_over_min2;
    }
    else if(r2 < fixed_radius2) 
    {
        float temp = (fixed_radius2 / r2);
        z *= temp;
        dz *= temp;
    }
}

void box_fold( inout vec3 z, inout float dz ) 
{
    z = clamp(z, -folding_limit, folding_limit) * 2.0 - z;
}

float mandelbox( in vec3 z ) 
{
    vec3 offset = z;
    float dr = 1.0;	// mb_scale ?
    for(int n = 0; n < 10; ++n) 
    {
        box_fold(z, dr);
        sphere_fold(z, dr);

        z = mb_scale * z + offset;
        dr = dr * abs(mb_scale) + 1.0;
    }
    float r = length(z);
    return r / abs(dr);
}

float DE(in vec3 Pos)
{
    return mandelbox( Pos );
    
    // Folding 'tiling' of 3D space;
	//Pos  = abs(20.0-mod(Pos,20.0));
	vec3 v = Pos - vec3(10.0, 0.0, 0.0);
	float d = -5.0+length(v);
	
	return d-0.001;
}

// Finite difference normal
vec3 getNormal(in vec3 pos) 
{
	vec3 e = vec3(0.0,normalDistance,0.0);
	
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
    float dist = MinimumDistance*2.0;
    float sum_dist = 0.0;
    int sum_steps = 0;
    for( int i=0; i<150; i++ )
    {
        pos = from + sum_dist * dir;
	    dist = DE(pos);
        if( dist < MinimumDistance ) 
            break;
        sum_dist += dist * 1.0;
        sum_steps++;
    }

    if( sum_dist >= MaxDistance ) 
        sum_dist = -1.0;

    float ao = float(sum_steps) / 150.0;
    return vec2(sum_dist, ao);   
}


vec3 camera()
{
    /*vec3 p = vec3( 1.58093041374559, 3.8640507877609, -1.64185034532454 );
    return p;*/

    float stime = sin(global_time*0.1); 
 	float ctime = cos(global_time*0.1); 
	vec3 p = vec3( 3.0*stime, 4.0*ctime, -2.9 + 0.0*stime );
    return p;

} 

vec3 cameraTarget()
{
    /*vec3 p = vec3( 1.58093041374559, 3.8640507877609, -1.64185034532454 );
    return p;*/

    float stime = sin(global_time*0.1 + 0.1); 
 	float ctime = cos(global_time*0.1 + 0.1); 
	vec3 p = vec3( 3.0*stime, 4.0*ctime, -2.9 + 0.0*stime );
    return p;

} 

void main(void)
{
    //vec4 rgba = vec4(1.0,0.0,1.0,0.5);
	//color = rgba;//vec4( rgba.r, 0,0,1);
	
	    // Camera position (eye), and camera target
	vec3 camPos = camera();///*0.5*global_time**/vec3(4.0, 0.0, 0.0);
	vec3 target = cameraTarget();//vec3( 0.0, 0.0, -3.0 );//camPos + vec3(-1.0, 0.0*cos(0.4*global_time), 0.0*sin(0.4*global_time));
	vec3 camUp  = vec3(0.0,1.0,0.0);
	
	// Calculate orthonormal camera reference system
	vec3 camDir   = normalize(target-camPos); // direction for center ray
	camUp = normalize(camUp-dot(camDir,camUp)*camDir); // orthogonalize
	vec3 camRight = normalize(cross(camDir,camUp));
	
	vec2 coord = vs_fs_texcoord.xy;//-1.0+2.0*fragCoord.xy/iResolution.xy;
	//coord.x *= iResolution.x/iResolution.y;
	
	// Get direction for this pixel
	vec3 rayDir = normalize(camDir + (coord.x*camRight + coord.y*camUp)*FieldOfView);
    
    //
	vec3 l0 = vec3(1.0, 0.75, 0.05);
    vec3 d0 = normalize( vec3(1.0, 1.0, 1.0) );
	vec3 l1 = vec3(0.4, 0.8, 0.9);//vec3(0.8, 0.2, 0.1);//
    vec3 d1 = normalize( vec3(1.0, -0.5, -1.0) );
    vec4 bg_color = vec4( vec3(0.05, 0.14, 0.27)*0.5, 1.0 );
    
   	vec2 res = rayMarch( camPos, rayDir, 10.0 );
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
        
    	//vec3 tmp = normal*0.5 + 0.5;
        frag_color = mix( vec4( col, 1.0 ), bg_color, res.y );
    }
	
	
}