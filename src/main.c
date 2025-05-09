/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 11:18:00 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/09 14:06:26 by vicperri         ###   ########lyon.fr   */
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
	pthread_mutex_init(&shared.urgency_mutex, NULL);
	shared.someone_died = 0;
	shared.all_ate_enough = 0;
	shared.start_time = 0;
	/////////init philo struct/////////////////
	philo = malloc(rules.num_of_philo * sizeof(t_philo));
	///////////init threads philos////////////////
	threads = malloc(rules.num_of_philo * sizeof(pthread_t));
	/////////////create forks//////////////////
	fork = malloc(rules.num_of_philo * sizeof(t_fork));
	while (i < rules.num_of_philo)
	{
		pthread_mutex_init(&fork[i].mutex, NULL);
		fork[i].is_taken = 0;
		i++;
	}
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
		philo[i].urgency = 0;
		philo[i].meal_mutex = &meal_mutex;
		philo[i].shared = &shared;
		philo[i].time_left = 0;
		philo[i].wait_time = 0;
		i++;
	}
	/////////////start routine///////////////////////
	i = 0;
	while (i < rules.num_of_philo)
	{
		pthread_create(&threads[i], NULL, routine, &philo[i]);
		i++;
	}
	shared.start_time = get_time_in_ms();
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

