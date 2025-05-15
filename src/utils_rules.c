/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_rules.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 15:44:42 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/15 15:01:55 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	safe_release_both(t_fork *first, t_fork *second)
{
	release_fork(second);
	release_fork(first);
}

int	print_fork_status(t_philo *philo)
{
	if (!can_print(philo))
		return (1);
	pthread_mutex_lock(philo->print_mutex);
	printf("%ld %d has taken a fork\n", timestamp(philo->shared), philo->id);
	pthread_mutex_unlock(philo->print_mutex);
	return (0);
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

int	update_meal_time(t_philo *philo)
{
	if (check_if_dead(philo))
		return (1);
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
