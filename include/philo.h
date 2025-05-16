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
	long			start_time;
}					t_shared;

typedef struct s_philo
{
	pthread_t		monitor;
	pthread_mutex_t	*print_mutex;
	pthread_mutex_t	*slow_mutex;
	int				meals_eaten;
	int				id;
	long			last_meal_time;
	pthread_mutex_t	*meal_mutex;
	t_rules			*rules;
	t_shared		*shared;
	t_fork			*right_fork;
	int				right_fork_id;
	t_fork			*left_fork;
	int				left_fork_id;

}					t_philo;

typedef struct s_monitor_data
{
	t_philo			*philos;
	int				time_to_die;
	int				num_of_philo;
	t_shared		*shared;
}					t_monitor_data;

typedef struct s_data
{
	t_rules			rules;
	t_shared		shared;
	t_philo			*philos;
	t_fork			*forks;
	pthread_t		*threads;
	pthread_t		monitor;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	meal_mutex;
	pthread_mutex_t	slow_mutex;
	int				*ids;
}					t_data;

// main
int					main(int argc, char **argv);
void				*routine(void *args);
int					create_threads(t_data *data, t_monitor_data *monitor_data);

// init_and_clean
int					init_rules(t_rules *rules, int argc, char **argv);
int					init_mutexes(t_data *data);
void				destroy_forks(t_data *data, int num);
void				destroy_mutexes(t_data *data);
int					init_philosophers(t_data *data);
int					join_and_cleanup(t_data *data, t_monitor_data *monitor_data,
						int num);
// routine
void				*monitor_routine(void *args);
int					check_all_philosophers(t_monitor_data *data);
int					check_philosopher_status(t_monitor_data *data, int i);
int					report_death(t_monitor_data *data, int i);
int					should_philosopher_stop(t_philo *philo);

// routine_utils
int					check_if_dead(t_philo *philo);
int					take_fork(t_fork *fork);
int					check_death_status(t_shared *shared);
void				release_fork(t_fork *fork);
void				init_forks(t_philo *philo, t_fork **first, t_fork **second);
int					can_print(t_philo *philo);
// rules
int					p_sleep(t_philo *philo);
int					eat(t_philo *philo, t_fork *first, t_fork *second);
int					handle_one_philo(t_philo *philo);

// utils_rules
int					print_fork_status(t_philo *philo);
int					check_meals_completed(t_philo *philo);
int					update_meal_time(t_philo *philo);
int					print_eating(t_philo *philo);
void				safe_release_both(t_fork *first, t_fork *second);

// utils
int					check_ascii(char *argj);
long				ft_atoi(const char *nptr);
long				timestamp(t_shared *shared);
long long			get_time_in_ms(void);
void				my_usleep(long duration, t_philo *philo);
#endif