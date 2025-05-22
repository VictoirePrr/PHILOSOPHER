/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rules_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 15:44:42 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/22 13:12:42 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	release_fork(t_fork *fork)
{
	pthread_mutex_lock(&fork->mutex);
	fork->is_taken = 0;
	pthread_mutex_unlock(&fork->mutex);
}

int	print_fork_status(t_philo *philo)
{
	pthread_mutex_lock(philo->print_mutex);
	printf("%ld %d has taken a fork\n", timestamp(philo->shared), philo->id);
	pthread_mutex_unlock(philo->print_mutex);
	return (0);
}

int	update_meal_time(t_philo *philo)
{
	pthread_mutex_lock(philo->meal_mutex);
	philo->last_meal_time = get_time_in_ms();
	philo->meals_eaten++;
	pthread_mutex_unlock(philo->meal_mutex);
	return (0);
}

int	print_eating(t_philo *philo)
{
	if (check_if_dead(philo) == 1)
		return (1);
	pthread_mutex_lock(philo->print_mutex);
	printf("%ld %d is eating\n", timestamp(philo->shared), philo->id);
	pthread_mutex_unlock(philo->print_mutex);
	return (0);
}

int	check_if_dead(t_philo *philo)
{
	int	is_dead;

	pthread_mutex_lock(&philo->shared->death_mutex);
	is_dead = philo->shared->someone_died;
	pthread_mutex_unlock(&philo->shared->death_mutex);
	return (is_dead);
}
