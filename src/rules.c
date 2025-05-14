/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rules.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 10:51:01 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/14 14:47:42 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// static int	should_slow_down(t_philo *philo)
// {
// 	int	should_slow;

// 	should_slow = 0;
// 	if (philo->rules->num_of_philo < 10 && philo->rules->num_of_philo % 2 != 0
// 		&& philo->rules->time_to_sleep < philo->rules->time_to_eat)
// 		should_slow = 1;
// 	return (should_slow);
// }

int	handle_second_fork(t_philo *philo, t_fork *first, t_fork *second)
{
	if (take_fork(second) == 1)
	{
		if (check_if_dead(philo) == 1)
		{
			safe_release_both(first, second);
			return (1);
		}
		print_fork_status(philo, 1);
		print_fork_status(philo, 2);
		if (update_meal_time(philo) == 1)
		{
			safe_release_both(first, second);
			return (1);
		}
		if (print_eating(philo) == 1)
		{
			safe_release_both(first, second);
			return (1);
		}
		my_usleep(philo->rules->time_to_eat, philo);
		safe_release_both(first, second);
		if (check_if_dead(philo) == 1)
		{
			safe_release_both(first, second);
			return (1);
		}
		if (check_meals_completed(philo))
			return (1);
		return (0);
	}
	release_fork(first);
	return (1);
}

int	eat(t_philo *philo, t_fork *first, t_fork *second)
{
	int	status;
	int	should_slow;

	should_slow = 0;
	if (check_if_dead(philo) == 1)
		return (1);
	if (check_meals_completed(philo))
		return (1);
	// if (should_slow_down(philo) == 1)
	// {
	// 	pthread_mutex_lock(philo->slow_mutex);
	// 	should_slow = 1;
	// }
	status = take_fork(first);
	if (status == 1)
	{
		if (check_if_dead(philo) == 1)
		{
			// if (should_slow == 1)
			// 	pthread_mutex_unlock(philo->slow_mutex);
			release_fork(first);
			return (1);
		}
		// if (should_slow == 1)
		// 	pthread_mutex_unlock(philo->slow_mutex);
		status = handle_second_fork(philo, first, second);
		return (status);
	}
	// if (should_slow == 1)
	// 	pthread_mutex_unlock(philo->slow_mutex);
	return (1);
}

int	think(t_philo *philo)
{
	if (check_if_dead(philo) == 1)
		return (1);
	usleep(100);
	return (0);
}

int	p_sleep(t_philo *philo)
{
	if (!can_print(philo))
		return (1);
	pthread_mutex_lock(philo->print_mutex);
	printf(BLUE "[%ld] %d is sleeping." RESET "\n", timestamp(philo->shared),
		philo->id);
	pthread_mutex_unlock(philo->print_mutex);
	my_usleep(philo->rules->time_to_sleep, philo);
	if (check_if_dead(philo))
		return (1);
	pthread_mutex_lock(philo->print_mutex);
	printf(GREEN "[%ld] %d is thinking." RESET "\n", timestamp(philo->shared),
		philo->id);
	pthread_mutex_unlock(philo->print_mutex);
	return (0);
}

int	handle_one_philo(t_philo *philo)
{
	if (philo->rules->num_of_philo == 1)
	{
		print_fork_status(philo, philo->right_fork_id);
		my_usleep(philo->rules->time_to_die, philo);
		return (1);
	}
	return (0);
}