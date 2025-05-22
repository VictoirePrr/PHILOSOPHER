/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 14:56:57 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/22 15:04:11 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	destroy_mutexes(t_data *data)
{
	pthread_mutex_destroy(&data->print_mutex);
	pthread_mutex_destroy(&data->meal_mutex);
	pthread_mutex_destroy(&data->shared.death_mutex);
}

void	destroy_forks(t_data *data, int num)
{
	int	i;

	i = 0;
	if (num == -1)
		num = data->rules.num_of_philo;
	while (i < num)
	{
		pthread_mutex_destroy(&data->forks[i].mutex);
		i++;
	}
	free(data->forks);
}

int	check_creation(t_data *data)
{
	t_monitor_data	*monitor_data;
	int				thread_status;

	thread_status = -1;
	monitor_data = malloc(sizeof(t_monitor_data));
	if (!monitor_data)
	{
		printf("ERROR : monitor creation failed\n");
		join_and_cleanup(data, -2, monitor_data);
		return (1);
	}
	thread_status = start_philo(data, monitor_data);
	if (thread_status != 0)
	{
		printf("ERROR : thread creation failed\n");
		pthread_mutex_lock(&data->shared.death_mutex);
		data->shared.end = 1;
		pthread_mutex_unlock(&data->shared.death_mutex);
		join_and_cleanup(data, thread_status, monitor_data);
		return (1);
	}
	join_and_cleanup(data, -1, monitor_data);
	return (0);
}

long	ft_atoi(const char *nptr)
{
	size_t			i;
	int				sign;
	unsigned long	nb;

	i = 0;
	sign = 1;
	nb = 0;
	while ((nptr[i] >= 9 && nptr[i] <= 13) || nptr[i] == 32)
		i++;
	if (nptr[i] == '-' || nptr[i] == '+')
		if (nptr[i++] == '-')
			sign *= -1;
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		if (((nb * 10) + (nptr[i] - '0')) > INT_MAX && sign == 1)
			return (-1);
		nb = nb * 10 + (nptr[i++] - '0');
	}
	return (nb * sign);
}

int	check_ascii(char *argj)
{
	int	j;

	j = 0;
	while (argj[j])
	{
		if (!(argj[j] >= '0' && argj[j] <= '9'))
		{
			printf("ERROR : use numbers\n");
			return (1);
		}
		j++;
	}
	return (0);
}
