#version 450

#define BAND_COUNT 2

#if BAND_COUNT == 1
    #define SH_VECTORS 1
#elif BAND_COUNT == 2
    #define SH_VECTORS 4
#elif BAND_COUNT == 3
    #define SH_VECTORS 9
#else
    #define SH_VECTORS 16
#endif

layout (location = 0) in vec3 vertex_position;

layout (std430, binding = 0) buffer splatPosition {vec4 positions[];};
layout (std430, binding = 1) buffer splatScale {vec4 scales[];};
layout (std430, binding = 2) buffer splatColor {vec4 colors[];};
layout (std430, binding = 3) buffer splatQuat {vec4 quats[];};
layout (std430, binding = 4) buffer splatAlpha {float alphas[];};
layout (std430, binding = 5) buffer splatCov3D {float splatCovs3D[];};

uniform mat4 view;
uniform mat4 perspective;

out vec3 vertex_translated_position;
out vec3 ellipsoid_center;
out vec3 ellipsoid_scale;
out mat3 ellipsoid_rotation;
out float ellipsoid_alpha;
out float covariance3D[6];
out vec4 spherical_harmonics[SH_VECTORS];

mat3 build_rotation(vec4 q) 
{
    float r = q.x;
    float x = q.y;
    float y = q.z;
    float z = q.w;

    return mat3(1.0 - 2.0 * (y*y + z*z), 2.0 * (x*y - r*z), 2.0 * (x*z + r*y),
                2.0 * (x*y + r*z), 1.0 - 2.0 * (x*x + z*z), 2.0 * (y*z - r*x), 
                2.0 * (x*z - r*y), 2.0 * (y*z + r*x), 1.0 - 2.0 * (x*x + y*y)
               );
}

void main()
{
    ellipsoid_center = vec3(positions[gl_InstanceID]);
    ellipsoid_scale = vec3(scales[gl_InstanceID]) * 2.0;
    ellipsoid_rotation = transpose(build_rotation(quats[gl_InstanceID]));
    ellipsoid_alpha = alphas[gl_InstanceID];

    vertex_translated_position = ellipsoid_rotation * (ellipsoid_scale * vertex_position) + ellipsoid_center; //place bounding box to its place

    for(int i = 0; i < SH_VECTORS; i++) spherical_harmonics[i] = colors[gl_InstanceID * 16 + i]; //1d packed array of spherical harmonics 
    for (int i = 0; i < 6; i++) covariance3D[i] = splatCovs3D[gl_InstanceID * 6 + i]; //1d packed array of 3d covariance 6-values 

    gl_Position = perspective * view * vec4(vertex_translated_position, 1.0) * vec4(1, -1, 1, 1); //project vertex and flip in y (don't know why)
}