/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rules.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 10:51:01 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/12 15:46:03 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	handle_second_fork(t_philo *philo, t_fork *first, t_fork *second)
{
	if (take_fork(second) == 1)
	{
		if (check_if_dead(philo) == 1)
		{
			safe_release_both(first, second);
			return (1);
		}
		print_fork_status(philo, 2);
		update_meal_time(philo);
		print_eating(philo);
		my_usleep(philo->rules->time_to_eat, philo);
		safe_release_both(first, second);
		if (check_meals_completed(philo))
			return (1);
		return (0);
	}
	release_fork(first);
	return (1);
}

int	eat(t_philo *philo, t_fork *first, t_fork *second)
{
	if (check_if_dead(philo) == 1)
		return (1);
	if (check_meals_completed(philo))
		return (1);
	if (take_fork(first) == 1)
	{
		if (check_if_dead(philo) == 1)
		{
			release_fork(first);
			return (1);
		}
		print_fork_status(philo, 1);
		return (handle_second_fork(philo, first, second));
	}
	return (1);
}

int	think(t_philo *philo)
{
	if (check_if_dead(philo) == 1)
		return (1);
	pthread_mutex_lock(philo->print_mutex);
	printf(GREEN "[%ld] %d is thinking." RESET "\n", timestamp(philo->shared),
		philo->id);
	pthread_mutex_unlock(philo->print_mutex);
	my_usleep(100, philo);
	return (0);
}

int	p_sleep(t_philo *philo)
{
	if (check_if_dead(philo) == 1)
		return (1);
	pthread_mutex_lock(philo->print_mutex);
	printf(BLUE "[%ld] %d is sleeping." RESET "\n", timestamp(philo->shared),
		philo->id);
	pthread_mutex_unlock(philo->print_mutex);
	my_usleep(philo->rules->time_to_sleep, philo);
	return (0);
}
