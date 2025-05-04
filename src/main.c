/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 11:18:00 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/01 13:59:35 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long long	get_time_in_ms(void)
{
	struct timeval	current_time;

	gettimeofday(&current_time, NULL);
	return (current_time.tv_sec * 1000 + current_time.tv_usec / 1000);
	// wtf is going on
}
void	*routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	if (philo->data->num_of_times_each_philo_must_eat != -1)
		while (!philo->data->stop_simulation)
		{
			philo->data->start_time = get_time_in_ms();
			// Try to take forks
			if (philo->times_eaten == 0
				&& philo->data->start_time == philo->data->time_to_die)
			{
				print_action(philo, "died 💀");
				break ;
			} 
			if (philo->last_meal_time == philo->data->time_to_die)
			{
				print_action(philo, "died 💀");
				break ;
			} 
			if (take_fork(philo->left_fork) && take_fork(philo->right_fork))
			{
				print_action(philo, "is eating 🍝");
				usleep(philo->data->time_to_eat);
				philo->times_eaten += 1;
				philo->last_meal_time = get_time_in_ms();
				release_fork(philo->left_fork);
				release_fork(philo->right_fork);
				print_action(philo, "is sleeping 😴");
				usleep(philo->data->time_to_sleep);
			}
			else
			{
				// If you failed to take both forks, release any you took
				release_fork(philo->left_fork);
				release_fork(philo->right_fork);
				print_action(philo, "is thinking 🧠");
				usleep(philo->data->time_to_sleep);
					// wait a bit before retrying
			}
		}
	return (NULL);
}

int	init_data(t_data *data, int argc, char **argv)
{
	int	i;

	data->nb_philo = ft_atoi(argv[1]);
	data->time_to_die = ft_atoi(argv[2]);
	data->time_to_eat = ft_atoi(argv[3]);
	data->time_to_sleep = ft_atoi(argv[4]);
	if (argc == 6)
		data->num_of_times_each_philo_must_eat = ft_atoi(argv[5]);
	else
		data->num_of_times_each_philo_must_eat = -1;
	data->philos = malloc(sizeof(t_philo) * data->nb_philo);
	data->forks = malloc(sizeof(t_fork) * data->nb_philo);
	if (!data->philos || !data->forks)
		return (ERROR);
	// Initialize mutexes
	pthread_mutex_init(&data->stop_mutex, NULL);
	pthread_mutex_init(&data->print_mutex, NULL);
	// Initialize forks
	if (init_forks(data->forks, data->nb_philo) == ERROR)
		return (ERROR);
	// Initialize philosophers and assign forks
	if (init_philosophers(data->philos, data->forks, data) == ERROR)
		return (ERROR);
	return (SUCCESS);
}

int	main(int argc, char **argv)
{
	t_data	data;

	if (argc != 5 && argc != 6)
	{
		printf("ERROR : invalid arguments");
		return (ERROR);
	}
	if (init_data(&data, argc, argv) == ERROR) // wtf
	{
		printf("ERROR: initialization failed\n");
		return (ERROR);
	}
	if (handle_routine(&data) == ERROR)
	{
		printf("ERROR: routine handling failed\n");
		return (ERROR);
	}
	// Clean up and destroy mutexes
	free(data.philos);
	free(data.forks);
	pthread_mutex_destroy(&data.stop_mutex);
	pthread_mutex_destroy(&data.print_mutex);
	return (SUCCESS);
}
