/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 10:49:07 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/22 13:13:07 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long long	get_time_in_ms(void)
{
	struct timeval	current_time;

	gettimeofday(&current_time, NULL);
	return (current_time.tv_sec * 1000 + current_time.tv_usec / 1000);
}

long	timestamp(t_shared *shared)
{
	return (get_time_in_ms() - shared->start_time);
}

void	my_usleep(long duration, t_philo *philo)
{
	long	start_time;
	long	elapsed;

	start_time = get_time_in_ms();
	while (1)
	{
		if (check_if_dead(philo))
			break ;
		elapsed = get_time_in_ms() - start_time;
		if (elapsed >= duration)
			break ;
		usleep(500);
	}
}

int	check_meals_completed(t_philo *philo)
{
	int	meals;

	pthread_mutex_lock(philo->meal_mutex);
	meals = philo->meals_eaten;
	pthread_mutex_unlock(philo->meal_mutex);
	if (philo->rules->num_must_eat != -1 && meals >= philo->rules->num_must_eat)
		return (1);
	return (0);
}

int	check_death_status(t_shared *shared)
{
	int	is_dead;

	pthread_mutex_lock(&shared->death_mutex);
	is_dead = shared->someone_died;
	pthread_mutex_unlock(&shared->death_mutex);
	return (is_dead);
}
