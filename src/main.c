/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 11:18:00 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/21 18:02:31 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	start_philo(t_data *data, t_monitor_data *monitor_data)
{
	int	i;

	i = 0;
	printf(" Philosopher threads starting\n");
	monitor_data->num_of_philo = data->rules.num_of_philo;
	monitor_data->time_to_die = data->rules.time_to_die;
	monitor_data->shared = &data->shared;
	monitor_data->philos = data->philos;
	while (i < data->rules.num_of_philo)
	{
		if (pthread_create(&data->threads[i], NULL, routine,
				&data->philos[i]) != 0)
			return (i);
		i++;
	}
	pthread_mutex_lock(&data->shared.death_mutex);
	data->shared.start_time = get_time_in_ms();
	pthread_mutex_unlock(&data->shared.death_mutex);
	if (pthread_create(&data->monitor, NULL, monitor_routine,
			monitor_data) != 0)
		return (-1);
	return (0);
}

int	main(int argc, char **argv)
{
	t_data	data;

	if (init_rules(&data.rules, argc, argv) == 1)
	{
		printf("ERROR : invalid arguments\n");
		return (1);
	}
	if (init_mutexes(&data) != 0)
	{
		printf("ERROR : mutex initialization failed\n");
		return (1);
	}
	if (init_philosophers(&data) != 0)
	{
		printf("ERROR : philosophers initialization failed\n");
		return (1);
	}
	printf(" Philosopher simulation started\n");
	init_philo_data(&data);
	printf(" Philosopher data initialized\n");
	if (check_creation(&data) != 0)
		return (1);
	printf(" Philosopher threads created\n");
	join_and_cleanup(&data, -1);
	printf(" Philosopher simulation ended\n");
	return (0);
}
