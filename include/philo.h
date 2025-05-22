/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 16:35:52 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/22 14:51:42 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

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

typedef struct s_fork
{
	int				is_taken;
	pthread_mutex_t	mutex;
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
	int				end;
	pthread_mutex_t	death_mutex;
	long			start_time;
}					t_shared;

typedef struct s_philo
{
	pthread_t		monitor;
	pthread_mutex_t	*print_mutex;
	int				print_status;
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
	int				*ids;
}					t_data;

/* main.c */
int					main(int argc, char **argv);
int					start_philo(t_data *data, t_monitor_data *monitor_data);

/* init_and_clean.c */
int					init_rules(t_rules *rules, int argc, char **argv);
int					init_philosophers(t_data *data);
void				init_philo_data(t_data *data);
int					join_and_cleanup(t_data *data, int num,
						t_monitor_data *monitor_data);

/* init_mutexes.c */
int					init_mutexes(t_data *data);
int					init_basic_mutexes(t_data *data);
int					init_fork_mutexes(t_data *data);
void				destroy_mutexes(t_data *data);
void				destroy_forks(t_data *data, int num);

/* parsing_utils.c */
int					check_creation(t_data *data);
long				ft_atoi(const char *nptr);
int					check_ascii(char *argj);

/* routine.c */
void				*routine(void *args);
int					should_philosopher_stop(t_philo *philo);
void				init_forks(t_philo *philo, t_fork **first, t_fork **second);

/* utils_routine.c */
long long			get_time_in_ms(void);
long				timestamp(t_shared *shared);
void				my_usleep(long duration, t_philo *philo);
int					check_meals_completed(t_philo *philo);
int					check_death_status(t_shared *shared);

/* rules.c */
int					eat(t_philo *philo, t_fork *first, t_fork *second,
						int *num_of_forks);
void				thinking(t_philo *philo);
int					p_sleep(t_philo *philo);
int					take_fork(t_fork *first, t_fork *second, t_philo *philo,
						int *num_of_forks);

/* utils_rules.c */
void				release_fork(t_fork *fork);
void				safe_release_both(t_fork *first, t_fork *second,
						int *num_of_forks);
int					print_fork_status(t_philo *philo);
int					update_meal_time(t_philo *philo);
int					print_eating(t_philo *philo);

/* monitor.c */
int					report_death(t_monitor_data *data, int i);
int					check_philosopher_status(t_monitor_data *data, int i);
int					check_all_philosophers(t_monitor_data *data);
void				*monitor_routine(void *args);
int					check_if_dead(t_philo *philo);
#endif