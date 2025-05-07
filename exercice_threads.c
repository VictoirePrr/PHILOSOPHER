#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#define LONG_LONG_INT 9223372036854775807

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

long	ft_atoi(const char *nptr)
{
	size_t			i;
	int				sign;
	unsigned long	nb;

	i = 0;
	sign = 1;
	nb = 0;
	while ((nptr[i] >= 9 && nptr[i] <= 13) || nptr[i] == 32)
		i++;
	if (nptr[i] == '-' || nptr[i] == '+')
		if (nptr[i++] == '-')
			sign *= -1;
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		if (((nb * 10) + (nptr[i] - '0')) > LONG_LONG_INT && sign == 1)
			return (-1);
		else if (((nb * 10) + (nptr[i] - '0')) > LONG_LONG_INT && sign == -1)
			return (0);
		nb = nb * 10 + (nptr[i++] - '0');
	}
	return (nb * sign);
}

long long	get_time_in_ms(void)
{
	struct timeval	current_time;

	gettimeofday(&current_time, NULL);
	return (current_time.tv_sec * 1000 + current_time.tv_usec / 1000);
}

long	timestamp(t_shared *shared)
{
	return (get_time_in_ms() - shared->start_time);
}
int	check_ascii(char *argj)
{
	int	j;

	j = 0;
	while (argj[j])
	{
		if (!(argj[j] >= '0' && argj[j] <= '9'))
		{
			printf("ERROR : use numbers\n");
			return (1);
		}
		j++;
	}
	return (0);
}

int	check_if_dead(t_philo *philo)
{
	pthread_mutex_lock(&philo->shared->death_mutex);
	if (philo->shared->someone_died)
	{
		pthread_mutex_unlock(&philo->shared->death_mutex);
		return (1);
	}
	pthread_mutex_unlock(&philo->shared->death_mutex);
	return (0);
}

int	take_fork(t_fork *fork)
{
	pthread_mutex_lock(&fork->mutex);
	if (fork->is_taken == 0)
	{
		fork->is_taken = 1;
		return (fork->is_taken);
	}
	else
		pthread_mutex_unlock(&fork->mutex);
	return (0);
}

void	release_fork(t_fork *fork)
{
	pthread_mutex_lock(&fork->mutex);
	fork->is_taken = 0;
	pthread_mutex_unlock(&fork->mutex);
}
void	*routine(void *args)
{
	t_philo	*philo;
	t_fork	*first;
	t_fork	*second;

	philo = (t_philo *)args;
	while (check_if_dead(philo) != 1)
	{
		// 1. Think
		pthread_mutex_lock(philo->print_mutex);
		printf("%ld Philosopher %d is thinking.\n", timestamp(philo->shared),
			philo->id);
		pthread_mutex_unlock(philo->print_mutex);
		my_usleep(100);
		// 2. Define which forks
		if (philo->id % 2 == 0)
		{
			first = philo->left_fork;
			second = philo->right_fork;
		}
		else
		{
			first = philo->right_fork;
			second = philo->left_fork;
		}
		// 3. Eat
		if ((take_fork(first) == 1) && (take_fork(second) == 1))
		{
			if (check_if_dead(philo) == 1) // Dead before eating
			{
				release_fork(first);
				release_fork(second);
				break ;
			}
			pthread_mutex_lock(philo->print_mutex);
			printf("%ld Philosopher %d is eating\n", timestamp(philo->shared),
				philo->id);
			philo->meals_eaten++;
			pthread_mutex_unlock(philo->print_mutex);
			pthread_mutex_lock(philo->meal_mutex);
			philo->last_meal_time = get_time_in_ms();
			pthread_mutex_unlock(philo->meal_mutex);
			my_usleep(philo->rules->time_to_eat * 1000);
			// 4. Drop forks
			release_fork(first);
			release_fork(second);
		}
		// 5. Sleep
		if (check_if_dead(philo) == 1)
			break ;
		pthread_mutex_lock(philo->print_mutex);
		printf("%ld Philosopher %d is sleeping\n", timestamp(philo->shared),
			philo->id);
		pthread_mutex_unlock(philo->print_mutex);
		my_usleep(philo->rules->time_to_sleep * 1000);
	}
	return (NULL);
}

void	*monitor_routine(void *args)
{
	t_monitor_data	*monitor_data;
	int				i;
	int				finished;
	long			time_since_meal;

	monitor_data = (t_monitor_data *)args;
	while (1)
	{
		i = 0;
		finished = 0;
		pthread_mutex_lock(&monitor_data->shared->death_mutex);
		if (monitor_data->shared->someone_died == 1)
		{
			pthread_mutex_unlock(&monitor_data->shared->death_mutex);
			return (NULL); // Stop monitoring, someone already died
		}
		pthread_mutex_unlock(&monitor_data->shared->death_mutex);
		while (i < monitor_data->num_of_philo)
		{
			pthread_mutex_lock(monitor_data->philos[i].meal_mutex);
			time_since_meal = get_time_in_ms()
				- monitor_data->philos[i].last_meal_time;
			pthread_mutex_unlock(monitor_data->philos[i].meal_mutex);
			if (time_since_meal > monitor_data->time_to_die)
			{
				pthread_mutex_lock(monitor_data->philos[i].print_mutex);
				printf("%lld Philosopher %d died.\n", get_time_in_ms()
					- monitor_data->shared->start_time,
					monitor_data->philos[i].id);
				pthread_mutex_unlock(monitor_data->philos[i].print_mutex);
				pthread_mutex_lock(&monitor_data->shared->death_mutex);
				monitor_data->shared->someone_died = 1;
				pthread_mutex_unlock(&monitor_data->shared->death_mutex);
				return (NULL);
			}
			if (monitor_data->philos[i].meals_eaten >= monitor_data->philos[i].rules->num_must_eat)
				finished++;
			i++;
		}
		if (finished == monitor_data->num_of_philo)
		{
			pthread_mutex_lock(&monitor_data->shared->death_mutex);
			monitor_data->shared->someone_died = 1;
			pthread_mutex_unlock(&monitor_data->shared->death_mutex);
			return (NULL);
		}
		my_usleep(1000);
	}
}

int	init_rules(t_rules *rules, int argc, char **argv)
{
	int	i;

	i = 1;
	if (argc != 5 && argc != 6)
	{
		printf("ERROR : invalid arguments\n");
		return (1);
	}
	while (argv[i])
	{
		if (check_ascii(argv[i]) == 0)
			i++;
		else
			return (1);
	}
	rules->num_of_philo = ft_atoi(argv[1]);
	rules->time_to_die = ft_atoi(argv[2]);
	rules->time_to_eat = ft_atoi(argv[3]);
	rules->time_to_sleep = ft_atoi(argv[4]);
	if (argc == 6)
		rules->num_must_eat = ft_atoi(argv[5]);
	else
		rules->num_must_eat = -1;
	return (0);
}
int	main(int argc, char **argv)
{
	t_philo *philo;
	t_rules rules;
	t_shared shared;
	t_fork *fork;
	t_monitor_data monitor_data;
	pthread_t monitor;
	pthread_t *threads;
	pthread_mutex_t print_mutex;
	pthread_mutex_t meal_mutex;
	int i;
	int ids[5];

	i = 0;
	////////////////////parsing///////////////
	if (init_rules(&rules, argc, argv) == 1)
	{
		printf("ERROR: initialization failed\n");
		return (1);
	}
	///////////init mutexes//////////////////
	pthread_mutex_init(&print_mutex, NULL);
	pthread_mutex_init(&meal_mutex, NULL);
	///////shared init////////
	pthread_mutex_init(&shared.death_mutex, NULL);
	shared.someone_died = 0;
	shared.all_ate_enough = 0;
	/////////init philo struct/////////////////
	philo = malloc(rules.num_of_philo * sizeof(t_philo));
	///////////init threads philos////////////////
	threads = malloc(rules.num_of_philo * sizeof(pthread_t));
	/////////////create forks//////////////////
	fork = malloc(rules.num_of_philo * sizeof(t_fork));
	fork->is_taken = 0;
	while (i < rules.num_of_philo)
	{
		pthread_mutex_init(&fork[i].mutex, NULL);
		i++;
	}
	shared.start_time = get_time_in_ms();
	////////////assign values to each philos/////////////////////
	i = 0;
	while (i < rules.num_of_philo)
	{
		ids[i] = i + 1;
		philo[i].id = ids[i];
		philo[i].meals_eaten = 0;
		philo[i].print_mutex = &print_mutex;
		philo[i].left_fork = &fork[i];
		philo[i].right_fork = &fork[(i + 1) % rules.num_of_philo];
		philo[i].rules = &rules;
		philo[i].last_meal_time = get_time_in_ms();
		philo[i].meal_mutex = &meal_mutex;
		philo[i].shared = &shared;
		i++;
	}
	/////////////start routine///////////////////////
	i = 0;
	while (i < rules.num_of_philo)
	{
		pthread_create(&threads[i], NULL, routine, &philo[i]);
		i++;
	}
	/////////create monitor//////////////////////
	monitor_data.num_of_philo = ft_atoi(argv[1]);
	monitor_data.time_to_die = ft_atoi(argv[2]);
	monitor_data.shared = &shared;
	monitor_data.philos = philo;
	pthread_create(&monitor, NULL, monitor_routine, &monitor_data);
	////////join threads/////////////////////////
	i = 0;
	while (i < rules.num_of_philo)
	{
		pthread_join(threads[i], NULL);
		// printf("Philosopher %d died.\n", i + 1);
		i++;
	}
	pthread_mutex_destroy(&print_mutex);
	return (0);
}