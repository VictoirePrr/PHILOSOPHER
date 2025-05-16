/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 10:49:07 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/16 14:29:22 by vicperri         ###   ########lyon.fr   */
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
