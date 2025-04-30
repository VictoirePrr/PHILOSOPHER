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
# define LONG_LONG_INT 9223372036854775807
typedef struct s_fork
{
	int is_taken;          // 1 = someone took the fork, 0 = free
	pthread_mutex_t mutex; // protects the `is_taken`
}					t_fork;

typedef struct s_philo
{
	int				id;
	pthread_mutex_t	*print_mutex;
	int				times_eaten;

	long long		last_meal_time;
	pthread_mutex_t meal_mutex; // protects last_meal_time

	t_fork			*left_fork;
	t_fork			*right_fork;

	struct s_data *data; // shared global data
}					t_philo;
typedef struct s_data
{
	int				nb_philo;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				num_of_times_each_philo_must_eat;

	long long		start_time;
	int				stop_simulation;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	print_mutex;
	t_philo			*philos;
	t_fork			*forks;
}					t_data;

// main
void				*routine(void *arg);
t_data				*init_data(t_data *data, int argc, char **argv);

// parsing
int					check_ascii(char *argj);
int					parse_args(char **argv, t_philo *philo);

// routine
int					handle_routine(t_data *data);
int					init_forks(t_fork *forks, int num);
int					start_routine(t_data *data, pthread_t *thread);
int					init_philosophers(t_philo *philo, t_fork *forks,
						t_data *data);
void				destroy_mutex(t_philo *philo, pthread_mutex_t *print_mutex,
						t_fork *forks);

// utils
int					take_fork(t_fork *fork);
long				ft_atoi(const char *nptr);
char				*ft_strtrim(const char *s1, const char *set);
void				release_fork(t_fork *fork);
void				print_action(t_philo *philo, char *str);
char				*ft_substr(char const *s, int start, int len);
char				*ft_strchr(const char *s, int c);
char				*ft_strdup(const char *s1);
#endif