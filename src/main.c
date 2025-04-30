/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 11:18:00 by vicperri          #+#    #+#             */
/*   Updated: 2025/04/30 11:43:11 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	int cycles = 10; //  time_to_die
	while (cycles > 0)
	{
		// Try to take forks
		if (take_fork(philo->left_fork) && take_fork(philo->right_fork))
		{
			print_action(philo, "is eating 🍝");
			usleep(philo->time_to_eat * 1000);
			release_fork(philo->left_fork);
			release_fork(philo->right_fork);
			print_action(philo, "is sleeping 😴");
			usleep(philo->time_to_sleep * 1000);
		}
		else
		{
			// If you failed to take both forks, release any you took
			release_fork(philo->left_fork);
			release_fork(philo->right_fork);
			print_action(philo, "is thinking 🧠");
			usleep(1000); // wait a bit before retrying
		}
		cycles--;
	} // wtf is -> [number_of_times_each_philosopher_must_eat]
	return (NULL);
}

int	main(int argc, char **argv)
{
	t_philo	*philo;
	int		size;

	size = 0;
	if (argc < 6)
	{
		printf("ERROR : missing argument");
		return (ERROR);
	}
	else
	{
		printf("ERROR : too much arguments");
		return (ERROR);
	}
	if (check_ascii(argv[1]) == SUCCESS)
		size = ft_atoi(argv[1]);
	philo = malloc(size + 1 * sizeof(char)); // Array of philosophers
	if (!philo)
		return (ERROR);
	if (argc == 6)
		parse_args(argv, philo);
	handle_routine(philo);
	printf("All the philo died. Bye.\n");
	return (SUCCESS);
}
