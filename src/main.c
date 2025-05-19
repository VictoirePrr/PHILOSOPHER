/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 11:18:00 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/19 17:50:14 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	create_threads(t_data *data, t_monitor_data *monitor_data)
{
	int	i;

	i = 0;
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

static int	wait_for_start(t_philo *philo)
{
	int	started;

	while (1)
	{
		pthread_mutex_lock(&philo->shared->death_mutex);
		started = philo->shared->start_time != 0;
		pthread_mutex_unlock(&philo->shared->death_mutex);
		if (started)
			break ;
		my_usleep(1, philo);
	}
	if (philo->id == philo->rules->num_of_philo)
		my_usleep(1, philo);
	return (0);
}

void	*routine(void *args)
{
	t_philo	*philo;
	t_fork	*first;
	t_fork	*second;

	philo = (t_philo *)args;
	if (wait_for_start(philo) != 0)
		return (NULL);
	if (philo->id % 2 == 0)
		my_usleep(philo->rules->time_to_eat / 2, philo);
	init_forks(philo, &first, &second);
	if (handle_one_philo(philo) == 1)
		return (NULL);
	pthread_mutex_lock(philo->meal_mutex);
	philo->last_meal_time = get_time_in_ms();
	pthread_mutex_unlock(philo->meal_mutex);
	while (!should_philosopher_stop(philo))
	{
		while (take_fork(first, philo) != 0 && !should_philosopher_stop(philo))
			usleep(100);
		while (take_fork(second, philo) != 0 && !should_philosopher_stop(philo))
			usleep(100);
		if (eat(philo, first, second) == 0)
		{
			if (p_sleep(philo) == 1)
				break ;
			thinking(philo);
		}
	}
	return (NULL);
}

int	start_threads(t_data *data)
{
	t_monitor_data	monitor_data;
	int				thread_status;

	thread_status = create_threads(data, &monitor_data);
	if (thread_status != 0)
	{
		printf("ERROR : thread creation failed\n");
		join_and_cleanup(data, thread_status);
		return (1);
	}
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
	init_philo_data(&data);
	if (start_threads(&data) != 0)
		return (1);
	join_and_cleanup(&data, -1);
	return (0);
}
