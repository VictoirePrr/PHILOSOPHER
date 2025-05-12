/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 10:49:07 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/12 15:49:04 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	check_if_dead(t_philo *philo)
{
	pthread_mutex_lock(&philo->shared->death_mutex);
	if (philo->shared->someone_died)
	{
		pthread_mutex_unlock(&philo->shared->death_mutex);
		return (1);
	}
	pthread_mutex_unlock(&philo->shared->death_mutex);
	return (0);
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
	fork->is_taken = 0;
	pthread_mutex_unlock(&fork->mutex);
}
void	init_forks(t_philo *philo, t_fork **first, t_fork **second)
{
	if (philo->left_fork_id < philo->right_fork_id)
	{
		*first = philo->left_fork;
		*second = philo->right_fork;
	}
	else
	{
		*first = philo->right_fork;
		*second = philo->left_fork;
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
