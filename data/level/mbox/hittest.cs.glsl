

#version 330

layout(local_size_x = 128) in;
//layout(std430) buffer;


//in uvec3 gl_NumWorkGroups; // Check how many work groups there are. Provided for convenience.
//in uvec3 gl_WorkGroupID; // Check which work group the thread belongs to.
//in uvec3 gl_LocalInvocationID; // Within the work group, get a unique identifier for the thread.
//in uvec3 gl_GlobalInvocationID; // Globally unique value across the entire compute dispatch. Short-hand for gl_WorkGroupID * gl_WorkGroupSize + gl_LocalInvocationID;
//in uint gl_LocalInvocationIndex; // 1D version of gl_LocalInvocationID. Provided for convenience.

struct hit_seg
{
	vec3 p0;	// beginning of segment to test
	vec3 p1;	// end of segment to test
};

struct hit_res
{
	vec3 hit;	// hit position
	vec3 dir;	// normal to hit position
};

layout(std430, binding = 0) writeonly buffer Output 
{
	hit_res elements[];
} output_data;

layout(std430, binding = 1) readonly buffer Input 
{
	hit_seg elements[];
} input_data0;

void main()
{
	uint ident = gl_GlobalInvocationID.x;
	output_data.elements[ident] = input_data0.elements[ident];
}