/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 11:18:00 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/06 13:39:15 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	init_rules(t_rules *rules, int argc, char **argv)
{
	int	i;

	i = 1;
	if (argc != 5 && argc != 6)
	{
		printf("ERROR : invalid arguments\n");
		return (1);
	}
	while (argv[i])
	{
		if (check_ascii(argv[i]) == 0)
			i++;
		else
		{
			printf("ERROR: initialization failed\n");
			return (1);
		}
	}
	rules->num_of_philo = ft_atoi(argv[1]);
	rules->time_to_die = ft_atoi(argv[2]);
	rules->time_to_eat = ft_atoi(argv[3]);
	rules->time_to_sleep = ft_atoi(argv[4]);
	if (argc == 6)
		rules->num_must_eat = ft_atoi(argv[5]);
	else
		rules->num_must_eat = -1;
	return (0);
}

int	main(int argc, char **argv)
{
	t_philo			*philo;
	t_rules			rules;
	t_shared		shared;
	t_fork			*fork;
	t_monitor_data	monitor_data;
	pthread_t		monitor;
	pthread_t		*threads;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	meal_mutex;
	int				i;
	int				*ids;

	i = 0;
	////////////////////parsing///////////////
	if (init_rules(&rules, argc, argv) == 1)
		return (1);
	ids = malloc(rules.num_of_philo * sizeof(int));
	///////////init mutexes//////////////////
	pthread_mutex_init(&print_mutex, NULL);
	pthread_mutex_init(&meal_mutex, NULL);
	///////shared init////////
	pthread_mutex_init(&shared.death_mutex, NULL);
	shared.someone_died = 0;
	shared.all_ate_enough = 0;
	shared.creation = 0;
	/////////init philo struct/////////////////
	philo = malloc(rules.num_of_philo * sizeof(t_philo));
	///////////init threads philos////////////////
	threads = malloc(rules.num_of_philo * sizeof(pthread_t));
	/////////////create forks//////////////////
	fork = malloc(rules.num_of_philo * sizeof(t_fork));
	fork->is_taken = 0;
	while (i < rules.num_of_philo)
	{
		pthread_mutex_init(&fork[i].mutex, NULL);
		i++;
	}
	shared.start_time = get_time_in_ms();
	////////////assign values to each philos/////////////////////
	i = 0;
	while (i < rules.num_of_philo)
	{
		ids[i] = i + 1;
		philo[i].id = ids[i];
		philo[i].meals_eaten = 0;
		philo[i].print_mutex = &print_mutex;
		philo[i].left_fork = &fork[i];
		philo[i].right_fork = &fork[(i + 1) % rules.num_of_philo];
		philo[i].rules = &rules;
		philo[i].last_meal_time = get_time_in_ms();
		philo[i].meal_mutex = &meal_mutex;
		philo[i].shared = &shared;
		i++;
	}
	/////////////start routine///////////////////////
	i = 0;
	while (i < rules.num_of_philo)
	{
		shared.creation = i;
		pthread_create(&threads[i], NULL, routine, &philo[i]);
		i++;
	}
	/////////create monitor//////////////////////
	monitor_data.num_of_philo = ft_atoi(argv[1]);
	monitor_data.time_to_die = ft_atoi(argv[2]);
	monitor_data.shared = &shared;
	monitor_data.philos = philo;
	pthread_create(&monitor, NULL, monitor_routine, &monitor_data);
	////////join threads/////////////////////////
	i = 0;
	while (i < rules.num_of_philo)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	pthread_mutex_destroy(&print_mutex);
	pthread_mutex_destroy(&meal_mutex);
	pthread_mutex_destroy(&shared.death_mutex);
	return (0);
}

// void	*routine(void *arg)
// {
// 	t_philo	*philo;

// 	philo = (t_philo *)arg;
// 	if (philo->data->num_of_times_each_philo_must_eat != -1)
// 		while (!philo->data->stop_simulation)
// 		{
// 			philo->data->start_time = get_time_in_ms();
// 			// Try to take forks
// 			if (philo->times_eaten == 0
// 				&& philo->data->start_time == philo->data->time_to_die)
// 			{
// 				print_action(philo, "died 💀");
// 				break ;
// 			}
// 			if (philo->last_meal_time == philo->data->time_to_die)
// 			{
// 				print_action(philo, "died 💀");
// 				break ;
// 			}
// 			if (take_fork(philo->left_fork) && take_fork(philo->right_fork))
// 			{
// 				print_action(philo, "is eating 🍝");
// 				usleep(philo->data->time_to_eat);
// 				philo->times_eaten += 1;
// 				philo->last_meal_time = get_time_in_ms();
// 				release_fork(philo->left_fork);
// 				release_fork(philo->right_fork);
// 				print_action(philo, "is sleeping 😴");
// 				usleep(philo->data->time_to_sleep);
// 			}
// 			else
// 			{
// 				// If you failed to take both forks, release any you took
// 				release_fork(philo->left_fork);
// 				release_fork(philo->right_fork);
// 				print_action(philo, "is thinking 🧠");
// 				usleep(philo->data->time_to_sleep);
// 					// wait a bit before retrying
// 			}
// 		}
// 	return (NULL);
// }

// int	init_data(t_data *data, int argc, char **argv)
// {
// 	int	i;

// 	data->nb_philo = ft_atoi(argv[1]);
// 	data->time_to_die = ft_atoi(argv[2]);
// 	data->time_to_eat = ft_atoi(argv[3]);
// 	data->time_to_sleep = ft_atoi(argv[4]);
// 	if (argc == 6)
// 		data->num_of_times_each_philo_must_eat = ft_atoi(argv[5]);
// 	else
// 		data->num_of_times_each_philo_must_eat = -1;
// 	data->philos = malloc(sizeof(t_philo) * data->nb_philo);
// 	data->forks = malloc(sizeof(t_fork) * data->nb_philo);
// 	if (!data->philos || !data->forks)
// 		return (ERROR);
// 	// Initialize mutexes
// 	pthread_mutex_init(&data->stop_mutex, NULL);
// 	pthread_mutex_init(&data->print_mutex, NULL);
// 	// Initialize forks
// 	if (init_forks(data->forks, data->nb_philo) == ERROR)
// 		return (ERROR);
// 	// Initialize philosophers and assign forks
// 	if (init_philosophers(data->philos, data->forks, data) == ERROR)
// 		return (ERROR);
// 	return (SUCCESS);
// }

// int	main(int argc, char **argv)
// {
// 	t_data	data;

// 	if (argc != 5 && argc != 6)
// 	{
// 		printf("ERROR : invalid arguments");
// 		return (ERROR);
// 	}
// 	if (init_data(&data, argc, argv) == ERROR) // wtf
// 	{
// 		printf("ERROR: initialization failed\n");
// 		return (ERROR);
// 	}
// 	if (handle_routine(&data) == ERROR)
// 	{
// 		printf("ERROR: routine handling failed\n");
// 		return (ERROR);
// 	}
// 	// Clean up and destroy mutexes
// 	free(data.philos);
// 	free(data.forks);
// 	pthread_mutex_destroy(&data.stop_mutex);
// 	pthread_mutex_destroy(&data.print_mutex);
// 	return (SUCCESS);
// }
