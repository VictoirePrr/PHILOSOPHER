/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 14:56:57 by vicperri          #+#    #+#             */
/*   Updated: 2025/05/15 13:43:23 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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
		if (((nb * 10) + (nptr[i] - '0')) > LONG_LONG_INT && sign == 1)
			return (-1);
		else if (((nb * 10) + (nptr[i] - '0')) > LONG_LONG_INT && sign == -1)
			return (0);
		nb = nb * 10 + (nptr[i++] - '0');
	}
	return (nb * sign);
}

long long	get_time_in_ms(void)
{
	struct timeval	current_time;

	gettimeofday(&current_time, NULL);
	return (current_time.tv_sec * 1000 + current_time.tv_usec / 1000);
}

long	timestamp(t_shared *shared)
{
	return (get_time_in_ms() - shared->start_time);
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

void	my_usleep(long duration, t_philo *philo)
{
	long	start_time;

	start_time = get_time_in_ms();
	while ((get_time_in_ms() - start_time) < duration)
	{
		if (check_if_dead(philo))
			break ;
		usleep(400);
	}
}
