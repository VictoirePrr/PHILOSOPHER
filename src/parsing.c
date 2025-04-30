/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 10:58:05 by vicperri          #+#    #+#             */
/*   Updated: 2025/04/30 11:32:17 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	check_ascii(char *argj)
{
	int	j;

	j = 0;
	while (argj[j])
	{
		if (!(argj[j] >= '0' && argj[j] <= '9'))
		{
			printf("ERROR : use numbers\n");
			return (ERROR);
		}
		j++;
	}
	return (SUCCESS);
}

int	parse_args(char **argv, t_philo *philo)
{
	int		i;
	char	*str;

	i = 1;
	while (argv[i])
	{
		if (check_ascii(argv[i]) == SUCCESS)
		{
			if (i == 1)
				philo->num = ft_atoi(argv[i]);
			if (i == 2)
				philo->time_to_die = ft_atoi(argv[i]);
			if (i == 3)
				philo->time_to_eat = ft_atoi(argv[i]);
			if (i == 4)
				philo->time_to_sleep = ft_atoi(argv[i]);
			if (i == 5)
			{
				str = ft_strtrim(argv[i], "[");
				philo->times_eaten = ft_atoi(ft_strtrim(str, "]"));
			}
		}
		else
			return (ERROR);
		i++;
	}
	return (SUCCESS);
}
