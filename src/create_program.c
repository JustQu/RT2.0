#include "rt.h"
#define BUFF_SIZE 32

/**
** @TODO(dmelessa): rewrite this one
** @brief Get the source buf object
**
** @param file_name
** @return ** char*
*/
char		*get_source_buf(char *file_name)
{
	int		fd;
	char	*source;
	char	*tmp;
	int		ret;
	char	buff[BUFF_SIZE + 1];

	fd = open(file_name, O_RDONLY);
	if (fd < 0)
		return (NULL);
	source = ft_strdup("");
	ret = 0;
	while ((ret = read(fd, buff, BUFF_SIZE)) > 0)
	{
		buff[ret] = '\0';
		tmp = source;
		source = ft_strjoin(source, buff);
		ft_strdel(&tmp);
	}
	if (ret < 0)
	{
		ft_strdel(&source);
		return (NULL);
	}
	close(fd);
	return (source);
}

static const char const *files[] = {
	"cl_rt.h",
	"utils.cl",
	"solver.cl",
	"random.cl",
	"color.cl",
	"sampler.cl",
	"camera.cl",
	"intersection.cl",
	"normal.cl",
	"light.cl",
	"ray_tracer.cl",
	"path_tracer.cl",
	"main_kernel.cl",
	NULL};
static const int num_files = sizeof(files) / sizeof(char *);

void create_source(char **source_buf, int num)
{
	char *dir;
	int i;

	i = 0;
	dir = DEFAULT_KERNEL_DIR;
	while (files[i])
	{
		printf("!%s:\n", files[i]);
		source_buf[i] = get_source_buf(ft_strjoin(dir, files[i]));
		i++;
	}
}

cl_program create_program(t_cl_program p, cl_context context)
{
	cl_program program;
	char **source_buf;
	int ret;
	// int NUM_FILES;

	// NUM_FILES = 5;
	source_buf = (char **)malloc(sizeof(char *) * num_files);
	create_source(source_buf, num_files);
	program = clCreateProgramWithSource(context, num_files - 1,
										(const char **)source_buf, NULL, &ret);
	printf("%d\n", ret);
	p.program = program;
	cl_error(&p, &p.clp, ret);
	assert(!ret);
	// ft_clerror(ret);
	free(source_buf);
	return (program);
}
