#include "philo.h"

void	*my_thread(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;                 // retrieve your struct in arg
	pthread_mutex_lock(philo->print_mutex); // Lock before printing
	printf("Philosopher %d is thinking...\n", philo->id);
	pthread_mutex_unlock(philo->print_mutex); // Unlock after printing
	return (NULL);
}

void	init_philosophers(t_philo *philo, pthread_mutex_t *print_mutex, int num)
{
	int	i;

	i = 0;
	while (i < num)
	{
		philo[i].id = i + 1; // to start at 1.
		philo[i].print_mutex = print_mutex;
		// assign shared print_mutex to each philosopher
		i++;
	}
}

int	init_mutexes(pthread_mutex_t *mutexes, int num)
{
	int	i;

	i = 0;
	while (i < num)
	{
		if (pthread_mutex_init(&mutexes[i], NULL) != 0)
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

	// Init the print_mutex
	pthread_mutex_init(&print_mutex, NULL);

	// Fill the structs with the shared print_mutex
	init_philosophers(philo, &print_mutex, number_of_philosophers);

	// Create the threads
	i = 0;
	while (i < number_of_philosophers)
	{
		if (pthread_create(&thread[i], NULL, my_thread, &philo[i]) != 0)
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

	// Now, destroy the shared print_mutex once all threads are finished
	pthread_mutex_destroy(&print_mutex);
		// This should be done after threads finish

	// Clean up done
	printf("All threads finished 👋\n");
	return (0);
}