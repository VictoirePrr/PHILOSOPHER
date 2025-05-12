/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 10:59:52 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/12 15:50:11 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	should_philosopher_stop(t_philo *philo)
{
	int	meals;

	if (check_if_dead(philo))
		return (1);
	pthread_mutex_lock(philo->meal_mutex);
	meals = philo->meals_eaten;
	pthread_mutex_unlock(philo->meal_mutex);
	if (philo->rules->num_must_eat != -1 && meals >= philo->rules->num_must_eat)
		return (1);
	return (0);
}

void	*routine(void *args)
{
	t_philo	*philo;
	t_fork	*first;
	t_fork	*second;

	philo = (t_philo *)args;
	while (philo->shared->start_time == 0)
		my_usleep(50, philo);
	my_usleep(philo->id * 10, philo);
	init_forks(philo, &first, &second);
	pthread_mutex_lock(philo->meal_mutex);
	philo->last_meal_time = get_time_in_ms();
	pthread_mutex_unlock(philo->meal_mutex);
	while (!should_philosopher_stop(philo))
	{
		if (think(philo) == 1)
			break ;
		if (eat(philo, first, second) == 0)
			if (p_sleep(philo) == 1)
				break ;
	}
	return (NULL);
}

int	check_philosopher_status(t_monitor_data *data, int i)
{
	long	time_since_meal;
	int		meals;

	pthread_mutex_lock(data->philos[i].meal_mutex);
	time_since_meal = get_time_in_ms() - data->philos[i].last_meal_time;
	meals = data->philos[i].meals_eaten;
	pthread_mutex_unlock(data->philos[i].meal_mutex);
	if (time_since_meal > data->time_to_die)
	{
		pthread_mutex_lock(data->philos[i].print_mutex);
		printf(RED "[%ld] %d died." RESET "\n", timestamp(data->shared),
			data->philos[i].id);
		pthread_mutex_unlock(data->philos[i].print_mutex);
		pthread_mutex_lock(&data->shared->death_mutex);
		data->shared->someone_died = 1;
		pthread_mutex_unlock(&data->shared->death_mutex);
		return (1);
	}
	return (meals >= data->philos[i].rules->num_must_eat);
}

int	check_all_philosophers(t_monitor_data *data)
{
	int	i;
	int	finished;

	i = 0;
	finished = 0;
	while (i < data->num_of_philo)
	{
		if (check_philosopher_status(data, i))
			finished++;
		i++;
	}
	return (finished == data->num_of_philo);
}

void	*monitor_routine(void *args)
{
	t_monitor_data	*data;

	data = (t_monitor_data *)args;
	while (!check_death_status(data->shared))
	{
		if (check_all_philosophers(data))
			break ;
		my_usleep(data->time_to_die / 50, data->philos);
	}
	free(data);
	return (NULL);
}
