/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 10:59:52 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/07 17:23:25 by vicperri         ###   ########lyon.fr   */
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

int	take_fork(t_fork *fork, t_philo *philo)
{
	if (philo->urgency == 0 || philo->shared->vip == 0)
		my_usleep(100);
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
	printf(YELLOW ">>> Fork released!\n" RESET);
}

int	eat(t_philo *philo, t_fork *first, t_fork *second)
{
	// 2. Eat
	if (take_fork(first, philo) == 1)
	{
		pthread_mutex_lock(philo->print_mutex);
		printf(LIGHT_PINK "[%ld] %d has taken fork 1." RESET "\n",
			timestamp(philo->shared), philo->id);
		pthread_mutex_unlock(philo->print_mutex);
		if (check_if_dead(philo) == 1)
		{
			release_fork(first);
			return (1);
		}
		if (take_fork(second, philo) == 1)
		{
			pthread_mutex_lock(philo->print_mutex);
			printf(LIGHT_PINK "[%ld] %d has taken a fork 2." RESET "\n",
				timestamp(philo->shared), philo->id);
			pthread_mutex_unlock(philo->print_mutex);
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
			printf(HOT_PINK "[%ld] %d is eating." RESET "\n",
				timestamp(philo->shared), philo->id);
			pthread_mutex_unlock(philo->print_mutex);
			pthread_mutex_lock(philo->meal_mutex);
			philo->meals_eaten++;
			pthread_mutex_unlock(philo->meal_mutex);
			my_usleep(philo->rules->time_to_eat * 1000);
			// 3. Drop forks
			release_fork(second);
			release_fork(first);
		}
		else
		{
			pthread_mutex_lock(philo->print_mutex);
			printf(LIGHT_PINK "[%ld] %d couldn't take fork 2." RESET "\n",
				timestamp(philo->shared), philo->id);
			pthread_mutex_unlock(philo->print_mutex);
			release_fork(first); // if the first is good but not the second
			return (1);          // they should retry immediatly
		}
	}
	else
	{
		pthread_mutex_lock(philo->print_mutex);
		printf(LIGHT_PINK "[%ld] %d couldn't take fork 1." RESET "\n",
			timestamp(philo->shared), philo->id);
		pthread_mutex_unlock(philo->print_mutex);
		return (1); // they should retry immediatly
	}
	printf(YELLOW "[%ld] %d finished eating." RESET "\n",
		timestamp(philo->shared), philo->id);
	return (0);
}

void	think(t_philo *philo)
{
	// 4. Think
	pthread_mutex_lock(philo->print_mutex);
	printf(GREEN "[%ld] %d is thinking." RESET "\n", timestamp(philo->shared),
		philo->id);
	pthread_mutex_unlock(philo->print_mutex);
	my_usleep(200);
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
	my_usleep(philo->rules->time_to_sleep * 1000);
}
void	*routine(void *args)
{
	t_philo	*philo;
	t_fork	*first;
	t_fork	*second;

	philo = (t_philo *)args;
	while (philo->shared->start_time == 0)
		my_usleep(50);
	pthread_mutex_lock(philo->meal_mutex);
	philo->last_meal_time = get_time_in_ms();
	pthread_mutex_unlock(philo->meal_mutex);
	// 1. Define which forks
	if (philo->id % 2 == 0)
	{
		first = philo->left_fork;
		second = philo->right_fork;
		my_usleep(100);
	}
	else
	{
		first = philo->right_fork;
		second = philo->left_fork;
	}
	while (check_if_dead(philo) != 1)
	{
		if (philo->urgency == 1)
		{
			while (philo->urgency == 1 && check_if_dead(philo) != 1)
			{
				if (eat(philo, first, second) == 0)
					break ; // Successfully ate, exit urgent loop
				my_usleep(100);
			}
		}
		else
		{
			think(philo);
			if (eat(philo, first, second) == 0)
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
			if (time_since_meal >= monitor_data->time_to_die / 2)
			{
				pthread_mutex_lock(monitor_data->philos[i].print_mutex);
				printf(RED "[%ld] %d almost dead wesh." RESET "\n",
					timestamp(monitor_data->shared),
					monitor_data->philos[i].id);
				pthread_mutex_unlock(monitor_data->philos[i].print_mutex);
				pthread_mutex_lock(&monitor_data->shared->urgency_mutex);
				monitor_data->shared->vip = 1;
				monitor_data->philos[i].urgency = 1;
				pthread_mutex_unlock(&monitor_data->shared->urgency_mutex);
			}
			else
			{
				pthread_mutex_lock(&monitor_data->shared->urgency_mutex);
				monitor_data->shared->vip = 0;
				monitor_data->philos[i].urgency = 0;
				pthread_mutex_unlock(&monitor_data->shared->urgency_mutex);
			}
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
		my_usleep(500);
	}
}
