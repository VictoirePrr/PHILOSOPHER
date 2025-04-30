/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 10:59:52 by vicperri          #+#    #+#             */
/*   Updated: 2025/04/30 15:44:43 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	handle_routine(t_data *data)
{
	pthread_t	*threads;
	int			i;

	// Initialize threads array
	threads = malloc(sizeof(pthread_t) * data->nb_philo);
	if (!threads)
		return (ERROR);
	if (start_routine(data, threads) == ERROR)
		return (ERROR);
	return (SUCCESS);
}

int	init_philosophers(t_philo *philo, t_fork *forks, t_data *data)
{
	int	i;

	i = 0;
	while (i < data->nb_philo)
	{
		philo[i].id = i + 1; // to start at 1.
		philo[i].times_eaten = 0;
		philo[i].last_meal_time = get_time_in_ms();
		philo[i].data = data;
		philo[i].left_fork = &forks[i];
		philo[i].right_fork = &forks[(i + 1) % data->nb_philo];
		// calcul a revoir
		if (pthread_mutex_init(&philo[i].meal_mutex, NULL) != 0)
		{
			printf("ERROR: failed mutex init\n");
			return (ERROR);
		}
		i++;
	}
}

int	init_forks(t_fork *forks, int num)
{
	int	i;

	i = 0;
	while (i < num)
	{
		forks[i].is_taken = 0;
		if (pthread_mutex_init(&forks[i].mutex, NULL) != 0)
		{
			printf("ERROR : failed mutex init\n");
			return (ERROR);
		}
		i++;
	}
	return (SUCCESS);
}

int	start_routine(t_data *data, pthread_t *thread)
{
	int	i;

	i = 0;
	while (i < data->nb_philo)
	{
		if (pthread_create(&thread[i], NULL, routine, &data->philos[i]) != 0)
		{
			printf("ERROR : pthread_create didn't work\n");
			free(thread);
			return (ERROR);
		}
		i++;
	}
	i = 0;
	while (i < data->nb_philo)
	{
		pthread_join(thread[i], NULL);
		i++;
	}
	free(thread);
	return (SUCCESS);
}

void	destroy_mutex(t_philo *philo, pthread_mutex_t *print_mutex,
		t_fork *forks)
{
	int	i;

	pthread_mutex_destroy(print_mutex);
	i = 0;
	while (i < philo->num)
	{
		pthread_mutex_destroy(&forks[i].mutex);
		i++;
	}
}
