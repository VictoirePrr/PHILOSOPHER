/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rules.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 10:51:01 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/19 17:58:35 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	eat(t_philo *philo, t_fork *first, t_fork *second)
{
	if (check_if_dead(philo) == 1 || check_meals_completed(philo) == 1)
	{
		safe_release_both(first, second);
		return (1);
	}
	if (update_meal_time(philo) == 1 || print_eating(philo) == 1)
	{
		safe_release_both(first, second);
		return (1);
	}
	my_usleep(philo->rules->time_to_eat, philo);
	safe_release_both(first, second);
	if (check_meals_completed(philo) == 1)
		return (1);
	return (0);
}

void	thinking(t_philo *philo)
{
	if (!can_print(philo))
		return ;
	pthread_mutex_lock(philo->print_mutex);
	printf("%ld %d is thinking\n", timestamp(philo->shared), philo->id);
	pthread_mutex_unlock(philo->print_mutex);
	usleep(400);
}

int	p_sleep(t_philo *philo)
{
	if (!can_print(philo))
		return (1);
	pthread_mutex_lock(philo->print_mutex);
	printf("%ld %d is sleeping\n", timestamp(philo->shared), philo->id);
	pthread_mutex_unlock(philo->print_mutex);
	my_usleep(philo->rules->time_to_sleep, philo);
	return (0);
}

int	take_fork(t_fork *fork, t_philo *philo)
{
	int	success;

	pthread_mutex_lock(&fork->mutex);
	success = (fork->is_taken == 0);
	if (success)
	{
		fork->is_taken = 1;
		pthread_mutex_unlock(&fork->mutex);
		if (print_fork_status(philo)) // Print after securing the fork
			return (1);
		return (0);
	}
	pthread_mutex_unlock(&fork->mutex);
	return (1);
}

void	release_fork(t_fork *fork)
{
	pthread_mutex_lock(&fork->mutex);
	if (fork->is_taken == 1)
		fork->is_taken = 0;
	pthread_mutex_unlock(&fork->mutex);
}

int	handle_one_philo(t_philo *philo)
{
	if (philo->rules->num_of_philo == 1)
	{
		print_fork_status(philo);
		my_usleep(philo->rules->time_to_die, philo);
		return (1);
	}
	return (0);
}
