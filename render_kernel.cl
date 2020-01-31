#include "defines.h"

typedef enum	e_objs
{
	none,
	sphere,
	plane,
	cone,
	cylinder,
	light,
	camera
}				t_obj_type;

typedef struct	s_ray
{
	float4	origin;
	float4	dir;
}				t_ray;

typedef struct	s_material
{
	int4	color;
	float	kd;
	float	ks;
	float	n;
}				t_material;

typedef struct	s_obj
{
	int			type;
	float4		c_s;
	float4		origin;
	t_material	mat;
	float4		dir;
	float		r;
	float		r2;
	float		angle;
	
}				t_obj;

typedef struct	s_camera
{
	float4		origin;
	float4		orient;
	int			fov;
	float		near_z;
	float		far_z;
	float		inv_w;
	float		inv_h;
	float		ratio;
	float		angle;
}				t_camera;

typedef	struct	s_light_source
{
	float4		origin;
	float		intensity;
	float4		c_s;
}				t_light_source;

typedef struct		s_world
{
	t_obj			*objs;
	int				nobjs;
	t_light_source	*lights;
	int				nlights;
}					t_world;

typedef struct	s_hit_info
{
	float4		hit_point;
	float4		surf_normal;
	float		t;
}				t_hit_info;

t_world	new_world(t_obj objs, const int nobjs, t_light_source lights, const int nlights);
float	noise(float x, float y, float z);
t_ray	create_cam_ray(const float x, const float y, t_camera cam);
void	get_hit_point(__constant t_obj *obj, t_ray *ray, t_hit_info *info);
bool	sphere_intersection(__constant t_obj *sp, t_ray *ray, t_hit_info * info);
bool	plane_intersection(__constant t_obj *pl, t_ray *ray, t_hit_info * info);
bool	cone_intersection(__constant t_obj *cn, t_ray *ray, t_hit_info * info);
bool	cylinder_intersection(__constant t_obj *cl, t_ray *ray, t_hit_info * info);
bool		is_intersect(__constant t_obj *obj, t_ray *ray, t_hit_info * info);
void		get_sphere_normal(__constant t_obj *obj, t_hit_info *info);
void		get_plane_normal(__constant t_obj *obj, t_hit_info *info);
void		get_cone_normal(__constant t_obj *obj, t_hit_info *info);
void		get_cylinder_normal(__constant t_obj *obj, t_hit_info *info);
void		get_surface_normal(__constant t_obj *obj, t_hit_info *info);
__constant t_obj	*get_intersection(__constant t_obj *objs, int nobjs, t_ray *ray, t_hit_info *info);
t_ray		cast_shadow_ray(float4 start, __constant t_light_source *light);
int4	calc_color(__constant t_obj *obj, float diff_light, float spec_light);
float4		reflect(float4 i, float4 n);
int4	get_point_color(__constant t_obj *objs, const int nobjs,
				__constant t_light_source *lights, const int nlights,
				__constant t_obj *obj, t_ray *ray, int i, t_hit_info *info);
int4	trace_ray(t_ray *ray, __constant t_obj *objs, const int nobjs,
				__constant t_light_source *lights, const int nlights);

float	noise(float x, float y, float z)
{
	float num = 0.0f;
	return fract(sin(x * 112.9898f + y * 179.233f + z * 237.212f) * 43758.5453f, &num);
}

void	printfl(float4 data)
{
	printf("%f %f %f %f\n", data.x, data.y, data.z, data.w);
}

t_ray	create_cam_ray(const float x, const float y, t_camera cam)
{
	t_ray	ray;
	float3	tmp;

	ray.dir = (float4)(1.0f, 1.0f, 1.0f, 0.0f);
	ray.dir.y = (1 - 2 * ((y + 0.5) * cam.inv_h)) * cam.angle;
	ray.dir.x = (2 * ((x + 0.5) * cam.inv_w) - 1) * cam.angle * cam.ratio;
	tmp = normalize((float3)(ray.dir.x, ray.dir.y, ray.dir.z));
	ray.dir.x = tmp.x;
	ray.dir.y = tmp.y;
	ray.dir.z = tmp.z;
	return ray;
}

void	get_hit_point(__constant t_obj *obj, t_ray *ray, t_hit_info *info)
{
	info->hit_point = ray->dir * info->t;
	info->hit_point = ray->origin + info->hit_point;
	// printf("%f\n", info->t);
	// printf("%f %f %f %f\n", info->hit_point.x, info->hit_point.y, info->hit_point.z, info->hit_point.w);
}

bool	sphere_intersection(__constant t_obj *sp, t_ray *ray, t_hit_info *info)
{
	float4 rayToCenter = sp->c_s - ray->origin;
	float4 tmp = normalize(rayToCenter);
	// printf("%f, %f, %f\n", rayToCenter.x, rayToCenter.y, rayToCenter.z);
	// printf("%f, %f, %f %f\n", sp->c_s.x, sp->c_s.y, sp->c_s.z, sp->c_s.w);
	float a = dot(tmp, tmp);
	float b = - 2 * dot(ray->dir, rayToCenter);
	float c = dot(rayToCenter, rayToCenter) - sp->r2;

	float disc = b * b - 4 * c * a; 
	// printf ("disc = %f\n", disc);

	if (disc < 0.0f)
		return false;
	else{
		info->t = (-b - sqrt(disc)) / (2 * a);
	}
	if (info->t < 0.0f)
	{
		info->t = (-b + sqrt(disc)) / (2 * a);
		if (info->t < 0.0f)
			return false; 
	}
	return true;
}

bool	sphere_intersection1(__constant t_obj *sp, t_ray *ray, t_hit_info *info)
{
	float4 rayToCenter = sp->c_s - ray->origin;
	float4 tmp = normalize(rayToCenter);
	// printf("%f, %f, %f\n", rayToCenter.x, rayToCenter.y, rayToCenter.z);
	// printf("%f, %f, %f %f\n", sp->c_s.x, sp->c_s.y, sp->c_s.z, sp->c_s.w);
	float a = dot(tmp, tmp);
	float b = - 2 * dot(ray->dir, rayToCenter);
	float c = dot(rayToCenter, rayToCenter) - sp->r2;

	float disc = b * b - 4 * c * a; 
	// printf ("disc = %f\n", disc);

	if (disc < 0.0f)
		return false;
	else{
		info->t = (-b - sqrt(disc)) / (2 * a);
	}
	if (info->t < 0.0f)
	{
		info->t = (-b + sqrt(disc)) / (2 * a);
		if (info->t < 0.0f)
			return false; 
	}
	return true;
}


bool	plane_intersection(__constant t_obj *pl, t_ray *ray, t_hit_info *info)
{
	float4	tmp;
	float2	coefs;
	float	denom;

	tmp = -pl->c_s - ray->origin;
	denom = dot(pl->dir, ray->dir);
	coefs.x = -dot(pl->dir, tmp);
	coefs.y = dot(pl->dir, ray->dir);
	if (denom > 1e-6f)
	{
		tmp = pl->c_s - ray->origin;
		info->t = dot(tmp, pl->dir) / denom;
		return (info->t >= 0.0f);
		if ((info->t = coefs.x / coefs.y) >= 0.0f)
			return (true);
		if (coefs.y < 0.0f)
		{
			info->t = -info->t;
			return (true);
		}
		return (false);
	}
	return (false);
}

bool	cone_intersection(__constant t_obj *cn, t_ray *ray, t_hit_info *info)
{
	float4	tmp;
	float4	coefs;

	tmp = -cn->c_s - ray->origin;
	coefs.x = dot(ray->dir, ray->dir) - cn->r2 * pow(dot(ray->dir, cn->dir), 2);
	coefs.y = dot(ray->dir, tmp) - cn->r2 * dot(ray->dir, cn->dir) * dot(tmp, cn->dir);
	coefs.y *= 2.0f;
	coefs.z = dot(tmp, tmp) - cn->r2 * (pow(dot(tmp, cn->dir), 2));
	coefs.w = pow(coefs.y, 2) - 4.0f * coefs.x * coefs.z;
	if (coefs.w >= 0.0f)
	{
		info->t = (-coefs.y - sqrt(coefs.w)) * 0.5f / coefs.x;
		if (info->t <= 0.0f)
			info->t = (-coefs.y - sqrt(coefs.w)) * 0.5f / coefs.x;
		return (info->t >= 0.0f);
	}
	return (false);
}

bool	cylinder_intersection(__constant t_obj *cl, t_ray *ray, t_hit_info *info)
{
	float4		tmp;
	float4		coefs;

	tmp = -cl->c_s - ray->origin;
	coefs.x = dot(ray->dir, ray->dir) - pow(dot(ray->dir, cl->dir), 2);
	coefs.y = dot(ray->dir, tmp) - dot(ray->dir, cl->dir) * dot(tmp, cl->dir);
	coefs.y *= 2.0f;
	coefs.z = dot(tmp, tmp) - pow(dot(tmp, cl->dir), 2) - cl->r * cl->r;
	coefs.w = pow(coefs.y, 2) - 4.0f * coefs.x * coefs.z;
	// printf("eee %f\n", coefs.w);
	if (coefs.w >= 0.0f)
	{
		coefs.x *= 2.0f;
		coefs.w = sqrt(coefs.w);
		info->t = (-coefs.y - coefs.w) / coefs.x;
		if (info->t >= 0.0f)
			return (true);
	}
	return (false);
}

bool	cylinder_intersection1(__constant t_obj *cl, t_ray *ray, t_hit_info *info)
{
	float4		tmp;
	float4		coefs;

	tmp = -cl->c_s - ray->origin;
	coefs.x = dot(ray->dir, ray->dir) - pow(dot(ray->dir, cl->dir), 2);
	coefs.y = dot(ray->dir, tmp) - dot(ray->dir, cl->dir) * dot(tmp, cl->dir);
	coefs.y *= 2.0f;
	coefs.z = dot(tmp, tmp) - pow(dot(tmp, cl->dir), 2) - cl->r * cl->r;
	coefs.w = pow(coefs.y, 2) - 4.0f * coefs.x * coefs.z;
	if (coefs.w >= 0.0f)
	{
		coefs.x *= 2.0f;
		coefs.w = sqrt(coefs.w);
		info->t = (-coefs.y - coefs.w) / coefs.x;
		if (info->t >= 0.0f)
			return (true);
	}
	return (false);
}

bool		is_intersect1(__constant t_obj *obj, t_ray *ray, t_hit_info *info)
{
	bool	is_hit;

	is_hit = false;
	if (obj->type == sphere)
		is_hit = sphere_intersection1(obj, ray, info);
	else if (obj->type == plane)
		is_hit = plane_intersection(obj, ray, info);
	else if (obj->type == cone)
		is_hit = cone_intersection(obj, ray, info);
	else if (obj->type == cylinder)
		is_hit = cylinder_intersection1(obj, ray, info);
	return (is_hit);
}

bool		is_intersect(__constant t_obj *obj, t_ray *ray, t_hit_info *info)
{
	bool	is_hit;

	is_hit = false;
	if (obj->type == sphere)
		is_hit = sphere_intersection(obj, ray, info);
	else if (obj->type == plane)
		is_hit = plane_intersection(obj, ray, info);
	else if (obj->type == cone)
		is_hit = cone_intersection(obj, ray, info);
	else if (obj->type == cylinder)
		is_hit = cylinder_intersection(obj, ray, info);
	return (is_hit);
}

void		get_sphere_normal(__constant t_obj *obj, t_hit_info *info)
{
	float3 tmp;
	
	info->surf_normal = info->hit_point - obj->c_s;
	tmp = (float3)(info->surf_normal.x, info->surf_normal.y, info->surf_normal.z);
	tmp = normalize(tmp);
	info->surf_normal.x = tmp.x;
	info->surf_normal.y = tmp.y;
	info->surf_normal.z = tmp.z;
}

void		get_plane_normal(__constant t_obj *obj, t_hit_info *info)
{
	info->surf_normal = -obj->dir;
}

void		get_cone_normal(__constant t_obj *obj, t_hit_info *info)
{
	float	m;
	float3 tmp;

	info->surf_normal = info->hit_point - obj->c_s;
	m = dot(info->surf_normal, obj->dir);
	info->surf_normal = obj->dir * m * obj->r2;
	info->surf_normal = info->hit_point - info->surf_normal;
	info->surf_normal = info->surf_normal - obj->c_s;
	tmp = (float3)(info->surf_normal.x, info->surf_normal.y, info->surf_normal.z);
	tmp = normalize(tmp);
	info->surf_normal.x = tmp.x;
	info->surf_normal.y = tmp.y;
	info->surf_normal.z = tmp.z;
}

void		get_cylinder_normal(__constant t_obj *obj, t_hit_info *info)
{
	float	m;
	float3 tmp;

	info->surf_normal = info->hit_point - obj->c_s;
	m = dot(info->surf_normal, obj->dir);
	info->surf_normal = obj->dir * m;
	info->surf_normal = info->hit_point - info->surf_normal;
	info->surf_normal = info->surf_normal - obj->c_s;
	tmp = (float3)(info->surf_normal.x, info->surf_normal.y, info->surf_normal.z);
	tmp = normalize(tmp);
	info->surf_normal.x = tmp.x;
	info->surf_normal.y = tmp.y;
	info->surf_normal.z = tmp.z;
}

void		get_surface_normal(__constant t_obj *obj, t_hit_info *info)
{
	if (obj->type == sphere)
		get_sphere_normal(obj, info);
	else if (obj->type == plane)
		get_plane_normal(obj, info);
	else if (obj->type == cone)
		get_cone_normal(obj, info);
	else if (obj->type == cylinder)
		get_cylinder_normal(obj, info);
}

__constant t_obj	*get_intersection(__constant t_obj *objs, int nobjs, t_ray *ray, t_hit_info *info)
{
	int		i = -1;
	int		hit_id = -1;
	float	hit_distance = MAXFLOAT;
	info->t = 0.0f;
	
	while (++i < nobjs)
	{
		if (is_intersect(objs + i, ray, info))
		{
			// printf("vtoroi %f %f\n", info->t, hit_distance);
			
			if (info->t < hit_distance)
			{
				// if (hit_id != -1)
				// 	info->t = INFINITY;
				hit_distance = info->t;
				// printf ("%f\n", hit_distance);
				hit_id = i;
			}
			else
			{
				info->t = hit_distance;
			}
		}
	}
	if (hit_id != -1)
	{
		get_hit_point(objs + hit_id, ray, info);
		get_surface_normal(objs + hit_id, info);
		info->t = INFINITY;
	}
	// printf("%d\n", hit_id);
	return ((hit_id != -1 && hit_distance < 1000.0f) ? objs + hit_id : 0);
}

__constant t_obj	*get_intersection1(__constant t_obj *objs, int nobjs, t_ray *ray, t_hit_info *info)
{
	int		i = -1;
	int		hit_id = -1;
	float	hit_distance = MAXFLOAT;
	info->t = 0.0f;
	
	while (++i < nobjs)
	{
		if (is_intersect1(objs + i, ray, info))
		{
			// printfl(ray->dir);
			if (info->t < hit_distance)
			{
				// if (hit_id != -1)
				// 	info->t = INFINITY;
				// printf("hit %f\n", info->t);
				hit_distance = info->t;
				// printf ("%f\n", hit_distance);
				hit_id = i;
			}
		}
		else
		{
			// printf("not %f %f\n", info->t, hit_distance);
		}
	}
	info->t = hit_distance;
	// printf("res %f\n", info->t);
	if (hit_id != -1)
	{
		get_hit_point(objs + hit_id, ray, info);
		get_surface_normal(objs + hit_id, info);
		info->t = INFINITY;
	}
	// printf("%u\n", (objs + hit_id)->type);
	return ((hit_id != -1 && hit_distance < 1000.0f) ? objs + hit_id : 0);
}



t_ray		cast_shadow_ray(float4 start, __constant t_light_source *light)
{
	t_ray	shadow_ray;
	float3	tmp;
	shadow_ray.origin = start;
	shadow_ray.dir = light->c_s - start;
	shadow_ray.dir.w = 0.0f;
	shadow_ray.origin.w = 0.0f;
	tmp = (float3)(shadow_ray.dir.x, shadow_ray.dir.y, shadow_ray.dir.z);
	tmp = normalize(tmp);
	shadow_ray.dir.x = tmp.x;
	shadow_ray.dir.y = tmp.y;
	shadow_ray.dir.z = tmp.z;
	return (shadow_ray);
}

int4	calc_color(__constant t_obj *obj, float diff_light, float spec_light)
{
	int4	color;

	color.x = clamp(obj->mat.color.x * (diff_light * obj->mat.kd + spec_light * obj->mat.ks), 0.0f, 255.0f);
	color.y = clamp(obj->mat.color.y * (diff_light * obj->mat.kd + spec_light * obj->mat.ks), 0.0f, 255.0f);
	color.z = clamp(obj->mat.color.z * (diff_light * obj->mat.kd + spec_light * obj->mat.ks), 0.0f, 255.0f);
	color.w = clamp(obj->mat.color.w * (diff_light * obj->mat.kd + spec_light * obj->mat.ks), 0.0f, 255.0f);
	return (color);
}

float4		reflect(float4 i, float4 n)
{
	float	k;
	float4	tmp;

	n.w = 0;
	i.w = 0;
	k = dot(i, n);
	tmp = n * 2 * k;
	return (i - tmp);
}

int4	get_point_color(__constant t_obj *objs, const int nobjs,
				__constant t_light_source *lights, const int nlights,
				__constant t_obj *obj, t_ray *ray, int i, t_hit_info *info)
{
	float3	ds_light = 0.0f;
	__constant t_obj	*shadow_obj;
	t_hit_info	sinfo;
	float4	r;
	float4	light_dir;
	t_ray	shadow_ray;
	float3	tmp;

	while (++i < nlights)
	{
		shadow_ray = cast_shadow_ray(info->hit_point, lights + i);
		// printfl(shadow_ray.dir);
		if ((shadow_obj = get_intersection1(objs, nobjs, &shadow_ray, &sinfo)) != 0)
		{
			// printfl(info->hit_point);
			// printf("%f %f %f %f\n", sinfo.hit_point.x, sinfo.hit_point.y, sinfo.hit_point.z, sinfo.hit_point.w);
			if (shadow_obj != obj && distance(info->hit_point, (lights + i)->c_s) >
				distance(info->hit_point, sinfo.hit_point))
				continue;
		}
		light_dir = (lights + i)->c_s - info->hit_point;
		// printf("%f, %f, %f, %f\n", (lights + i)->c_s.x, (lights + i)->c_s.y, (lights + i)->c_s.z, (lights + i)->c_s.w);
		// printf("%f %f %f\n", info->hit_point.x, info->hit_point.y, info->hit_point.z, info->hit_point.w);
		tmp = (float3)(light_dir.x, light_dir.y, light_dir.z);
		tmp = normalize(tmp);
		light_dir.x = tmp.x;
		light_dir.y = tmp.y;
		light_dir.z = tmp.z;
		light_dir.w = 0.0f;
		info->surf_normal.w = 0.0f;
		ds_light.x += (lights + i)->intensity * max(0.0f, dot(light_dir, info->surf_normal));
		r = reflect(light_dir, info->surf_normal);
		ds_light.y += (lights + i)->intensity * pow(max(0.0f, dot(r, ray->dir)), obj->mat.n);
	}
	// printf ("%f %f\n", ds_light.x, ds_light.y);
	return (calc_color(obj, ds_light.x, ds_light.y));
}

int4	trace_ray(t_ray *ray, __constant t_obj *objs, const int nobjs,
				__constant t_light_source *lights, const int nlights)
{
	int4	color = (int4)(50, 50, 50, 0);
	int		i = -1;
	__constant t_obj	*obj;
	t_hit_info	info;


	obj = get_intersection(objs, nobjs, ray, &info);
	if (obj != 0)
	{
		return (color = get_point_color(objs,
		nobjs, lights, nlights, obj, ray, i, &info));
	}

	return color;
}

t_world	new_world(t_obj objs, const int nobjs, t_light_source lights, const int nlights)
{
	t_world	world;

	world.objs = &objs;
	world.lights = &lights;
	world.nobjs = nobjs;
	world.nlights = nlights;
	return world;
}

__kernel void	render(__global int *C, __constant t_obj *jojo, const int nobjs,
				__constant t_light_source *lights, const int nlights, const t_camera cam)
{
	int		i = get_global_id(0);
	int		x = i % SCREEN_WIDTH;
	int		y = i / SCREEN_WIDTH;
	int4	color = 20;
	t_ray	ray = create_cam_ray(x, y, cam);
	// if (x == 1500 && y == 510){
		color = trace_ray(&ray, jojo, nobjs, lights, nlights);
		// printf("%d", jojo[0].type);
		// printf("%f %f %f\n", jojo[0].c_s.x, jojo[0].c_s.y, jojo[0].c_s.z);
		// printf("%f %f %f\n", jojo[1].c_s.x, jojo[1].c_s.y, jojo[1].c_s.z);
	// }
	// int fx = (float)(x) / (float)SCREEN_WIDTH * 255;
	// int fy = (float)(y) / (float)SCREEN_HEIGHT * 255;
	// color = (int3)(fx << 16, fy << 8, 180);
	
	C[i] = color.x << 16| color.y << 8| color.z;
	// if (i ==0)
}
