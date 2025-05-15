/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 10:49:07 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/15 14:42:02 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	check_if_dead(t_philo *philo)
{
	int	is_dead;

	pthread_mutex_lock(&philo->shared->death_mutex);
	is_dead = philo->shared->someone_died;
	pthread_mutex_unlock(&philo->shared->death_mutex);
	return (is_dead);
}

int	can_print(t_philo *philo)
{
	int	status;

	pthread_mutex_lock(&philo->shared->death_mutex);
	status = !philo->shared->someone_died;
	pthread_mutex_unlock(&philo->shared->death_mutex);
	return (status);
}

int	take_fork(t_fork *fork)
{
	pthread_mutex_lock(&fork->mutex);
	if (fork->is_taken == 0)
	{
		fork->is_taken = 1;
		pthread_mutex_unlock(&fork->mutex);
		return (1);
	}
	pthread_mutex_unlock(&fork->mutex);
	return (0);
}

void	release_fork(t_fork *fork)
{
	pthread_mutex_lock(&fork->mutex);
	if (fork->is_taken == 1)
	{
		fork->is_taken = 0;
		pthread_mutex_unlock(&fork->mutex);
	}
	pthread_mutex_unlock(&fork->mutex);
}

void	init_forks(t_philo *philo, t_fork **first, t_fork **second)
{
	if (philo->id % 2 == 0)
	{
		*first = philo->right_fork;
		*second = philo->left_fork;
	}
	else
	{
		*first = philo->left_fork;
		*second = philo->right_fork;
	}
}

int	check_death_status(t_shared *shared)
{
	int	is_dead;

	pthread_mutex_lock(&shared->death_mutex);
	is_dead = shared->someone_died;
	pthread_mutex_unlock(&shared->death_mutex);
	return (is_dead);
}
