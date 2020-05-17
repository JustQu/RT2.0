/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   world.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmelessa <dmelessa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/10 19:36:22 by dmelessa          #+#    #+#             */
/*   Updated: 2020/05/14 15:32:52 by dmelessa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WORLD_H
# define WORLD_H

# define DEFAULT_WIDTH 1200
# define DEFAULT_HEIGHT 600
# define DEFAULT_FOV 90
# define WORK_GROUP_SIZE 128

//# define BOUNCES 5
//# define SAMPLES 100
// # define EPSILON  0.00003f /* required to compensate for limited float precision */
// # define PI  3.14159265359f //We dont need this constant since we can use constant from math.h

# ifndef __OPENCL_C_VERSION__
#  ifdef __APPLE__
#   include "OpenCL/opencl.h"
#  else
#   include "CL/cl.h"
#  endif
# endif

# ifdef __OPENCL_C_VERSION__
#  define cl_float4 float4
#  define cl_float3 float3
#  define cl_float float
#  define cl_int4 int4
#  define cl_int3 int3
#  define cl_int int
#  define cl_char char
#  define cl_uchar uchar
#  define cl_bool bool
#  define cl_uint unsigned int
# endif

typedef union s_color				t_color;

typedef enum e_sampler_type			t_sampler_type;
typedef struct s_sampler			t_sampler;

typedef struct s_render_options		t_render_options;

typedef enum e_material_type		t_material_type;
typedef struct s_material			t_material;

typedef enum e_types				t_type;
typedef enum e_light_types			t_light_type;
typedef struct s_obj				t_obj;
typedef struct s_light				t_light;
typedef struct s_ambient_occluder	t_ambient_occluder;
typedef struct s_triangle			t_triangle;
typedef struct s_bbox				t_bbox;

typedef enum e_camera_type			t_camera_type;
typedef struct s_camera				t_camera;
typedef struct s_viewplane			t_viewplane;

union s_color {
	cl_int value;
	struct
	{
		cl_uchar b;
		cl_uchar g;
		cl_uchar r;
		cl_uchar a;
	};
};

enum	e_sampler_type
{
		none, //maybe we can replace it with regular grid where is onlu one sample per pixel
		regular_grid,
		jitter,
		rand_jitter,
		pure_random,
		nrooks
};


/*
** Sample types flags
*/

#define DEFAULT_SAMPLES		1 << 0
#define DISK_SAMPLES		1 << 1
#define HEMISPHERE_SAMPLES	1 << 2

# ifdef _WIN64
__declspec(align(4))
# endif
struct				s_sampler
{
	t_sampler_type	type;
	cl_int			num_samples; /* the number of sample points in a pattern */
	cl_int			num_sets;	/* the number of sample sets(patterns) stores */
	cl_uint			count;		/* the currenct numer of sample points used */
	cl_int			jump;		// random index jump
	cl_int			samples_type; // default / disk / hemisphere
	// cl_int			shuffled_indices[NUM_SAMPLES * NUM_SETS];

	cl_int			offset;
	// cl_int			disk_samples_offset;
	// cl_int			hemisphere_samples_offset;
};

# ifdef _WIN64
__declspec(align(4))
# endif
struct				s_render_options
{
	t_sampler		ambient_occluder_sampler;
	cl_int			depth;
	cl_int			shadows;
	t_color			background_color;
	cl_int			sampler_id;
	cl_int			ambient_occlusion;
};

enum e_material_type
{
	matte, //kd, ka
	phong
};

# ifdef _WIN64
__declspec(align(4))
# endif
struct					s_material //kd + ks < 1.0
{
	t_material_type		type;
	t_color				color;
	cl_float			kd; //diffuse reflection coefficient [0, 1]
	cl_float			ka; //ambient reflection
	cl_float			ks; //coefficient of specular reflection [0, 1]
	cl_float			exp;
	void				*texture;
};

enum	e_types
{
	cone,
	cylinder,
	paraboloid,
	plane,
	sphere,
	torus,
	triangle
};

//NOTE: some types will be removed from that list
enum	e_light_types
{
	ambient,
	ambient_occluder,
	directional,
	point,
	area,
	enviromental
};

# ifdef _WIN64
__declspec(align(4))
# endif
struct	s_light
{
	cl_float4		origin;
	cl_float4		direction;
	t_color			color;
	cl_float		ls; //radiance scaling factor [0, inf)
	t_light_type	type;
};

/**
** @brief
** axis-aligned bounding box for object
*/
#ifdef _WIN64
__declspec(align(4))
#endif
struct					s_bbox
{
	cl_float4			min;
	cl_float4			max;
};

# ifdef _WIN64
__declspec(align(4))
# endif
struct					s_obj
{
	t_type				type;
	t_material			material;
	t_bbox				bounding_box;
	cl_float4			origin;
	cl_float4			direction;
	cl_float			r;
	cl_float			r2;
	cl_float			angle;
	cl_float			maxm;
	cl_float			minm;
	cl_bool				shadows;
};

# ifdef _WIN64
__declspec(align(4))
# endif
struct	s_triangle
{
	t_material	material;
	cl_float4	vertex1;
	cl_float4	vertex2;
	cl_float4	vertex3;
	cl_float4	vector1; //vertex2 - vertex1
	cl_float4	vector2; // vertex3 - vertex1
	cl_float4	normal;
};

enum	e_camera_type
{
		orthographic,
		perspective,
		thin_lens,
		fisheye,
		spherical,
		stereo
};

# ifdef _WIN64
__declspec(align(4))
# endif
struct	s_viewplane
{
	cl_float	pixel_size;
	cl_int		width;
	cl_int		height;
};

/**
** @TODO: make transformation matrix
** TODO: singularity paragraph 9.9
*/
# ifdef _WIN64
__declspec(align(4))
# endif
struct	s_camera
{
	t_viewplane			viewplane;
	cl_float4			origin; // eye
	cl_float4			direction; // lookat - eye
	cl_float4			up; // (0, 1, 0)
	cl_float4			u;
	cl_float4			v; // actually we need only th3 vectors. the up vector could be hardcodeded and w = -direction
	cl_float4			w;
	cl_float			d; //the view-plane distance
	cl_float			zoom; //zoom factor
	cl_float			exposure_time; //using somewhere later
	t_camera_type		type;
	cl_int				sampler_id; //thin_camera
	cl_int				normalized;
	//thin-lens camera
	cl_float			l; //lens radius or lambda
	cl_float			f; //focal plane distance or psi_max in fisheye

//note: prob not needed
	cl_float			ratio;
	cl_float			inv_w;
	cl_float			inv_h;
	cl_float			angle;
	cl_int				fov;
};

# ifdef _WIN64
__declspec(align(4))
# endif
struct	s_ambient_occluder
{
	cl_float4	u;
	cl_float4	v;
	cl_float4	w;
	t_color		color;
	cl_float	ls; //radiance scaling factor [0, inf)
	cl_int		sampler_id;
	t_color		min_amount;
};

#endif
#ifndef CL_RT_H
# define CL_RT_H

// #include "world.h"

#define K_HUGE_VALUE 1e4f
#define EPSILON 1e-5f
#define SAMPLES 1
#define BOUNCES 2

typedef struct s_scene				t_scene;

typedef struct s_sampler_manager	t_sampler_manager;

typedef struct s_ray				t_ray;
typedef struct s_hit_information	t_hit_info;
typedef struct s_shade_rec			t_shade_rec;

struct s_scene
{
	__constant t_obj		*objects;
	__constant t_triangle	*triangles;
	__constant t_light		*lights;
	int						nobjects;
	int						ntriangles;
	int						nlights;//36 + 80
	t_camera				camera;
	t_light					ambient_light;
	t_ambient_occluder		ambient_occluder;
};

struct					s_sampler_manager
{
	__global t_sampler	*samplers;
	__global float2		*samples;
	__global float2		*disk_samples;
	__global float3		*hemisphere_samples;
};

struct		s_ray
{
	float4	origin;
	float4	direction;
};

struct		s_hit_information
{
	float	t; //ray distance
	float	m;
	float	dv; //dot(ray.direction, object.direction)
	float	xv; //dot(ray.origin - object.origin, object.direction)
};

//TODO(dmelessa): change later
struct			s_shade_rec
{
	t_hit_info	hit_info;	//hit info for calculating hit point and normal
	t_ray		ray;				//for specular highlights
	float4		hit_point;		//world coordinates of hit point
	float4		local_hit_point; //for attaching textures to objects
	float4		normal;			//normal at hit point NOTE: maybe not needed here
							// and we can use local variable
	float4		direction;		//for area lights

	int			id;
	int			depth;			  //recursion depth
	bool		hit_a_triangle; //did the ray hit a triangle
	bool		hit_an_object;	  //or did the ray hit an object?
};

// t_color	float_color_multi(float	c, t_color color);
// t_color	color_sum(t_color a, t_color b);
// t_color	color_multi(t_color a, t_color b);
// t_color	lambertian_f(float kd, t_color color);
// float	glossy_specular_f(float4 camera_direction, float4 normal, float4 light_direction, float ks, float exp);
// t_color	lambertian_rho(float kd, t_color color);

// float4	get_light_direction(t_light light, t_shade_rec shade_rec);
// t_color	get_light_radiance(t_light light);

float4	get_reflected_vector(float4 l, float4 n);

// bool	is_intersect(t_ray ray, t_obj obj, t_hit_info *hit_info);
// bool	bbox_intersection(t_ray ray, t_bbox bbox);
// bool	triangle_intersection(t_ray ray, t_triangle triangle, t_hit_info *hit_info);
// float4	get_triangle_normal(t_triangle triangle);
// float4	get_object_normal(float4 point, t_obj object, t_hit_info hit_info);

// void	swap(float *a, float *b);

// float	get_random(unsigned int *seed0, unsigned int *seed1);

// int		SolveQuartic(double c[5], double s[4]);

#endif
void	swap(float *a, float *b)
{
	float tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}
/*
 *
 *  Utility functions to find cubic and quartic roots,
 *  coefficients are passed like this:
 *
 *      c[0] + c[1]*x + c[2]*x^2 + c[3]*x^3 + c[4]*x^4 = 0
 *
 *  The functions return the number of non-complex roots and
 *  put the values into the s array.
 *
 *  Author:         Jochen Schwarze (schwarze@isa.de)
 *
 *  Jan 26, 1990    Version for Graphics Gems
 *  Oct 11, 1990    Fixed sign problem for negative q's in SolveQuartic
 *  	    	    (reported by Mark Podlipec),
 *  	    	    Old-style function definitions,
 *  	    	    IsZero() as a macro
 *  Nov 23, 1990    Some systems do not declare acos() and cbrt() in
 *                  <math.h>, though the functions exist in the library.
 *                  If large coefficients are used, EQN_EPS should be
 *                  reduced considerably (e.g. to 1E-30), results will be
 *                  correct but multiple roots might be reported more
 *                  than once.
 */

/* epsilon surrounding for near zero values */

#define EQN_EPS 1e-9
#define IsZero(x) ((x) > -EQN_EPS && (x) < EQN_EPS)

int SolveQuadric(double c[3], double s[2])
{
	double p, q, D;

	/* normal form: x^2 + px + q = 0 */

	p = c[1] / (2 * c[2]);
	q = c[0] / c[2];

	D = p * p - q;

	if (IsZero(D))
	{
		s[0] = -p;
		return 1;
	}
	else if (D < 0)
	{
		return 0;
	}
	else /* if (D > 0) */
	{
		double sqrt_D = sqrt(D);

		s[0] = sqrt_D - p;
		s[1] = -sqrt_D - p;
		return 2;
	}
}

int SolveCubic(double c[4], double s[3])
{
	int i, num;
	double sub;
	double A, B, C;
	double sq_A, p, q;
	double cb_p, D;

	/* normal form: x^3 + Ax^2 + Bx + C = 0 */

	A = c[2] / c[3];
	B = c[1] / c[3];
	C = c[0] / c[3];

	/*  substitute x = y - A/3 to eliminate quadric term:
	x^3 +px + q = 0 */

	sq_A = A * A;
	p = 1.0 / 3 * (-1.0 / 3 * sq_A + B);
	q = 1.0 / 2 * (2.0 / 27 * A * sq_A - 1.0 / 3 * A * B + C);

	/* use Cardano's formula */

	cb_p = p * p * p;
	D = q * q + cb_p;

	if (IsZero(D))
	{
		if (IsZero(q)) /* one triple solution */
		{
			s[0] = 0;
			num = 1;
		}
		else /* one single and one double solution */
		{
			double u = cbrt(-q);
			s[0] = 2 * u;
			s[1] = -u;
			num = 2;
		}
	}
	else if (D < 0) /* Casus irreducibilis: three real solutions */
	{
		double phi = 1.0 / 3 * acos(-q / sqrt(-cb_p));
		double t = 2 * sqrt(-p);

		s[0] = t * cos(phi);
		s[1] = -t * cos(phi + M_PI / 3);
		s[2] = -t * cos(phi - M_PI / 3);
		num = 3;
	}
	else /* one real solution */
	{
		double sqrt_D = sqrt(D);
		double u = cbrt(sqrt_D - q);
		double v = -cbrt(sqrt_D + q);

		s[0] = u + v;
		num = 1;
	}

	/* resubstitute */

	sub = 1.0 / 3 * A;

	for (i = 0; i < num; ++i)
		s[i] -= sub;

	return num;
}

int SolveQuartic(double c[5], double s[4])
{
	double coeffs[4];
	double z, u, v, sub;
	double A, B, C, D;
	double sq_A, p, q, r;
	int i, num;

	/* normal form: x^4 + Ax^3 + Bx^2 + Cx + D = 0 */

	A = c[3] / c[4];
	B = c[2] / c[4];
	C = c[1] / c[4];
	D = c[0] / c[4];

	/*  substitute x = y - A/4 to eliminate cubic term:
	x^4 + px^2 + qx + r = 0 */

	sq_A = A * A;
	p = -3.0 / 8 * sq_A + B;
	q = 1.0 / 8 * sq_A * A - 1.0 / 2 * A * B + C;
	r = -3.0 / 256 * sq_A * sq_A + 1.0 / 16 * sq_A * B - 1.0 / 4 * A * C + D;

	if (IsZero(r))
	{
		/* no absolute term: y(y^3 + py + q) = 0 */

		coeffs[0] = q;
		coeffs[1] = p;
		coeffs[2] = 0;
		coeffs[3] = 1;

		num = SolveCubic(coeffs, s);

		s[num++] = 0;
	}
	else
	{
		/* solve the resolvent cubic ... */

		coeffs[0] = 1.0 / 2 * r * p - 1.0 / 8 * q * q;
		coeffs[1] = -r;
		coeffs[2] = -1.0 / 2 * p;
		coeffs[3] = 1;

		(void)SolveCubic(coeffs, s);

		/* ... and take the one real solution ... */

		z = s[0];

		/* ... to build two quadric equations */

		u = z * z - r;
		v = 2 * z - p;

		if (IsZero(u))
			u = 0;
		else if (u > 0)
			u = sqrt(u);
		else
			return 0;

		if (IsZero(v))
			v = 0;
		else if (v > 0)
			v = sqrt(v);
		else
			return 0;

		coeffs[0] = z - u;
		coeffs[1] = q < 0 ? -v : v;
		coeffs[2] = 1;

		num = SolveQuadric(coeffs, s);

		coeffs[0] = z + u;
		coeffs[1] = q < 0 ? v : -v;
		coeffs[2] = 1;

		num += SolveQuadric(coeffs, s + num);
	}

	/* resubstitute */

	sub = 1.0 / 4 * A;

	for (i = 0; i < num; ++i)
		s[i] -= sub;

	return num;
}
uint		random(uint2 *seed)
{
	enum { A=4294883355U };
	uint x = (*seed).x, c = (*seed).y;  // Unpack the state
	uint res = x^c;                     // Calculate the result
	uint hi = mul_hi(x, A);              // Step the RNG
	x = x * A + c;
	c = hi + (x < c);
	*seed = (uint2)(x, c);               // Pack the state back up
	return res;                       // Return the next result
}

float get_random(unsigned int *seed0, unsigned int *seed1) {

	/* hash the seeds using bitwise AND operations and bitshifts */
	*seed0 = 36969 * ((*seed0) & 65535) + ((*seed0) >> 16);
	*seed1 = 18000 * ((*seed1) & 65535) + ((*seed1) >> 16);

	unsigned int ires = ((*seed0) << 16) + (*seed1);

	/* use union struct to convert int to float */
	union {
		float f;
		unsigned int ui;
	} res;

	res.ui = (ires & 0x007fffff) | 0x40000000;  /* bitwise AND, bitwise OR */
	return (res.f - 2.0f) / 2.0f;
}
t_color	color_sum(t_color a, t_color b)
{
	t_color res;
	res.b = clamp(a.b + b.b, 0, 255);
	res.g = clamp(a.g + b.g, 0, 255);
	res.r = clamp(a.r + b.r, 0, 255);
	return (res);
}

t_color	color_multi(t_color a, t_color b)
{
	t_color	res;

	res.r = clamp(a.r * b.r / 255.0f, 0.0f, 255.0f);
	res.g = clamp(a.g * b.g / 255.0f, 0.0f, 255.0f);
	res.b = clamp(a.b * b.b / 255.0f, 0.0f, 255.0f);
	return (res);
}

t_color	float_color_multi(float	c, t_color color)
{
	t_color	res;

	res.b = clamp(c * color.b, 0.0f, 255.0f);
	res.g = clamp(c * color.g, 0.0f, 255.0f);
	res.r = clamp(c * color.r, 0.0f, 255.0f);
	return (res);
}
/**
**TODO: randomness §5
*/
float2	sample_unit_square(t_sampler *sampler, __global float2 *samples, uint2 *seed)
{
	if (sampler->count % sampler->num_samples == 0) // start of a new pixel
		sampler->jump = (random(seed) % sampler->num_sets) * sampler->num_samples;

	return ((samples + sampler->offset)[sampler->jump + sampler->count++ % sampler->num_samples]);

	// return (samples[sampler->jump + sampler->shuffled_indices[sampler->jump + sampler->count++ % sampler->num_samples]]);
}

// float2	sampler_unit_square1(__global float2 *samples, int jump);

/*
**
*/
float2	sample_unit_disk(t_sampler *sampler, __global float2 *disk_samples, uint2 *seed)
{
	if (sampler->count % sampler->num_samples == 0)
		sampler->jump = (random(seed) % sampler->num_sets) * sampler->num_samples;

	return ((disk_samples + sampler->offset)[sampler->jump + sampler->count++ % sampler->num_samples]);
}

float3	sample_hemisphere(t_sampler *sampler, __global float3 *hemisphere_samples, uint2 *seed)
{
	if (sampler->count % sampler->num_samples == 0)
		sampler->jump = (random(seed) % sampler->num_sets) * sampler->num_samples;

	return ((hemisphere_samples + sampler->offset)[sampler->jump + sampler->count++ % sampler->num_samples]);
}
t_sampler	get_sampler(t_sampler_manager sampler_manager, int sampler_id)
{
	int			i;
	int			offset;
	t_sampler	tmp;

	offset = 0;
	for (i = 0; i < sampler_id; i++)
	{
		tmp = sampler_manager.samplers[i];
		offset += tmp.num_sets * tmp.num_samples;
	}
	tmp = sampler_manager.samplers[i];
	tmp.offset = offset;
	return (tmp);
}

// #include "cl_rt.h"

t_ray cast_camera_ray(t_camera camera, float x, float y, t_sampler_manager sampler_manager,
					t_sampler *camera_sampler, uint2 *seed)
{
	t_ray	ray;
	float	px;
	float	py;
	float	pixel_size;

	pixel_size = camera.viewplane.pixel_size / camera.zoom;
	if (camera.normalized == true)
	{
		px = pixel_size * ((2.0f * x / camera.viewplane.width) - 1.0f);
		py = pixel_size * (1.0f - 2.0f * y / camera.viewplane.height);
	}
	else
	{
		px = pixel_size * (x - 0.5f * camera.viewplane.width);
		py = pixel_size * (0.5f * camera.viewplane.height - y) ;
	}

	if (camera.type == perspective)
	{
		ray.origin = camera.origin;
		ray.direction = px * camera.u + py  * camera.v - camera.d * camera.w; // camera coordinates
		ray.direction = normalize(ray.direction);
	}
	else if (camera.type == thin_lens)
	{
		float2 dp = sample_unit_disk(camera_sampler, sampler_manager.disk_samples, seed);
		float2 lp = dp * camera.l; //lens_point
		ray.origin = camera.origin + camera.u * lp.x + camera.v * lp.y;
		px = px * camera.f / camera.d;
		py = py * camera.f / camera.d;
		ray.direction = (px - lp.x) * camera.u + (py - lp.y) * camera.v - camera.w * camera.f;
		ray.direction = normalize(ray.direction);
	}
	else if (camera.type == fisheye)
	{
		ray.origin = camera.origin;

		float	r_squared = px * px + py * py;
		if (r_squared <= 1.0f)
		{
			float r = sqrt(r_squared);
			float psi = r * camera.f * 0.0174532925199432957; // PI / 180 todo: make define
			float sin_psi = sin(psi);
			float cos_psi = cos(psi);
			float sin_alpha = py / r;
			float cos_alpha = px / r;

			ray.direction = sin_psi * cos_alpha * camera.u + sin_psi * sin_alpha * camera.v - cos_psi * camera.w;
		}
		else
			ray.direction = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	}
	else if (camera.type == spherical)
	{
		ray.origin = camera.origin;

		//compute the angles lambda and phi in radians
		float	lambda = px * camera.l * 0.0174532925199432957;
		float	psi = py * camera.f * 0.0174532925199432957;

		//compute the spherical azimuth and polar angles

		float	phi = M_PI - lambda;
		float	theta = 0.5f * M_PI - psi;

		float	sin_phi = sin(phi);
		float	cos_phi = cos(phi);
		float	sin_theta = sin(theta);
		float	cos_theta = cos(theta);

		ray.direction = sin_theta * sin_phi * camera.u + cos_theta * camera.v + sin_theta * cos_phi * camera.w;
	}
	else if (camera.type == stereo) //todo: rewrite paragraph 12
	{
		float r = 10;
		float xx = r * tan(0.5f * camera.f * 0.0174532925199432957);

		if (x < (camera.viewplane.width) / 2.0f - camera.l) //right eye
		{
			px = px + camera.viewplane.width / 4;
			ray.origin = camera.origin + xx * camera.u;
			px = px - xx;
		}
		else if (x > (camera.viewplane.width) / 2.0f + camera.l) //left eye
		{
			px = px - camera.viewplane.width / 4;
			ray.origin = camera.origin - xx * camera.u;
			px = px + xx;
		}
		else
		{
			ray.direction = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
			return (ray);
		}
		ray.direction = px * camera.u + py  * camera.v - camera.d * camera.w; // camera coordinates
		ray.direction = normalize(ray.direction);
	}
	return (ray);
}
// #include "world.h"
// #include "cl_rt.h"

bool	bbox_intersection(t_ray ray, t_bbox bbox)
{
	float ox = ray.origin.x;
	float oy = ray.origin.y;
	float oz = ray.origin.z;
	float dx = ray.direction.x;
	float dy = ray.direction.y;
	float dz = ray.direction.z;
	float tx_min, ty_min, tz_min;
	float tx_max, ty_max, tz_max;
	float a = 1.0f / dx;
	if (a >= 0.0f)
	{
		tx_min = (bbox.min.x - ox) * a;
		tx_max = (bbox.max.x - ox) * a;
	}
	else
	{
		tx_min = (bbox.max.x - ox) * a;
		tx_max = (bbox.min.x - ox) * a;
	}
	float b = 1.0 / dy;
	if (b >= 0.0f)
	{
		ty_min = (bbox.min.y - oy) * b;
		ty_max = (bbox.max.y - oy) * b;
	}
	else
	{
		ty_min = (bbox.max.y - oy) * b;
		ty_max = (bbox.min.y - oy) * b;
	}

	float c = 1.0 / dz;
	if (c >= 0.0f)
	{
		tz_min = (bbox.min.z - oz) * c;
		tz_max = (bbox.max.z - oz) * c;
	}
	else
	{
		tz_min = (bbox.max.z - oz) * c;
		tz_max = (bbox.min.z - oz) * c;
	}
	float t0, t1;
	if (tx_min > ty_min)
		t0 = tx_min;
	else
		t0 = ty_min;

	if (tz_min > t0)
		t0 = tz_min;

	if (tx_max < ty_max)
		t1 = tx_max;
	else
		t1 = ty_max;
	if (tz_max < t1)
		t1 = tz_max;

	return (t0 < t1 && t1 > 1e-6);
}

 bool	sphere_intersection(t_ray ray, t_obj sphere, t_hit_info *hit_info)
{
#if 1 // geometric solution

	float4	L = sphere.origin - ray.origin;
	float	tca = dot(L, ray.direction);
	if (tca < 0.0f)
		return false;

	float	d2 = dot(L, L) - tca * tca;
	if (d2 > sphere.r2) //r^2 should be precomputed
		return false;

	float	thc = sqrt(sphere.r2 - d2);
	float	t = tca - thc;
	if (t < 0.0f)
	{
		t = tca + thc;
		if (t < 0.0f)
			return false;
	}
	hit_info->t = t;
	return (true);

#elif 1// analytic solution

	float4 L = sphere.origin - ray.origin;
	float a = 1; //NOTE: if ray.direction normalized!
	float b = dot(ray.direction, L); //b/2
	float c = dot(L, L) - sphere.r2;
	float disc = b * b - c; // DISC = (b/2)^2-ac

	if (disc < 0.0f)
		return false;
	a = sqrt(disc);
	hit_info->t = b - a;
	if (hit_info->t < 0.0f)
	{
		hit_info->t = b + a;
		if (hit_info->t < 0.0f)
			return false;
	}
	return true;
#endif
}

/**
** there could be mistake
** need some tests
*/
 bool	plane_intersection(t_ray ray, t_obj plane, t_hit_info *hit_info)
{
	float4	a;
	float	t;
	float	denom;
	bool	ret;

	ret = false;
	denom = dot(ray.direction, plane.direction);
	if (denom != 0)
	{
		a = plane.origin - ray.origin;
		t = dot(a, plane.direction);
		if (t * denom > 0.0f) //different signes
		{
			hit_info->t = t / denom;
			if (hit_info->t >= EPSILON)
			{
				hit_info->dv = denom;
				ret = true;
			}
		}
	}
	return (ret);
}

/*
** TODO(dmelessa): cap cylinder with discs
*/
 bool cylinder_intersection(t_ray ray, t_obj cylinder, t_hit_info *hit_info)
{
	float4	x;
	float	a, b, c, dv, xv, disc;

	x = ray.origin - cylinder.origin;
	dv = dot(ray.direction, cylinder.direction);
	xv = dot(x, cylinder.direction);
	a = 1.0f - dv * dv; // 1 if ray direction is normalized else dot(d,d)
	b = 2.0f * (dot(ray.direction, x) - dv * xv);
	c = dot(x, x) - xv * xv - cylinder.r2;
	disc = b * b - 4.0f * a * c;
	if (disc >= EPSILON)
	{
		a *= 2.0f;
		disc = sqrt(disc);
		hit_info->t = (-b - disc) / a;
		if (hit_info->t < EPSILON)
			hit_info->t = (-b + disc) / a;
		if (hit_info->t > EPSILON)
		{
			if (cylinder.maxm > 0.0f)
			{
				float m1 = dv * hit_info->t + xv;
				if (m1 < cylinder.maxm && m1 > EPSILON)
				{
					hit_info->m = m1;
					hit_info->dv = dv;
					hit_info->xv = xv;
					return true;
				}
				hit_info->t = (-b + disc) / a;
				hit_info->m = dv * hit_info->t + xv;
				if (hit_info->m >= cylinder.minm + EPSILON &&
					hit_info->m <= cylinder.maxm)
				{
					hit_info->dv = dv;
					hit_info->xv = xv;
					return true;
				}
			}
			else
			{
				hit_info->dv = dv;
				hit_info->xv = xv;
				return true;
			}
		}
	}
	return false;
}

/*
** TODO(dmelessa): cap cone with disc
*/
 bool	cone_intersection(t_ray ray, t_obj cone, t_hit_info *hit_info)
{
	float4	x;
	float	dv, xv, a, b, c, disc;

	x = ray.origin - cone.origin;
	dv = dot(ray.direction, cone.direction);
	xv = dot(x, cone.direction);
	a = dot(ray.direction, ray.direction) - cone.r2 * dv * dv;
	b = 2.0f * (dot(ray.direction, x) - cone.r2 * dv * xv);
	c = dot(x, x) - cone.r2 * xv * xv;
	disc = b * b - 4 * a * c;
	if (disc >= EPSILON)
	{
		a *= 2.0f;
		disc = sqrt(disc);
		hit_info->t = (-b - disc) / a;
		if (hit_info->t < EPSILON)
			hit_info->t = (-b + disc) / a;
		if (hit_info->t > EPSILON)
		{
			if (cone.maxm != 0.0f || cone.minm != 0)
			{
				hit_info->m = dv * hit_info->t + xv;
				if (hit_info->m >= cone.minm + EPSILON &&
					hit_info->m <= cone.maxm)
				{
					hit_info->dv = dv;
					hit_info->xv = xv;
					return true;
				}
				hit_info->t = (-b + disc) / a;
				hit_info->m = dv * hit_info->t + xv;
				if (hit_info->m >= cone.minm + EPSILON &&
					hit_info->m <= cone.maxm)
				{
					hit_info->dv = dv;
					hit_info->xv = xv;
					return true;
				}
			}
			else
			{
				hit_info->dv = dv;
				hit_info->xv = xv;
				return (true);
			}
		}
	}
	return (false);
}

 bool	paraboloid_intersection(t_ray ray, t_obj paraboloid, t_hit_info *hit_info)
{
	float4	x;
	float	a, b, c, dv, xv, disc;

	x = ray.origin - paraboloid.origin;
	dv = dot(ray.direction, paraboloid.direction);
	xv = dot(x, paraboloid.direction);
	a = 1.0f - dv * dv;
	b = 2.0f * (dot(ray.direction, x) - dv * (xv + 2.0f * paraboloid.r));
	c = dot(x, x) - xv * (xv + 4.0f * paraboloid.r);
	disc = b * b - 4.0f * a * c;
	if (disc >= EPSILON)
	{
		a *= 2;
		disc = sqrt(disc);
		hit_info->t = (-b - disc) / a;
		if (hit_info->t < EPSILON)
			hit_info->t = (-b + disc) / a;
		if (hit_info->t > EPSILON)
		{
			if (paraboloid.maxm > 0.0f)
			{
				hit_info->m = dv * hit_info->t + xv;
				if (hit_info->m >= paraboloid.minm &&
					hit_info->m <= paraboloid.maxm)
					return true;
				hit_info->t = (-b + disc) / a;
				hit_info->m = dv * hit_info->t + xv;
				if (hit_info->m >= paraboloid.minm &&
					hit_info->m <= paraboloid.maxm)
					return true;
			}
			else
			{
				hit_info->dv = dv;
				hit_info->xv = xv;
				return (true);
			}
		}
	}
	return (false);
}

 bool	torus_intersecion(t_ray ray, t_obj torus, t_hit_info *hit_info)
{
	if (!bbox_intersection(ray, torus.bounding_box))
		return false;
	double	coeffs[5];
	double	roots[4];
	float4	x;
	float	m, n, o, p, q, r2, R2;
	int		num_real_roots;

	x = ray.origin - torus.origin;
	m = dot(ray.direction, ray.direction);
	n = dot(ray.direction, x);
	o = dot(x, x);
	p = dot(ray.direction, torus.direction);
	q = dot(x, torus.direction);
	r2 = torus.r2 * torus.r2;
	R2 = torus.r * torus.r;

	coeffs[4] = m * m; //a
	coeffs[3] = 4.0f * n * m; //b
	coeffs[2] = 4.0f * n * n + 2.0f * m * o - 2.0f * (R2 + r2) * m + 4.0f * R2 * p * p;
	coeffs[1] = 4.0f * n * o - 4.0f * (R2 + r2) * n + 8.0f * R2 * p * q;
	coeffs[0] = o * o - 2.0f * (R2 + r2) * o + 4 * R2 * q * q + (R2 - r2) * (R2 - r2);
	num_real_roots = SolveQuartic(coeffs, roots);
	bool	intersect = false;
	float	t;

	if (num_real_roots == 0)
		return false;

	t = 1000.0f;
	for (int j = 0; j < num_real_roots; j++)
	{
		if (roots[j] > EPSILON)
		{
			intersect = true;
			if (roots[j] < t)
			{
				t = roots[j];
			}
		}
	}
	if (!intersect)
		return false;
	hit_info->t = t;
	return (intersect);
}

/*
** TODO(dmelessa): change later
*/
bool	triangle_intersection(t_ray ray, t_triangle triangle,
								t_hit_info *hit_info)
{
	float4	pvec = cross(ray.direction, triangle.vector2);
	float	det = dot(triangle.vector1, pvec);

	if (det < 1e-8 && det > -1e-8)
		return false;

	float	inv_det = 1.0f / det;
	float4	tvec = ray.origin - triangle.vertex1;
	float	u = dot(tvec, pvec) * inv_det;
	if (u < EPSILON || u > 1)
		return false;

	float4	qvec = cross(tvec, triangle.vector1);
	float	v = dot(ray.direction, qvec) * inv_det;
	if (v < EPSILON || u + v > 1)
		return false;
	hit_info->t = dot(triangle.vector2, qvec) * inv_det;
	return hit_info->t > EPSILON;
}

bool	is_intersect(t_ray ray, t_obj obj, t_hit_info *hit_info)
{
	if (obj.type == sphere)
	{
		return (sphere_intersection(ray, obj, hit_info));
	}
	else if (obj.type == plane)
	{
		return (plane_intersection(ray, obj, hit_info));
	}
	else if (obj.type == cylinder)
	{
		return cylinder_intersection(ray, obj, hit_info);
	}
	else if (obj.type == cone)
	{
		return (cone_intersection(ray, obj, hit_info));
	}
	else if (obj.type == paraboloid)
	{
		return (paraboloid_intersection(ray, obj, hit_info));
	}
	else if (obj.type == torus)
	{
		return torus_intersecion(ray, obj, hit_info);
	}
	return (false);
}
float4	get_sphere_normal(float4 point, t_obj sphere)
{
	return (normalize(point - sphere.origin));
}

float4	get_plane_normal(t_obj plane, t_hit_info hit_info)
{
	if (hit_info.dv < 0.0F)
		return plane.direction;
	else
		return -(plane.direction);
}

float4	get_cylinder_normal(float4 point, t_obj cylinder, t_hit_info hit_info)
{
	float m = hit_info.dv * hit_info.t + hit_info.xv;
	return (normalize(point - cylinder.origin - cylinder.direction * m));
}

float4	get_cone_normal(float4 point, t_obj cone, t_hit_info hit_info)
{
	float m = hit_info.dv * hit_info.t + hit_info.xv;
	return (normalize(point - cone.origin - cone.r2 * cone.direction * m));
}

float4	get_paraboloid_normal(float4 point, t_obj paraboloid, t_hit_info hit_info)
{
	if (paraboloid.maxm > 0.0f)
		return (normalize(point - paraboloid.origin - paraboloid.direction *
			(hit_info.m + paraboloid.r)));
	float m = hit_info.dv * hit_info.t + hit_info.xv;
	return (normalize(point - paraboloid.origin - paraboloid.direction * (m +
		paraboloid.r)));
}

 float4	get_triangle_normal(t_triangle triangle)
{
	return (triangle.normal);
}

float4	get_torus_normal(float4 point, t_obj torus, t_hit_info hit_info)
{
	float	k;
	float	m;
	float4	A;

	point = 1.0001f * point;
	k = dot(point - torus.origin, torus.direction);
	A = point - k * torus.direction;
	m = sqrt(torus.r2 * torus.r2 - k * k);
	return normalize(point - A - (torus.origin - A) * (m / (torus.r + m)));
}

float4	get_object_normal(float4 point, t_obj object, t_hit_info hit_info)
{
	if (object.type == sphere)
	{
		return get_sphere_normal(point, object);
	}
	else if (object.type == plane)
	{
		return (get_plane_normal(object, hit_info));
	}
	else if (object.type == cylinder)
	{
		return (get_cylinder_normal(point, object, hit_info));
	}
	else if (object.type == cone)
	{
		return (get_cone_normal(point, object, hit_info));
	}
	else if (object.type == paraboloid)
	{
		return (get_paraboloid_normal(point, object, hit_info));
	}
	else if (object.type == torus)
	{
		return (get_torus_normal(point, object, hit_info));
	}
}
// #include "world.h"
// #include "cl_rt.h"

 float4	get_light_direction(t_light light, t_shade_rec shade_rec)
{
	if (light.type == ambient)
		return ((float4)(0.0f, 0.0f, 0.0f, 0.0f));
	else if (light.type == point)
		return (normalize(light.origin - shade_rec.hit_point));
	else if (light.type == directional)
		return -normalize(light.direction);
}

 t_color	get_light_radiance(t_light light)
{
	t_color	color;

	if (light.type == ambient)
	{
		color.r = clamp(light.color.r * light.ls, 0.0f, 255.0f);
		color.g = clamp(light.color.g * light.ls, 0.0f, 255.0f);
		color.b = clamp(light.color.b * light.ls, 0.0f, 255.0f);
		return (color);
	}
}
/*
** BRDF - bidirectional reflectance distributuin functions
** the function f returns the BRDF itself, unless it contains the delta function
**
** the function sample_f is used to compute the direction of reflected rays for
** simulating reflective materials and diffuse-diffuse light transport
**
** the function rho returns the bihemispherical reflectance
*/

t_color	lambertian_f(float kd, t_color color)
{
	return (float_color_multi(kd * (float)M_1_PI, color));
}

t_color	lambertian_rho(float kd, t_color color)
{
	t_color	res;

	res = float_color_multi(kd, color);
	return (res);
}

float	glossy_specular_f(float4 camera_direction, float4 normal, float4 light_direction, float ks, float exp)
{
	float	res = 0;
	float4	r = get_reflected_vector(light_direction, normal);
	float	rdotdir = dot(r, camera_direction);

	if (rdotdir > 0)
		res = ks * pow(rdotdir, exp);
	return res;
}
float4	get_ambient_occluder_direction(t_ambient_occluder ambient_occluder,
										t_sampler_manager sampler_manager,
										t_sampler *sampler,
										uint2 *seed)
{
	float3 sp = sample_hemisphere(sampler, sampler_manager.hemisphere_samples, seed);
	return (sp.x *ambient_occluder.u + sp.y * ambient_occluder.v + sp.z * ambient_occluder.w);
}

bool	in_shadow(t_ray shadow_ray, t_scene scene)
{
	t_hit_info ht;

	for (int i = 0; i < scene.nobjects; i++)
	{
		if (is_intersect(shadow_ray, scene.objects[i], &ht))
			return (true);
	}
	return false;
}

t_color	ambient_occlusion_l(t_scene scene,
							t_sampler_manager sampler_manager,
							t_sampler *sampler,
							t_shade_rec shade_rec,
							uint2 *seed)
{
	t_color	color;

	scene.ambient_occluder.w = shade_rec.normal;
	scene.ambient_occluder.v = normalize(cross(scene.ambient_occluder.w, (float4)(0.0072f, 1.0f, 0.0034f, 0.0f)));
	scene.ambient_occluder.u = cross(scene.ambient_occluder.v, scene.ambient_occluder.w);

	t_ray shadow_ray;
	shadow_ray.origin = shade_rec.hit_point;
	shadow_ray.direction = get_ambient_occluder_direction(scene.ambient_occluder, sampler_manager, sampler, seed);
	color = float_color_multi(scene.ambient_occluder.ls, scene.ambient_occluder.color);
	if (in_shadow(shadow_ray, scene))
		color = float_color_multi(0.3, color);
	return (color);
}

bool	shadow_hit_object(t_ray shadow_ray, t_obj obj, t_hit_info *hit_info)
{
	if (!obj.shadows)
		return (false);
	return (is_intersect(shadow_ray, obj, hit_info));
}

//TODO: triangles
bool	shadow_hit(t_light light, t_ray shadow_ray, t_shade_rec shade_rec, t_scene scene)
{
	float	t;
	float	d = distance(light.origin, shadow_ray.origin);

	for (int i = 0; i < scene.nobjects; i++)
	{
		if (shadow_hit_object(shadow_ray, scene.objects[i], &shade_rec.hit_info)
			&& shade_rec.hit_info.t < d)
			return (true);
	}
	return (false);
}

/*
lights can optionally cast shadows
objects can optionally cast shadows
materials can optionally cast shadows
*/
//TODO(dmelessa): light colors
//note we don't need pass an object. We can pass  only material and compute normal
//in function before
t_color		shade_object(t_material material,
						t_shade_rec shade_rec,
						t_scene scene,
						t_sampler_manager sampler_manager,
						t_render_options options,
						uint2 *seed)
{
	float4	light_direction;
	float	dirdotn;
	t_color	color_tmp;
	t_color	color;

	// return material.color;

	/* revert camera ray for specular light */
	shade_rec.ray.direction = -shade_rec.ray.direction;


	if (options.ambient_occlusion) /* ambient occlusion */
	{
		color = ambient_occlusion_l(scene, sampler_manager, &options.ambient_occluder_sampler, shade_rec, seed);
		color = color_multi(color, material.color);
	}
	else /* compute constant ambient light using ka coefficent of the materail */
	{
		color = lambertian_rho(material.ka, material.color);
		color_tmp = get_light_radiance(scene.ambient_light);
		color = color_multi(color, color_tmp);
	}

	/* compute sahding for each light source */
	for (int i = 0; i < scene.nlights; i++)
	{
		bool	in_shadow = false;

		/* compute light direction at hit point */
		light_direction = get_light_direction(scene.lights[i], shade_rec);

		/* multiplying by 0.999f to avoid self shadowing error */
		t_ray	shadow_ray = {.origin = shade_rec.hit_point * 0.999f, .direction = light_direction };

		if (options.shadows)
			in_shadow = shadow_hit(scene.lights[i], shadow_ray, shade_rec, scene);

		if (!in_shadow)
		{
			/* compute angle between normal at the hit point and light direction */
			dirdotn = dot(shade_rec.normal, light_direction);

			/* if angle > 0 then hit point is receivingl light */
			if (dirdotn > 0.0f)
			{
				/* compute glossy_specular coefficient */
				float a = glossy_specular_f(shade_rec.ray.direction, shade_rec.normal, light_direction, material.ks, material.exp) ;

				/* compute diffuse color */
				color_tmp = lambertian_f(material.kd, material.color);

				/* sum lambertian color and glossy specular color */
				color_tmp = color_sum(color_tmp, float_color_multi(a, scene.lights[i].color));

				/* compute how much light the point receives depends on angle between the normal at this point and light direction */
				color_tmp.r = clamp(scene.lights[i].ls * scene.lights[i].color.r * color_tmp.r / 255.0f * dirdotn, 0.0f, 255.0f);
				color_tmp.b = clamp(scene.lights[i].ls * scene.lights[i].color.b * color_tmp.b / 255.0f * dirdotn, 0.0f, 255.0f);
				color_tmp.g = clamp(scene.lights[i].ls * scene.lights[i].color.g * color_tmp.g / 255.0f * dirdotn, 0.0f, 255.0f);
				color = color_sum(color_tmp, color);
			}
		}
	}
	return (color);
}
float4	get_reflected_vector(float4 l, float4 n)
{
	float4	r;

	r = -l + 2 * dot(l, n) * n;
	return normalize(r);
}

//TODO(dmelessa): shading both sides of surface ยง14
bool	scene_intersection(t_ray ray, t_shade_rec *shade_rec, t_scene scene)
{
	float		t;
	t_hit_info	last_rec;

	last_rec.t = K_HUGE_VALUE;
	shade_rec->hit_an_object = false;
	shade_rec->hit_a_triangle = false;

	for (int i = 0; i < scene.nobjects; i++)
	{
		if (is_intersect(ray, scene.objects[i], &shade_rec->hit_info)
			&& shade_rec->hit_info.t < last_rec.t)
		{
			shade_rec->hit_an_object = true;
			last_rec = shade_rec->hit_info;
			shade_rec->id = i;
		}
	}

	for (int i = 0; i < scene.ntriangles; i++)
	{
		if (triangle_intersection(ray, scene.triangles[i], &shade_rec->hit_info)
			&& shade_rec->hit_info.t < last_rec.t)
		{
			shade_rec->hit_a_triangle = true;
			last_rec = shade_rec->hit_info;
			shade_rec->id = i;
		}
	}

	shade_rec->hit_info = last_rec;
	return (shade_rec->hit_an_object || shade_rec->hit_a_triangle);
}

//todo: move normal computation to ray_trace_function
t_color	ray_trace(t_ray ray, t_scene scene, t_render_options options, t_sampler_manager sampler_manager, uint2 *seed)
{
	t_shade_rec	shade_rec;
	t_color		color;

	color.value = options.background_color.value;
	if (scene_intersection(ray, &shade_rec, scene))
	{
		/* save ray for specular reflection */
		shade_rec.ray = ray;

		/* compute hit point */
		shade_rec.hit_point = (shade_rec.hit_info.t) * shade_rec.ray.direction + shade_rec.ray.origin;

		/* shade ovject if it was triangle */
		if (shade_rec.hit_a_triangle)
		{
			/* compute triangle normal */
			shade_rec.normal = get_triangle_normal(scene.triangles[shade_rec.id]);

			/* shade triangle */
			color = shade_object(scene.triangles[shade_rec.id].material, shade_rec, scene, sampler_manager,options, seed);
		}

		/* shade object if it was not triangle */
		else
		{
			/* get object normal */
			shade_rec.normal = get_object_normal(shade_rec.hit_point,
				scene.objects[shade_rec.id], shade_rec.hit_info);
			shade_rec.normal = dot(shade_rec.normal, ray.direction) < 0.0f ?
				shade_rec.normal : -shade_rec.normal;

			/* shade object */
			color = shade_object(scene.objects[shade_rec.id].material, shade_rec, scene, sampler_manager, options, seed);
		}
	}
	return (color);
}
// #include "cl_rt.h"

//С накоплением цвета проблемы. Что такое эмиссия? И не понятно что на что умножать
float3		path_tracing(t_ray camray, t_scene scene, t_render_options options,
						unsigned int *seed0, unsigned int *seed1)
{
	t_ray	ray = camray;
	float3	accum_color = (float3)(0.0f, 0.0f, 0.0f);
	float3	mask = (float3)(1.0f, 1.0f, 1.0f);

	for (int bounces = 0; bounces < BOUNCES; bounces++)
	{
		t_shade_rec	shade_rec;

		// float	t;
		// int		hitobj_id = 0;

		if (!scene_intersection(ray, &shade_rec, scene))
		// if (!intersect_scene(objects, ray, &t, &hitobj_id, nobjects))
			return accum_color += mask * (float3)(0.05f, 0.05f, 0.05f);

		t_obj	hit_obj = scene.objects[shade_rec.id];

		/* compute the hitpoint using the ray equation */
		// float4	hitpoint = ray.origin + ray.direction * t;

		shade_rec.hit_point = (shade_rec.hit_info.t) * shade_rec.ray.direction + shade_rec.ray.origin;

		/* compute the surface normal and flip it if necessary to face the incoming ray */
		float4	normal = shade_rec.hit_a_triangle ?
							get_triangle_normal(scene.triangles[shade_rec.id]) :
							get_object_normal(shade_rec.hit_point, scene.objects[shade_rec.id], shade_rec.hit_info);
		float4 normal_facing = dot(normal, ray.direction) < 0.0f ? normal : -normal;

		/* compute two random numbers to pick a random point on the hemisphere above the hitpoint*/
		float rand1 = 2.0f * M_PI * get_random(seed0, seed1);
		float rand2 = get_random(seed0, seed1);
		float rand2s = sqrt(rand2);

		/* create a local orthogonal coordinate frame centered at the hitpoint */
		float4 w = normal_facing;
		float4 axis = fabs(w.x) > 0.1f ? (float4)(0.0f, 1.0f, 0.0f, 0.0f) : (float4)(1.0f, 0.0f, 0.0f, 0.0f);
		float4 u = normalize(cross(axis, w));
		float4 v = cross(w, u);

		/* use the coordinte frame and random numbers to compute the next ray direction */
		float4 newdir = normalize(u * cos(rand1)*rand2s + v*sin(rand1)*rand2s + w*sqrt(1.0f - rand2));

		/* add a very small offset to the hitpoint to prevent self intersection */
		// ray.origin = hitpoint + normal_facing * EPSILON;
		ray.origin = shade_rec.hit_point + normal_facing * EPSILON;
		ray.direction = newdir;

		/* add the colour and light contributions to the accumulated colour */
		// accum_color += mask * hit_obj.material.emission;

		/* the mask colour picks up surface colours at each bounce */
		// mask *= hit_obj.material.color;

		/* perform cosine-weighted importance sampling for diffuse surfaces*/
		mask *= dot(newdir, normal_facing);
	}

	return accum_color;
}
/**
**@brief
** получить цвет пикселя в позиции x,y экрана, используя камеру camera
** и объекты сцены
*/
int		get_pixel_color(int x, int y,
						t_scene scene,
						t_render_options options,
						t_sampler_manager sampler_manager,
						uint2 *seed)
{
	t_ray			ray;
	t_color			color;
	float3			test = (float3)(0.0f, 0.0f, 0.0f);
	t_sampler		ao_sampler;
	t_sampler		camera_sampler;

	color.value = 0;
	ao_sampler = get_sampler(sampler_manager, options.sampler_id);
	// ao_sampler.type = none;
	if (scene.camera.type == thin_lens)
	{
		camera_sampler = get_sampler(sampler_manager, scene.camera.sampler_id);
		camera_sampler.count = get_global_id(0) * camera_sampler.num_samples;
	}
	if (0) //path tracing
	{
		ray = cast_camera_ray(scene.camera, x, y, sampler_manager, 0, seed);
		for (int i = 0; i < SAMPLES; i++)
			test += path_tracing(ray, scene, options, &x, &y);
		color.r = test.x / SAMPLES * 255;
		color.g = test.y / SAMPLES * 255;
		color.b = test.z / SAMPLES * 255;
	}
	else if (ao_sampler.type == none) //без антиалиасинга
	{
		ray = cast_camera_ray(scene.camera, x, y, sampler_manager, &camera_sampler, seed);
		// color = ray_trace(ray, scene, options);
	}
	else //антиалиасинг сгенерированный заранее на цпу
	{
		ao_sampler.count = get_global_id(0) * ao_sampler.num_samples;
		float2	sp = sample_unit_square(&ao_sampler, sampler_manager.samples, seed);
		float	dx = x + sp.x;
		float	dy = y + sp.y;

		for (int j = 0; j < ao_sampler.num_samples; j++)
		{
			sp = sample_unit_square(&ao_sampler, sampler_manager.samples, seed);
			dx = x + sp.x;
			dy = y + sp.y;

			ray = cast_camera_ray(scene.camera, dx, dy, sampler_manager, &camera_sampler, seed);
			// color = ray_trace(ray, scene, options);

			/* accumulate color */
			test.x += color.r;
			test.y += color.g;
			test.z += color.b;
		}

		/* normalize accumulated color */
		color.r = test.x / ao_sampler.num_samples;
		color.g = test.y / ao_sampler.num_samples;
		color.b = test.z / ao_sampler.num_samples;
	}
	return (color.value);
}

void	init_scene(t_scene *scene, __constant t_obj *objects, int nobjects,
					__constant t_triangle *triangles, int ntriangles,
					__constant t_light *lights, int nlights,
					t_camera camera, t_light ambient_light,
					t_ambient_occluder ambient_occluder)
{
	scene->objects = objects;
	scene->nobjects = nobjects;
	scene->triangles = triangles;
	scene->ntriangles = ntriangles;
	scene->lights = lights;
	scene->nlights = nlights;
	scene->camera = camera;
	scene->ambient_light = ambient_light;
	scene->ambient_occluder = ambient_occluder;
}

void	init_sampler_manager(t_sampler_manager *sampler_manager,
							__global t_sampler *samplers,
							__global float2 *samples,
							__global float2 *disk_samples,
							__global float3 *hemisphere_samples)
{
	sampler_manager->samplers = samplers;
	sampler_manager->samples = samples;
	sampler_manager->disk_samples = disk_samples;
	sampler_manager->hemisphere_samples = hemisphere_samples;
}

/*
** Поскольку драйвер убивает кернел если он выполняется дольше чем 0,5 секунды,
** нам надо  его уменьшить. При каждом вызове будет считать по одной точке антиалиасинга.
** В дальнейшем когда будут сделаны материалы / преломления / отражения можно будет еще  ** больше разбить кернел чтобы избежать дивергенции кода.
*/
__kernel void	main(__global float3 *image,
				int step,

				__constant t_obj *objects, int nobjects,
				__constant t_triangle *triangles, int ntriangles,
				__constant t_light *lights, int nlights,
				t_camera camera,
				t_light ambient_light,
				t_ambient_occluder ambient_occluder,

				t_render_options options,

				__global t_sampler *samplers,
				__global float2 *samples,
				__global float2 *disk_samples,
				__global float3 *hemisphere_samples)
{
	int					global_id;
	int					x;
	int					y;
	uint2				seed;
	t_scene				scene;
	t_sampler_manager	sampler_manager;

	t_ray				ray;
	t_color				color;
	t_sampler			camera_sampler;
	t_sampler			ao_sampler;

	/* init some variables */
	global_id = get_global_id(0);
	x = global_id % camera.viewplane.width;
	y = global_id / camera.viewplane.width;
	seed.x = global_id;
	seed.y = get_local_id(0) + get_group_id(0);
	seed.y = random(&seed);
	init_scene(&scene, objects, nobjects, triangles, ntriangles, lights, nlights, camera, ambient_light, ambient_occluder);
	init_sampler_manager(&sampler_manager, samplers, samples, disk_samples, hemisphere_samples);

	ao_sampler = get_sampler(sampler_manager, options.sampler_id);
	ao_sampler.count = global_id * ao_sampler.num_samples + step;

	if (step != 0)
	{
		ao_sampler.jump = (random(&seed) % ao_sampler.num_sets) * ao_sampler.num_samples;
		options.ambient_occluder_sampler.jump = (random(&seed) % options.ambient_occluder_sampler.num_sets) * options.ambient_occluder_sampler.num_samples;
	}
	else
		image[global_id] = (float3)(0, 0, 0);

	float2	sp = sample_unit_square(&ao_sampler, sampler_manager.samples, &seed);
	float	dx = x + sp.x;
	float	dy = y + sp.y;

	if (scene.camera.type == thin_lens)
	{
		camera_sampler = get_sampler(sampler_manager, scene.camera.sampler_id);
		camera_sampler.count = global_id * camera_sampler.num_samples + step;
		if (step != 0)
			camera_sampler.jump = (random(&seed) % camera_sampler.num_sets) * camera_sampler.num_samples;
	}

	ray = cast_camera_ray(scene.camera, dx, dy, sampler_manager, &camera_sampler, &seed);
	color = ray_trace(ray, scene, options, sampler_manager, &seed);

	float3 rgb;
	rgb.x = color.r;
	rgb.y = color.g;
	rgb.z = color.b;
	image[global_id] += rgb;
}

__kernel void del(
				__global uint *output_image,

				__constant t_obj *objects, int nobjects,
				__constant t_triangle *triangles, int ntriangles,
				__constant t_light *lights, int nlights,
				t_camera camera,
				t_light ambient_light,

				t_render_options options,

				__global t_sampler *samplers,
				__global float2 *samples,
				__global float2 *disk_samples,
				__global float3 *hemisphere_samples)
{
	uint2				seed;
	int					global_id;
	int					x;
	int					y;
	t_scene				scene;
	t_sampler_manager	sampler_manager;

	global_id = get_global_id(0);
	x = global_id % camera.viewplane.width;
	y = global_id / camera.viewplane.height;
	seed.x = global_id;
	seed.y = get_local_id(0);
	t_ambient_occluder ambient_occluder;
	// seed.y = random(&seed);

	init_scene(&scene, objects, nobjects, triangles, ntriangles, lights, nlights, camera, ambient_light, ambient_occluder);
	init_sampler_manager(&sampler_manager, samplers, samples, disk_samples, hemisphere_samples);

	// put_pixel(image, x, y, color);
	output_image[global_id] = get_pixel_color(x, y, scene, options, sampler_manager, &seed);
}
__kernel void translate_image(__global float3 *float3_image,
							__global uint *uint_image,
							int num_samples)
{
	t_color	color;
	float3	rgb;
	int		global_id = get_global_id(0);

	rgb = float3_image[global_id];
	color.r = rgb.x / num_samples;
	color.g = rgb.y / num_samples;
	color.b = rgb.z / num_samples;

	uint_image[global_id] = color.value;
}
