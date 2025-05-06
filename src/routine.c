/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 10:59:52 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/06 15:56:02 by vicperri         ###   ########lyon.fr   */
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
		return (fork->is_taken);
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

int	eat(t_philo *philo, t_fork *first, t_fork *second)
{
	// 2. Eat
	if (take_fork(first) == 1)
	{
		pthread_mutex_lock(philo->print_mutex);
		printf(LIGHT_PINK "[%ld] %d has taken a fork." RESET "\n",
			timestamp(philo->shared), philo->id);
		pthread_mutex_unlock(philo->print_mutex);
		if (check_if_dead(philo) == 1)
		{
			release_fork(first);
			return (1);
		}
		if (take_fork(second) == 1)
		{
			if (check_if_dead(philo) == 1)
			{
				release_fork(second);
				release_fork(first);
				return (1);
			}
			pthread_mutex_lock(philo->meal_mutex);
			philo->last_meal_time = get_time_in_ms();
			pthread_mutex_unlock(philo->meal_mutex);
			pthread_mutex_lock(philo->print_mutex);
			printf(HOT_PINK"[%ld] %d is eating." RESET "\n",
				timestamp(philo->shared), philo->id);
			philo->meals_eaten++;
			pthread_mutex_unlock(philo->print_mutex);
			usleep(philo->rules->time_to_eat * 1000);
			// 3. Drop forks
			release_fork(second);
			release_fork(first);
		}
		else
		{
			release_fork(first); // if the first is good but not the second
			usleep(100);
			return (1);
		}
	}
	else
	{
		usleep(100); // If fork not taken, avoid CPU spin
		return (1);
	}
	return (0);
}

void	think(t_philo *philo)
{
	// 4. Think
	pthread_mutex_lock(philo->print_mutex);
	printf(GREEN "[%ld] %d is thinking." RESET "\n", timestamp(philo->shared),
		philo->id);
	pthread_mutex_unlock(philo->print_mutex);
}

void	p_sleep(t_philo *philo)
{
	// 5. Sleep
	if (check_if_dead(philo) == 1)
		return ;
	pthread_mutex_lock(philo->print_mutex);
	printf(BLUE "[%ld] %d is sleeping." RESET "\n", timestamp(philo->shared),
		philo->id);
	pthread_mutex_unlock(philo->print_mutex);
	usleep(philo->rules->time_to_sleep * 1000);
}
void	*routine(void *args)
{
	t_philo	*philo;
	t_fork	*first;
	t_fork	*second;

	philo = (t_philo *)args;
	while (philo->shared->creation < philo->rules->num_of_philo - 1)
	{
		usleep(1); // always one philo eating before everyone
	}
	// 1. Define which forks
	pthread_mutex_lock(philo->meal_mutex);
	philo->shared->start_time = get_time_in_ms();
	printf("Philo %d start time init at %ld\n", philo->id,
		philo->shared->start_time);
	pthread_mutex_unlock(philo->meal_mutex);
	if (philo->id % 2 == 0)
	{
		first = philo->left_fork;
		second = philo->right_fork;
	}
	else
	{
		first = philo->right_fork;
		second = philo->left_fork;
		usleep(100); //--> to eat here ?
	}
	while (check_if_dead(philo) != 1)
	{
		if (eat(philo, first, second) == 0)
		{
			think(philo);
			p_sleep(philo);
		}
	}
	return (NULL);
}

void	*monitor_routine(void *args)
{
	t_monitor_data	*monitor_data;
	int				i;
	int				finished;
	long			time_since_meal;
	int				meals;

	monitor_data = (t_monitor_data *)args;
	while (1)
	{
		i = 0;
		finished = 0;
		pthread_mutex_lock(&monitor_data->shared->death_mutex);
		// printf("someone died : %d\n", monitor_data->shared->someone_died);
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
			// printf("time since meal : %ld\n", time_since_meal);
			pthread_mutex_unlock(monitor_data->philos[i].meal_mutex);
			if (time_since_meal > monitor_data->time_to_die)
			{
				pthread_mutex_lock(monitor_data->philos[i].print_mutex);
				printf(RED "[%ld] %d died." RESET "\n",
					timestamp(monitor_data->shared),
					monitor_data->philos[i].id);
				pthread_mutex_unlock(monitor_data->philos[i].print_mutex);
				pthread_mutex_lock(&monitor_data->shared->death_mutex);
				monitor_data->shared->someone_died = 1;
				pthread_mutex_unlock(&monitor_data->shared->death_mutex);
				return (NULL);
			}
			if (monitor_data->philos[i].rules->num_must_eat > 0)
			{
				pthread_mutex_lock(monitor_data->philos[i].meal_mutex);
				meals = monitor_data->philos[i].meals_eaten;
				pthread_mutex_unlock(monitor_data->philos[i].meal_mutex);
				if (meals >= monitor_data->philos[i].rules->num_must_eat)
					finished++;
			}
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
