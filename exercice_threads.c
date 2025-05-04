#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct s_rules {
    int num_of_philo;
    int time_to_die;
    int time_to_eat;
    int time_to_sleep;
    int num_must_eat;
} t_rules;

typedef struct s_shared
{
    int *someone_died;
    pthread_mutex_t *death_mutex;
} t_shared;

typedef struct s_monitor_data {
    t_philo *philos;
    int time_to_die;
     int num_of_philo;
     t_shared *shared;
} t_monitor_data;

typedef struct s_philo
{
    pthread_t monitor;
    pthread_mutex_t *print_mutex;
    pthread_mutex_t *right_fork;
    pthread_mutex_t *left_fork;
    int id;
    long last_meal_time;
    pthread_mutex_t *meal_mutex;
    t_rules *rules;
    t_shared *shared;

} t_philo;

long long	get_time_in_ms(void)
{
	struct timeval	current_time;

	gettimeofday(&current_time, NULL);
	return (current_time.tv_sec * 1000 + current_time.tv_usec / 1000);
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
			return (0);
		}
		j++;
	}
	return (1);
}
void *routine(void *args)
{
    t_philo *philo;

    philo = (t_philo *)args;
    pthread_mutex_lock(philo->print_mutex);
    printf("Philosopher %d is thinking\n", philo->id);
     pthread_mutex_unlock(philo->print_mutex);
     sleep(3);
    if (philo->id % 2 == 0)
    {
        pthread_mutex_lock(philo->left_fork);
        pthread_mutex_lock(philo->right_fork);
    }
    else
    {
        pthread_mutex_lock(philo->right_fork);
        pthread_mutex_lock(philo->left_fork);
    }
    printf("Philosopher %d is eating\n", philo->id);
    sleep(philo->rules->time_to_eat);
    pthread_mutex_lock(philo->meal_mutex);
    philo->last_meal_time = get_time_in_ms();
    pthread_mutex_unlock(philo->meal_mutex);
    if (philo->id % 2 == 0)
    {
        pthread_mutex_unlock(philo->left_fork);
        pthread_mutex_unlock(philo->right_fork);
    }
    else
    {
        pthread_mutex_unlock(philo->right_fork);
        pthread_mutex_unlock(philo->left_fork);
    }
    pthread_mutex_lock(philo->print_mutex);
    printf("Philosopher %d is sleeping\n", philo->id);
     pthread_mutex_unlock(philo->print_mutex);
     sleep(philo->rules->time_to_sleep);
    return NULL;
}
void *monitor_routine(void *args)
{
    t_monitor_data *monitor_data;
    int i;

    monitor_data = (t_monitor_data *)args;
    i = 0;
    while(i < monitor_data->num_of_philo)
    {
        if ( get_time_in_ms() - monitor_data->philos[i].last_meal_time > monitor_data->time_to_die)
        {
            printf("Philosopher %d died.\n", monitor_data->philos[i].id);
            pthread_mutex_lock(&monitor_data->shared->death_mutex);
            monitor_data->shared->someone_died = 1;
            pthread_mutex_unlock(&monitor_data->shared->death_mutex);
        }
        i++;
    }
}
int	init_rules(t_rules *rules, int argc, char **argv)
{
int		i;

	i = 1;
	while (argv[i])
	{
		if (check_ascii(argv[i]) == 0)
		{
			if (i == 1)
				rules->num_of_philo = ft_atoi(argv[i]);
			if (i == 2)
				rules->time_to_die = ft_atoi(argv[i]);
			if (i == 3)
				rules->time_to_eat = ft_atoi(argv[i]);
			if (i == 4)
				rules->time_to_sleep = ft_atoi(argv[i]);
			if (i == 5)
				rules->num_must_eat = ft_atoi(argv[i]);
		}
		else
			return (1);
		i++;
	}
	return (0);
}
int main(int argc, char **argv)
{
    t_philo *philo;
    t_rules *rules;
    t_shared shared;
    t_monitor_data monitor_data;
    pthread_t monitor;
    pthread_t *threads;
    pthread_mutex_t print_mutex;
    pthread_mutex_t *forks;
    pthread_mutex_t meal_mutex;
    pthread_mutex_t death_mutex;
    int i;
    int ids[5];

    i = 0;
    ////////////////////parsing///////////////
    if (argc != 5 && argc != 6)
	{
		printf("ERROR : invalid arguments");
		return (1);
	}
    if (init_rules(&rules, argc, argv) == 1)
	{
		printf("ERROR: initialization failed\n");
		return (1);
	}
    ///////////init mutexes//////////////////
    pthread_mutex_init(&print_mutex, NULL);
    pthread_mutex_init(&meal_mutex, NULL);
    pthread_mutex_init(&death_mutex, NULL);
    ///////shared init////////
    shared.someone_died = 0;
    shared.death_mutex = &death_mutex;
    /////////init philo struct/////////////////
     philo = malloc(rules->num_of_philo * sizeof(t_philo));
    ///////////init threads philos////////////////
    threads = malloc(rules->num_of_philo * sizeof(pthread_t));
    /////////////create forks//////////////////
    forks = malloc(rules->num_of_philo * sizeof(pthread_mutex_t));
    while (i < 5)
    {
        pthread_mutex_init(&forks[i], NULL);
        i++;
    }
    ////////////assign values to each philos/////////////////////
    i = 0;
    while (i < 5)
    {
        ids[i] = i + 1;
        philo[i].id = ids[i];
        philo[i].print_mutex = &print_mutex;
        philo[i].left_fork = &forks[i];
        philo[i].right_fork = &forks[(i + 1) % 5];
        philo[i].rules = &rules;
        philo[i].last_meal_time = 0;
        philo[i].meal_mutex = &meal_mutex;
        philo[i].shared = &shared;
        i++;
    }
    /////////////start routine///////////////////////
    i = 0;
    while (i < 5)
    {
        pthread_create(&threads[i], NULL, routine, &philo[i]);
        i++;
    }
    /////////create monitor//////////////////////
    monitor_data.num_of_philo = ft_atoi(argv[1]);
    monitor_data.time_to_die = ft_atoi(argv[2]);
    monitor_data.shared = &shared;
    monitor_data.philos = &philo;
    pthread_create(&monitor, NULL, monitor_routine, &monitor_data);
    ////////join threads/////////////////////////
    i = 0;
    while (i < 5)
    {
        pthread_join(threads[i], NULL);
        printf("Philosopher %d died.\n", i + 1);
        i++;
    }
    pthread_mutex_destroy(&print_mutex);
    return (0);
}