#ifndef PHILO_H
# define PHILO_H

# include <fcntl.h>
# include <limits.h>
# include <pthread.h>
# include <stddef.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

# define LONG_LONG_INT 9223372036854775807
# define RESET "\033[0m"
# define BLUE "\033[34m"
# define RED "\033[1;31m"
# define GREEN "\033[38;5;28m"
# define YELLOW "\033[33m"
# define HOT_PINK "\033[38;5;198m"
# define LIGHT_PINK "\033[38;5;218m"

typedef struct s_fork
{
	int is_taken;          // 1 = someone took the fork, 0 = free
	pthread_mutex_t mutex; // protects the `is_taken`
}					t_fork;

typedef struct s_rules
{
	int				num_of_philo;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				num_must_eat;
}					t_rules;

typedef struct s_shared
{
	int				someone_died;
	pthread_mutex_t	death_mutex;
	pthread_mutex_t	urgency_mutex;
	int				all_ate_enough;
	long			start_time;
}					t_shared;
typedef struct s_philo
{
	pthread_t		monitor;
	pthread_mutex_t	*print_mutex;
	int				meals_eaten;
	int				id;
	long			last_meal_time;
	int				urgency;
	int				time_left;
	int				wait_time;
	pthread_mutex_t	*meal_mutex;
	t_rules			*rules;
	t_shared		*shared;
	t_fork			*right_fork;
	t_fork			*left_fork;

}					t_philo;

typedef struct s_monitor_data
{
	t_philo			*philos;
	int				time_to_die;
	int				num_of_philo;
	t_shared		*shared;
}					t_monitor_data;

// main
int					main(int argc, char **argv);
int					init_rules(t_rules *rules, int argc, char **argv);

// routine
int					check_if_dead(t_philo *philo);
int					take_fork(t_fork *fork, t_philo *philo);
void				release_fork(t_fork *fork, t_philo *philo);
void				*routine(void *args);
void				*monitor_routine(void *args);

// utils
long				ft_atoi(const char *nptr);
long long			get_time_in_ms(void);
long				timestamp(t_shared *shared);
int					check_ascii(char *argj);
void				my_usleep(long duration);
#endif