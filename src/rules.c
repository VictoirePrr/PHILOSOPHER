/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rules.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 10:51:01 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/22 15:01:24 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	eat(t_philo *philo, t_fork *first, t_fork *second, int *num_of_forks)
{
	philo->print_status = 0;
	if (*num_of_forks == 2)
	{
		if ((check_if_dead(philo)) == 1 || check_meals_completed(philo) == 1)
		{
			safe_release_both(first, second, num_of_forks);
			return (1);
		}
		update_meal_time(philo);
		if (print_eating(philo) == 1)
		{
			safe_release_both(first, second, num_of_forks);
			return (1);
		}
		my_usleep(philo->rules->time_to_eat, philo);
		safe_release_both(first, second, num_of_forks);
	}
	else
	{
		safe_release_both(first, second, num_of_forks);
		return (1);
	}
	return (0);
}

void	thinking(t_philo *philo)
{
	if ((check_if_dead(philo)) == 1)
		return ;
	if (philo->print_status == 0 && philo->rules->num_of_philo > 1)
	{
		pthread_mutex_lock(philo->print_mutex);
		printf("%ld %d is thinking\n", timestamp(philo->shared), philo->id);
		philo->print_status = 1;
		pthread_mutex_unlock(philo->print_mutex);
	}
	usleep(50);
}

int	p_sleep(t_philo *philo)
{
	if ((check_if_dead(philo)) == 1)
		return (1);
	pthread_mutex_lock(philo->print_mutex);
	printf("%ld %d is sleeping\n", timestamp(philo->shared), philo->id);
	pthread_mutex_unlock(philo->print_mutex);
	my_usleep(philo->rules->time_to_sleep, philo);
	return (0);
}

int	take_fork(t_fork *first, t_fork *second, t_philo *philo, int *num_of_forks)
{
	pthread_mutex_lock(&first->mutex);
	if (first->is_taken == 0)
	{
		first->is_taken = 1;
		*num_of_forks += 1;
		print_fork_status(philo);
	}
	pthread_mutex_unlock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	if (second->is_taken == 0)
	{
		second->is_taken = 1;
		*num_of_forks += 1;
		print_fork_status(philo);
	}
	pthread_mutex_unlock(&second->mutex);
	if (*num_of_forks == 2)
		return (0);
	else
		return (1);
}

void	safe_release_both(t_fork *first, t_fork *second, int *num_of_forks)
{
	release_fork(first);
	release_fork(second);
	*num_of_forks = 0;
}
