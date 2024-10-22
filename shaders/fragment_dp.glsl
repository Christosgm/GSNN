#version 450 core

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

#extension GL_NV_fragment_shader_interlock : enable

in vec3 vertex_translated_position;
in vec3 ellipsoid_center;
in vec3 ellipsoid_scale;
in mat3 ellipsoid_rotation;
in float ellipsoid_alpha;
in vec4 spherical_harmonics[SH_VECTORS];
in float covariance3D[6];

uniform mat4 V;
uniform mat4 P;

uniform vec3 camera_position;
uniform vec2 viewportSize;
uniform float fx;
uniform float fy;
uniform mat4 viewMatrix;

layout (rgba32f, binding = 0) coherent uniform image2D sum_aici;
layout (rgba32f, binding = 1) coherent uniform image2D sum_ai;
layout (rgba32f, binding = 2) coherent uniform image2D prod_1_ai;
layout (rgba32f, binding = 3) coherent uniform image2D depth_avg_image;
layout (rgba32f, binding = 4) coherent uniform image2D minimum_depth_image;
layout (rgba32f, binding = 5) coherent uniform image2D maximum_depth_image;

out vec4 outColor;

//transcribed from https://github.com/graphdeco-inria/gaussian-splatting/blob/main/utils/sh_utils.py
void evalSH(vec3 rd, out vec3 color)
{
    float x = rd.x;
    float y = rd.y;
    float z = rd.z;

    //FIRST BAND
    #if BAND_COUNT > 0
        color = 0.28209479177387814 * spherical_harmonics[0].xyz;
    #endif 

    //SECOND BAND
    #if BAND_COUNT > 1
        color = color - 0.4886025119029199 * y * spherical_harmonics[1].xyz + 
                        0.4886025119029199 * z * spherical_harmonics[2].xyz - 
                        0.4886025119029199 * x * spherical_harmonics[3].xyz;
    #endif

    //THIRD BAND
    #if BAND_COUNT > 2
        float xx = x * x;
        float yy = y * y;
        float zz = z * z;

        float xy = x * y;
        float yz = y * z;
        float xz = z * z;  
        color = color +  1.0925484305920792 * xy * spherical_harmonics[4].xyz + 
                        -1.0925484305920792 * yz * spherical_harmonics[5].xyz + 
                        0.31539156525252005 * (2.0 * zz - xx - yy) * spherical_harmonics[6].xyz + 
                        -1.0925484305920792 * xz * spherical_harmonics[7].xyz + 
                        0.5462742152960396 * (xx - yy) * spherical_harmonics[8].xyz;
    #endif
    
    //FOURTH BAND
    #if BAND_COUNT > 3
        color = color + -0.5900435899266435 * y * (3 * xx - yy) * spherical_harmonics[9].xyz +
                        2.890611442640554 * xy * z * spherical_harmonics[10].xyz +
                        -0.4570457994644658 * y * (4 * zz - xx - yy)* spherical_harmonics[11].xyz +
                        0.3731763325901154 * z * (2 * zz - 3 * xx - 3 * yy) * spherical_harmonics[12].xyz +
                        -0.4570457994644658 * x * (4 * zz - xx - yy) * spherical_harmonics[13].xyz +
                        1.445305721320277 * z * (xx - yy) * spherical_harmonics[14].xyz +
                        -0.5900435899266435 * x * (xx - 3 * yy) * spherical_harmonics[15].xyz;
    #endif
    color += vec3(0.5);
    color.r = clamp(color.r, 0.0, 1.0);
    color.g = clamp(color.g, 0.0, 1.0);
    color.b = clamp(color.b, 0.0, 1.0);
}

vec3 closestEllipsoid(vec3 rayDirection, out vec3 normal)
{

    dvec3 localRayOrigin = (camera_position - ellipsoid_center) * ellipsoid_rotation;
    dvec3 localRayDirection = normalize(rayDirection * ellipsoid_rotation);
    dvec3 oneover = double(1) / dvec3(ellipsoid_scale);

    double a = dot(localRayDirection * oneover, localRayDirection * oneover);
    double b = 2.0 * dot(localRayDirection * oneover, localRayOrigin * oneover);
    double c = dot(localRayOrigin * oneover, localRayOrigin * oneover) - 1.0;

    double discriminant = b * b - 4.0 * a * c;
    if (discriminant < 0.0)
    {
        return vec3(0.0);
    }

    float t1 = float((-b - sqrt(discriminant)) / (2.0 * a));
    float t2 = float((-b + sqrt(discriminant)) / (2.0 * a));

    float t = min(t1, t2);

    vec3 localIntersection = vec3(localRayOrigin + t * localRayDirection);

    vec3 localNormal = normalize(localIntersection / ellipsoid_scale);

    normal = normalize(ellipsoid_rotation * localNormal);

    vec3 intersection = ellipsoid_rotation * localIntersection + ellipsoid_center;

    return intersection;
}

float ndc2Pix(float v, float S)
{
	return ((v + 1.0) * S - 1) * 0.5;
}

vec3 computeCov2D()
{
    vec3 t = (V * vec4(ellipsoid_center, 1.0)).xyz;
    vec2 tanfov = vec2(tan(fx/2), tan(fy/2));
    vec2 focal = vec2(viewportSize.x/(2 * tanfov.x), viewportSize.y/(2 * tanfov.y));

    float limx = 1.3 * tanfov.x;
    float limy = 1.3 * tanfov.y;

    t.x = min(limx, max(-limx, t.x/t.z)) * t.z;
    t.y = min(limy, max(-limy, t.y/t.z)) * t.z;

    mat3 J = mat3(
                   focal.x / t.z, 0.0f,     -(focal.x * t.x) / (t.z * t.z),
		           0.0f,     focal.y / t.z, -(focal.y * t.y) / (t.z * t.z),
		           0,        0,             0
                 );

    mat3 W = mat3(
                   V[0][0], V[1][0], V[2][0],
		           V[0][1], V[1][1], V[2][1],
		           V[0][2], V[1][2], V[2][2]
                 );

    mat3 T = W * J;

    mat3 Vk = mat3(
                   covariance3D[0], covariance3D[1], covariance3D[2],
                   covariance3D[1], covariance3D[3], covariance3D[4],
                   covariance3D[2], covariance3D[4], covariance3D[5]
                  );

    mat3 cov = transpose(T) * Vk * T;

    cov[0][0] += 0.3;
    cov[1][1] += 0.3;

    return vec3(cov[0][0], cov[0][1], cov[1][1]);

}


void main() 
{
    // if (ellipsoid_alpha < 0.3) discard; //If alpha is low discard

    vec3 normalized_ray_dir = normalize(vertex_translated_position - camera_position); //camera ray direction
    vec3 normal; 
    vec3 intersection_point = closestEllipsoid(normalized_ray_dir, normal); //find ray-ellipsoid intersection 
    if (intersection_point == vec3(0.0)) discard; //if no intersection discard

    //project intersection point to screen (NDC)
    vec4 newPos = P * V * vec4(intersection_point, 1.0);
    newPos /= newPos.w;
    //project ellipsoid center to screen (NDC)
    vec4 ellipsoid_proj = P * V * vec4(ellipsoid_center, 1.0);
    ellipsoid_proj /= ellipsoid_proj.w;

    //calculate their position in screen coordinates and find their distance to calculate falloff
    vec2 ellipsoid_proj_pix = vec2(ndc2Pix(ellipsoid_proj.x, viewportSize.x), ndc2Pix(ellipsoid_proj.y, viewportSize.y));
    vec2 intersection_proj_pix = vec2(ndc2Pix(newPos.x, viewportSize.x), ndc2Pix(newPos.y, viewportSize.y));

   vec2 d = intersection_proj_pix - ellipsoid_proj_pix;
    vec3 cov2D = computeCov2D();
    float det = cov2D.x * cov2D.z - cov2D.y * cov2D.y;
    if (det == 0) discard;
    vec3 con = vec3(cov2D.z/det, -cov2D.y/det, cov2D.x/det);
    float power = -0.5 * (con.x * d.x * d.x + con.z * d.y * d.y) - con.y * d.x * d.y;
    if (power > 0) discard;
    float falloff = exp(power);
    float a = min(0.99, ellipsoid_alpha * falloff);
    if (a < 1.0/255.0) discard;

    vec3 color = vec3(0.0);
    vec3 normalized_view_dir = normalize(ellipsoid_center - camera_position);
    evalSH(normalized_view_dir, color); // evaluate color

    //Depth peeling: FIRST PASS 
    //Find nearest fragment to camera and write depth to framebuffer 
    gl_FragDepth = newPos.z; 

    outColor = vec4(color*a, a);

    //Calculate features and write to texture (ensure exclusive writing to everyone)
    ivec2 coords = ivec2(gl_FragCoord.xy);
    beginInvocationInterlockNV();
        //Σaici
        vec4 prev_val = imageLoad(sum_aici, coords);
        prev_val.xyz += color*a;
        prev_val.w += 1;
        imageStore(sum_aici, coords, prev_val);

        //Σai
        prev_val = imageLoad(sum_ai, coords);
        prev_val.xyz += vec3(a);
        prev_val.w += 1;
        imageStore(sum_ai, coords, prev_val);

        //Π(1 - ai)
        prev_val = imageLoad(prod_1_ai, coords);
        prev_val.xyz *= (1 - a);
        prev_val.w += 1;
        imageStore(prod_1_ai, coords, prev_val);

        //avg(1 - zi)
        vec4 current_mean = imageLoad(depth_avg_image, coords);
        current_mean.xyz += (vec3(1 - gl_FragDepth) - current_mean.xyz)/(current_mean.w + 1);
        current_mean.w += 1;
        imageStore(depth_avg_image, coords, current_mean);
        
        //min depth
        vec4 current_min = imageLoad(minimum_depth_image, coords);
        if (current_min.x > 1 - newPos.z)
        {
            imageStore(minimum_depth_image, coords, vec4(1 - newPos.z, 1 - newPos.z, 1 - newPos.z, 1));
        }

        //max depth
        vec4 current_max = imageLoad(maximum_depth_image, coords);
        if (current_max.x < 1 - newPos.z)
        {
            imageStore(maximum_depth_image, coords, vec4(1 - newPos.z, 1 - newPos.z, 1 - newPos.z, 1));
        }
    endInvocationInterlockNV();
}