/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 10:59:52 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/22 14:54:53 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	wait_for_start(t_philo *philo)
{
	int	started;
	int	end;

	while (1)
	{
		pthread_mutex_lock(&philo->shared->death_mutex);
		started = philo->shared->start_time != 0;
		pthread_mutex_unlock(&philo->shared->death_mutex);
		if (started > 0)
			break ;
		pthread_mutex_lock(&philo->shared->death_mutex);
		end = philo->shared->end != 0;
		pthread_mutex_unlock(&philo->shared->death_mutex);
		if (end == 1)
			return (1);
		my_usleep(1, philo);
	}
	if (philo->id == philo->rules->num_of_philo)
		my_usleep(1, philo);
	return (0);
}

void	init_forks(t_philo *philo, t_fork **first, t_fork **second)
{
	if (philo->id % 2 == 0)
	{
		*first = philo->left_fork;
		*second = philo->right_fork;
		my_usleep(philo->rules->time_to_eat / 2, philo);
	}
	else
	{
		*first = philo->right_fork;
		*second = philo->left_fork;
	}
}

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
	int		num_of_forks;

	num_of_forks = 0;
	philo = (t_philo *)args;
	if (wait_for_start(philo) != 0)
		return (NULL);
	init_forks(philo, &first, &second);
	pthread_mutex_lock(philo->meal_mutex);
	philo->last_meal_time = get_time_in_ms();
	pthread_mutex_unlock(philo->meal_mutex);
	while (!should_philosopher_stop(philo))
	{
		if (take_fork(first, second, philo, &num_of_forks) == 0)
		{
			if (eat(philo, first, second, &num_of_forks) == 0)
				if (p_sleep(philo) == 1)
					break ;
		}
		else
			thinking(philo);
	}
	return (NULL);
}
