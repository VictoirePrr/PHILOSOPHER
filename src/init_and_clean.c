/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_and_clean.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 14:06:35 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/16 14:31:10 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	init_rules(t_rules *rules, int argc, char **argv)
{
	int	i;

	i = 1;
	if (argc != 5 && argc != 6)
		return (1);
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

void	destroy_mutexes(t_data *data)
{
	pthread_mutex_destroy(&data->print_mutex);
	pthread_mutex_destroy(&data->meal_mutex);
	pthread_mutex_destroy(&data->shared.death_mutex);
	pthread_mutex_destroy(&data->slow_mutex);
}

void	init_philo_data(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->rules.num_of_philo)
	{
		data->ids[i] = i + 1;
		data->philos[i].id = data->ids[i];
		data->philos[i].meals_eaten = 0;
		data->philos[i].print_mutex = &data->print_mutex;
		data->philos[i].slow_mutex = &data->slow_mutex;
		data->philos[i].left_fork = &data->forks[i];
		data->philos[i].left_fork_id = i;
		data->philos[i].right_fork = &data->forks[(i + 1)
			% data->rules.num_of_philo];
		data->philos[i].right_fork_id = (i + 1) % data->rules.num_of_philo;
		data->philos[i].rules = &data->rules;
		data->philos[i].last_meal_time = get_time_in_ms();
		data->philos[i].meal_mutex = &data->meal_mutex;
		data->philos[i].shared = &data->shared;
		i++;
	}
}

int	init_philosophers(t_data *data)
{
	data->philos = malloc(data->rules.num_of_philo * sizeof(t_philo));
	if (!data->philos)
	{
		destroy_mutexes(data);
		destroy_forks(data, -1);
		return (1);
	}
	data->threads = malloc(data->rules.num_of_philo * sizeof(pthread_t));
	if (!data->threads)
	{
		destroy_mutexes(data);
		destroy_forks(data, -1);
		free(data->philos);
		return (1);
	}
	data->ids = malloc(data->rules.num_of_philo * sizeof(int));
	if (!data->ids)
	{
		destroy_mutexes(data);
		destroy_forks(data, -1);
		free(data->philos);
		free(data->threads);
		return (1);
	}
	return (0);
}

int	join_and_cleanup(t_data *data, int num)
{
	int	i;

	i = 0;
	if (num != -2)
	{
		if (num == -1)
			num = data->rules.num_of_philo;
		while (i < num)
		{
			if (pthread_join(data->threads[i], NULL) != 0)
				return (1);
			i++;
		}
		if (num != -1)
			if (pthread_join(data->monitor, NULL) != 0)
				return (1);
	}
	destroy_mutexes(data);
	destroy_forks(data, -1);
	free(data->threads);
	free(data->philos);
	free(data->ids);
	return (0);
}
