#version 330

#define MinimumDistance 0.001

in vec3 vs_fs_pos;
in vec3 vs_fs_view_dir;

layout (location = 0) out vec4 frag_color;

float DE(in vec3 p)
{
    return length( p ) - 0.95f;
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
        frag_color = vec4(0.f, 0.f, 0.f, 0.f);
    }
    else
    {
        //vec3 pos = camPos + res.x * rayDir;
        //col = material(inters, dir);
        
        //vec3 normal = getNormal(pos);//-dir*normalDistance*3.0);
        //vec3 col = l0 * max( 0.0, dot( d0, normal) ) + l1 * max( 0.0, dot( d1, normal) );
        
        frag_color = vec4(vec3(res.y), 1.0);//mix( vec4( col, 1.0 ), bg_color, res.y );
        
        //float zeye = dot( camDir, rayDir ) * -res.x;
        //zn = z_var.x + z_var.y / zeye;
        //frag_color = vec4( vec3( clamp( 0.5 + 0.5*zn , 0.0, 1.0) ), 1 );
    }

    //color = vec4(vs_fs_pos * 0.5 + 0.5,1.0);//vs_fs_color;
    //frag_color = vec4(dist * 0.5, dist * 0.5, dist * 0.5, 1.0);//vs_fs_color;
    
    // output in gamma space
    frag_color.rgb = pow( frag_color.rgb, vec3( 1.0/2.2 ) );
}
