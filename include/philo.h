#ifndef PHILO_H
# define PHILO_H

# include <fcntl.h>
# include <limits.h>
# include <pthread.h>
# include <stddef.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

# define ERROR -1
# define SUCCESS 0

typedef struct s_philo
{
	int				id;
	pthread_mutex_t	*print_mutex;
}					t_philo;


#endif