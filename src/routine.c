/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 10:59:52 by vicperri          #+#    #+#             */
/*   Updated: 2025/04/30 11:36:14 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	handle_routine(t_philo *philo)
{
	pthread_mutex_t	print_mutex;
	t_fork			forks[philo->num];

	pthread_mutex_init(&print_mutex, NULL);
	if (init_forks(forks, philo->num) == ERROR)
		return (ERROR);
	init_philosophers(philo, &print_mutex, forks);
	if (start_routine(philo) == ERROR)
		return (ERROR);
	destroy_mutex(philo, &print_mutex, forks);
	return (SUCCESS);
}

void	init_philosophers(t_philo *philo, pthread_mutex_t *print_mutex,
		t_fork *forks)
{
	int	i;

	i = 0;
	while (i < philo->num)
	{
		philo[i].id = i + 1; // to start at 1.
		philo[i].print_mutex = print_mutex;
		philo[i].left_fork = &forks[i];
		philo[i].right_fork = &forks[(i + 1) % philo->num]; // calcul a revoir
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

int	start_routine(t_philo *philo)
{
	int			i;
	pthread_t	thread[philo->num];

	i = 0;
	while (i < philo->num)
	{
		if (pthread_create(&thread[i], NULL, routine, &philo[i]) != 0)
		{
			printf("ERROR : pthread_create didn't work\n");
			return (ERROR);
		}
		i++;
	}
	i = 0;
	while (i < philo->num)
	{
		pthread_join(thread[i], NULL);
		i++;
	}
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
