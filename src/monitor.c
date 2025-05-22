/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 13:16:13 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/22 12:29:52 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	report_death(t_monitor_data *data, int i)
{
	pthread_mutex_lock(&data->shared->death_mutex);
	if (data->shared->someone_died == 0)
	{
		data->shared->someone_died = 1;
		pthread_mutex_lock(data->philos[i].print_mutex);
		printf("%ld %d died\n", timestamp(data->shared), data->philos[i].id);
		pthread_mutex_unlock(data->philos[i].print_mutex);
	}
	pthread_mutex_unlock(&data->shared->death_mutex);
	return (1);
}

int	check_philosopher_status(t_monitor_data *data, int i)
{
	long	time_since_meal;
	int		meals;
	int		status;

	pthread_mutex_lock(data->philos[i].meal_mutex);
	time_since_meal = get_time_in_ms() - data->philos[i].last_meal_time;
	meals = data->philos[i].meals_eaten;
	pthread_mutex_unlock(data->philos[i].meal_mutex);
	if (time_since_meal > (long)data->time_to_die)
	{
		status = report_death(data, i);
		return (status);
	}
	if (data->philos[i].rules->num_must_eat != -1)
		return (meals >= data->philos[i].rules->num_must_eat);
	return (0);
}

int	check_all_philosophers(t_monitor_data *data)
{
	int	i;
	int	finished;
	int	result;

	i = 0;
	finished = 0;
	while (i < data->num_of_philo)
	{
		if (check_philosopher_status(data, i))
			finished++;
		i++;
	}
	result = finished == data->num_of_philo;
	return (result);
}

void	*monitor_routine(void *args)
{
	t_monitor_data	*data;

	data = (t_monitor_data *)args;
	while (check_death_status(data->shared) != 1)
	{
		if (check_all_philosophers(data))
			break ;
		usleep(400);
	}
	return (NULL);
}
