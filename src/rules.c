/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rules.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 10:51:01 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/12 15:31:05 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	safe_release_both(t_fork *first, t_fork *second)
{
	release_fork(second);
	release_fork(first);
}

static int	print_fork_status(t_philo *philo, int fork_num)
{
	pthread_mutex_lock(philo->print_mutex);
	printf(LIGHT_PINK "[%ld] %d has taken fork %d." RESET "\n",
		timestamp(philo->shared), philo->id, fork_num);
	pthread_mutex_unlock(philo->print_mutex);
	return (0);
}

static int	check_meals_completed(t_philo *philo)
{
	int	meals;

	pthread_mutex_lock(philo->meal_mutex);
	meals = philo->meals_eaten;
	pthread_mutex_unlock(philo->meal_mutex);
	if (philo->rules->num_must_eat != -1 && meals >= philo->rules->num_must_eat)
		return (1);
	return (0);
}

static int	update_meal_time(t_philo *philo)
{
	pthread_mutex_lock(philo->meal_mutex);
	philo->last_meal_time = get_time_in_ms();
	philo->meals_eaten++;
	pthread_mutex_unlock(philo->meal_mutex);
	return (0);
}

static int	print_eating(t_philo *philo)
{
	pthread_mutex_lock(philo->print_mutex);
	printf(HOT_PINK "[%ld] %d is eating." RESET "\n", timestamp(philo->shared),
		philo->id);
	pthread_mutex_unlock(philo->print_mutex);
	return (0);
}

static int	handle_second_fork(t_philo *philo, t_fork *first, t_fork *second)
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
	// 4. Think
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
	// 5. Sleep
	if (check_if_dead(philo) == 1)
		return (1);
	pthread_mutex_lock(philo->print_mutex);
	printf(BLUE "[%ld] %d is sleeping." RESET "\n", timestamp(philo->shared),
		philo->id);
	pthread_mutex_unlock(philo->print_mutex);
	my_usleep(philo->rules->time_to_sleep, philo);
	return (0);
}
