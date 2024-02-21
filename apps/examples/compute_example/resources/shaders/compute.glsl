#version 450

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Particle {
	 vec4 pos;
};

// Binding 0 : Position storage buffer
layout(std430, set = 0, binding = 0) buffer ParticleBuf 
{
   Particle particle[ ];
};

// arguments
layout (set = 0, binding = 1) uniform Uniforms 
{
	float time;
};

#define PI 3.1415926535897932384626433


void main(){

	float t = time;

	uint index  = gl_GlobalInvocationID.x; // globalInvocationId is how we were dispatched.
	vec2 x_zero = (vec2(index / 65, index % 65) / 64.f - vec2(0.5)) * 1024; // original (x/y) position of the vertex

	float omega_0 = (PI * 2);

	const float DEG_TO_RAD = (2*PI)/360.f;
	
	// We simulate Gerstner waves. 
	//
	// These waves are calculated as the sum of wave vectors (denoted 'k' in Tessendorf[1])
	//
	// We want our waves to be the sum of three wave vectors. To give us more control, 
	// we calculate wave vectors based on their direction and length.
	// 
	float wave_angle[3] = {	
		23.f * DEG_TO_RAD,
		35.f * DEG_TO_RAD,
		15.f * DEG_TO_RAD,
	}; // given in radians

	vec2  wave_direction[3];
	wave_direction[0] = normalize(vec2(cos(wave_angle[0]),sin(wave_angle[0])));
	wave_direction[1] = normalize(vec2(cos(wave_angle[1]),sin(wave_angle[1])));
	wave_direction[2] = normalize(vec2(cos(wave_angle[2]),sin(wave_angle[2])));

	float wave_length[3] = {
		23.f,
		59.f,
		115.f,
	};

	// Now that we know length and direction for each wave vector, we can 
	// fully define the wave vectors.
	//
	vec2 wave_vector[3];

	float amplitude[3] = { 25,   13,  5  };
	float phase[3]     = {  2, 1.56, 0.12};
	float omega[3];

	for (int i=0; i!=3; i++){
		wave_vector[i] = wave_direction[i] * wave_length[i];
		float depthF = tanh(length(wave_vector[i])*0.002*(x_zero.x + 513));
		// depthF = 1; // deep water
		omega[i] = floor(sqrt(9.8 * length(wave_vector[i]) * depthF) / omega_0) * omega_0;
	}


	vec2 sum_xz = vec2(0);
	float sum_y = 0;

	for (int i =0; i!=3; i++){
		sum_xz += normalize(wave_vector[i]) * amplitude[i] * sin(dot(x_zero/1024., wave_vector[i]) - omega[i] * t + phase[i]);
		sum_y  += -amplitude[i] * cos(dot(x_zero/1024. , wave_vector[i]) - omega[i] * t + phase[i]);
	}

	particle[index].pos.xz = (x_zero - sum_xz);
	particle[index].pos.y  = sum_y;
    particle[index].pos.w  = 1;

}


/*
 
 References:

 [1]: TESSENDORF, J. 2001. Simulating ocean water. In Simulating Nature: Realistic and Interactive Techniques. SIGGRAPH 2001 Course Notes 47. 

*/
