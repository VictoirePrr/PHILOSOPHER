/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_mutexes.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 15:52:59 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/16 13:22:39 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	init_basic_mutexes(t_data *data)
{
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&data->meal_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&data->print_mutex);
		return (1);
	}
	if (pthread_mutex_init(&data->slow_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&data->print_mutex);
		pthread_mutex_destroy(&data->meal_mutex);
		return (1);
	}
	if (pthread_mutex_init(&data->shared.death_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&data->print_mutex);
		pthread_mutex_destroy(&data->meal_mutex);
		pthread_mutex_destroy(&data->slow_mutex);
		return (1);
	}
	data->shared.someone_died = 0;
	data->shared.start_time = 0;
	return (0);
}

void	destroy_forks(t_data *data, int num)
{
	int	i;

	i = 0;
	if (num == -1)
		num = data->rules.num_of_philo;
	while (i < num)
	{
		pthread_mutex_destroy(&data->forks[i].mutex);
		i++;
	}
	free(data->forks);
}

int	init_fork_mutexes(t_data *data)
{
	int	i;

	i = 0;
	data->forks = malloc(data->rules.num_of_philo * sizeof(t_fork));
	if (!data->forks)
	{
		destroy_mutexes(data);
		return (1);
	}
	while (i < data->rules.num_of_philo)
	{
		if (pthread_mutex_init(&data->forks[i].mutex, NULL) != 0)
		{
			destroy_mutexes(data);
			destroy_forks(data, i);
			return (1);
		}
		data->forks[i].is_taken = 0;
		i++;
	}
	return (0);
}

int	init_mutexes(t_data *data)
{
	if (init_basic_mutexes(data) != 0)
		return (1);
	if (init_fork_mutexes(data) != 0)
		return (1);
	return (0);
}
