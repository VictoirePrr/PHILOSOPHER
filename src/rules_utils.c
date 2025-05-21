/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rules_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 15:44:42 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/21 17:27:54 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	release_fork(t_fork *fork)
{
	pthread_mutex_lock(&fork->mutex);
	fork->is_taken = 0;
	pthread_mutex_unlock(&fork->mutex);
}

void	safe_release_both(t_fork *first, t_fork *second, int *num_of_forks)
{
	release_fork(first);
	release_fork(second);
	*num_of_forks = 0;
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
	if (check_if_dead(philo))
		return (1);
	pthread_mutex_lock(philo->print_mutex);
	printf("%ld %d is eating\n", timestamp(philo->shared), philo->id);
	pthread_mutex_unlock(philo->print_mutex);
	return (0);
}
