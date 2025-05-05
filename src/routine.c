/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 10:59:52 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/05 16:21:09 by vicperri         ###   ########lyon.fr   */
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
		return (fork->is_taken);
	}
	else
		pthread_mutex_unlock(&fork->mutex);
	return (0);
}

void	release_fork(t_fork *fork)
{
	pthread_mutex_lock(&fork->mutex);
	fork->is_taken = 0;
	pthread_mutex_unlock(&fork->mutex);
}

void	*routine(void *args)
{
	t_philo	*philo;
	t_fork	*first;
	t_fork	*second;

	philo = (t_philo *)args;
	while (check_if_dead(philo) != 1)
	{
		// 1. Define which forks
		if (philo->id % 2 == 0)
		{
			first = philo->left_fork;
			second = philo->right_fork;
		}
		else
		{
			first = philo->right_fork;
			second = philo->left_fork;
		}
		// 2. Eat
		if ((take_fork(first) == 1) && (take_fork(second) == 1))
		{ // if the first is good but not the second
			if (check_if_dead(philo) == 1) // Dead before eating
			{
				release_fork(first);
				release_fork(second);
				break ;
			}
			pthread_mutex_lock(philo->print_mutex);
			printf("%ld Philosopher %d is eating\n", timestamp(philo->shared),
				philo->id);
			philo->meals_eaten++;
			pthread_mutex_unlock(philo->print_mutex);
			pthread_mutex_lock(philo->meal_mutex);
			philo->last_meal_time = get_time_in_ms();
			pthread_mutex_unlock(philo->meal_mutex);
			usleep(philo->rules->time_to_eat * 1000);
			// 3. Drop forks
			release_fork(first);
			release_fork(second);
		}
		else
		{
			// 4. Think
			pthread_mutex_lock(philo->print_mutex);
			printf("%ld Philosopher %d is thinking.\n",
				timestamp(philo->shared), philo->id);
			pthread_mutex_unlock(philo->print_mutex);
			// usleep(100);
		}
		// 5. Sleep
		if (check_if_dead(philo) == 1)
			break ;
		pthread_mutex_lock(philo->print_mutex);
		printf("%ld Philosopher %d is sleeping\n", timestamp(philo->shared),
			philo->id);
		pthread_mutex_unlock(philo->print_mutex);
		usleep(philo->rules->time_to_sleep * 1000);
	}
	return (NULL);
}

void	*monitor_routine(void *args)
{
	t_monitor_data	*monitor_data;
	int				i;
	int				finished;
	long			time_since_meal;

	monitor_data = (t_monitor_data *)args;
	while (1)
	{
		i = 0;
		finished = 0;
		pthread_mutex_lock(&monitor_data->shared->death_mutex);
		if (monitor_data->shared->someone_died == 1)
		{
			pthread_mutex_unlock(&monitor_data->shared->death_mutex);
			return (NULL); // Stop monitoring, someone already died
		}
		pthread_mutex_unlock(&monitor_data->shared->death_mutex);
		while (i < monitor_data->num_of_philo)
		{
			pthread_mutex_lock(monitor_data->philos[i].meal_mutex);
			time_since_meal = get_time_in_ms()
				- monitor_data->philos[i].last_meal_time;
			pthread_mutex_unlock(monitor_data->philos[i].meal_mutex);
			if (time_since_meal > monitor_data->time_to_die)
			{
				pthread_mutex_lock(monitor_data->philos[i].print_mutex);
				printf("%lld Philosopher %d died.\n", get_time_in_ms()
					- monitor_data->shared->start_time,
					monitor_data->philos[i].id);
				pthread_mutex_unlock(monitor_data->philos[i].print_mutex);
				pthread_mutex_lock(&monitor_data->shared->death_mutex);
				monitor_data->shared->someone_died = 1;
				pthread_mutex_unlock(&monitor_data->shared->death_mutex);
				return (NULL);
			}
			if (monitor_data->philos[i].meals_eaten >= monitor_data->philos[i].rules->num_must_eat)
				finished++;
			i++;
		}
		if (finished == monitor_data->num_of_philo)
		{
			pthread_mutex_lock(&monitor_data->shared->death_mutex);
			monitor_data->shared->someone_died = 1;
			pthread_mutex_unlock(&monitor_data->shared->death_mutex);
			return (NULL);
		}
		usleep(1000);
	}
}
