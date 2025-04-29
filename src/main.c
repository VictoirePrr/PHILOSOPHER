#include "philo.h"

void	*routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	int cycles = 10; //  time_to_die
	while (cycles > 0)
	{
		pthread_mutex_lock(philo->print_mutex);
		printf("Philo %d is thinking...\n", philo->id);
		usleep(500 * 1000); // is there a time to think ??
		pthread_mutex_unlock(philo->print_mutex);
		pthread_mutex_lock(philo->left_fork);
		pthread_mutex_lock(philo->right_fork);
		pthread_mutex_lock(philo->print_mutex);
		printf("Philo %d is eating 🍝\n", philo->id);
		usleep(1000 * 1000); // time_to_eat
		pthread_mutex_unlock(philo->print_mutex);
		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_lock(philo->print_mutex);
		printf("Philo %d is sleeping 💤\n", philo->id);
		usleep(700 * 1000); // time_to_sleep
		pthread_mutex_unlock(philo->print_mutex);
		cycles--;
	} // wtf is -> [number_of_times_each_philosopher_must_eat]
	return (NULL);
}

void	init_philosophers(t_philo *philo, pthread_mutex_t *print_mutex,
		pthread_mutex_t *forks, int num)
{
	int	i;

	i = 0;
	while (i < num)
	{
		philo[i].id = i + 1; // to start at 1.
		philo[i].print_mutex = print_mutex;
		philo[i].left_fork = &forks[i];
		philo[i].right_fork = &forks[(i + 1) % num]; // calcul a revoir
		i++;
	}
}

int	init_forks(pthread_mutex_t *forks, int num)
{
	int	i;

	i = 0;
	while (i < num)
	{
		if (pthread_mutex_init(&forks[i], NULL) != 0)
		{
			printf("Error initializing mutex\n");
			return (ERROR);
		}
		i++;
	}
	return (SUCCESS);
}

int	main(void)
{
	int number_of_philosophers;
	int i;

	number_of_philosophers = 5; // Number of philosophers
	pthread_t thread[number_of_philosophers];
	t_philo philo[number_of_philosophers]; // Array of philosophers
	pthread_mutex_t print_mutex;           // Shared mutex for printing
	pthread_mutex_t forks[number_of_philosophers];

	// Init forks
	if (init_forks(forks, number_of_philosophers) == ERROR)
		return (1);

	// Init the print_mutex
	pthread_mutex_init(&print_mutex, NULL);

	// Fill the structs with the shared print_mutex
	init_philosophers(philo, &print_mutex, forks, number_of_philosophers);

	// Create the threads
	i = 0;
	while (i < number_of_philosophers)
	{
		if (pthread_create(&thread[i], NULL, routine, &philo[i]) != 0)
		{
			printf("ERROR : pthread_create didn't work !\n");
			return (1);
		}
		i++;
	}
	// Wait for all threads to finish
	i = 0;
	while (i < number_of_philosophers)
	{
		pthread_join(thread[i], NULL);
		i++;
	}
	pthread_mutex_destroy(&print_mutex);
	i = 0;
	while (i < number_of_philosophers)
	{
		pthread_mutex_destroy(&forks[i]);
		i++;
	}
	printf("All the philo died. Bye.\n");
	return (0);
}