/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 10:58:58 by vicperri          #+#    #+#             */
/*   Updated: 2025/04/30 14:11:11 by vicperri         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_strlen(const char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
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
		if (((nb * 10) + (nptr[i] - '0')) > LONG_LONG_INT && sign == 1)
			return (-1);
		else if (((nb * 10) + (nptr[i] - '0')) > LONG_LONG_INT && sign == -1)
			return (0);
		nb = nb * 10 + (nptr[i++] - '0');
	}
	return (nb * sign);
}
char	*ft_strchr(const char *s, int c)
{
	char			*s1;
	unsigned char	c1;
	int				i;

	s1 = (char *)s;
	c1 = (unsigned char)c;
	i = 0;
	while (s1[i])
	{
		if (s1[i] == c1)
			return (&s1[i]);
		i++;
	}
	if (s1[i] == '\0' && c1 == '\0')
		return (&s1[i]);
	return (NULL);
}
char	*ft_substr(char const *s, int start, int len)
{
	char	*str;
	int	i;

	i = 0;
	if (!s)
		return (NULL);
	if (start >= ft_strlen(s))
		return (ft_strdup(""));
	if (len + start > ft_strlen(s))
		len = ft_strlen(s) - start;
	str = (char *)malloc(len + 1 * sizeof(char));
	if (!(str))
		return (NULL);
	while (s[start] && len--)
		str[i++] = s[start++];
	str[i] = '\0';
	return (str);
}

char	*ft_strtrim(const char *s1, const char *set)
{
	char	*str;
	size_t	start;
	size_t	end;

	start = 0;
	if (!(s1 || set))
		return (NULL);
	while (s1[start] && (ft_strchr(set, s1[start])))
		start++;
	if (s1[start] == '\0')
		return (ft_strdup(""));
	end = ft_strlen(s1) - 1;
	while (end > start && ft_strchr(set, s1[end]))
		end--;
	str = ft_substr(s1, start, end - start + 1);
	return (str);
}

char	*ft_strdup(const char *s1)
{
	size_t	size;
	char	*dup;
	size_t	i;

	size = ft_strlen(s1) + 1;
	dup = (char *)malloc(size * sizeof(char));
	i = 0;
	if (!(dup))
		return (0);
	while (s1[i])
	{
		dup[i] = s1[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

int	take_fork(t_fork *fork)
{
	int	true;

	true = 0;
	pthread_mutex_lock(&fork->mutex);
	if (fork->is_taken == 0)
	{
		fork->is_taken = 1;
		true = 1;
	}
	pthread_mutex_unlock(&fork->mutex);
	return (true);
}

void	release_fork(t_fork *fork)
{
	pthread_mutex_lock(&fork->mutex);
	fork->is_taken = 0;
	pthread_mutex_unlock(&fork->mutex);
}

void	print_action(t_philo *philo, char *str)
{
	pthread_mutex_lock(philo->print_mutex);
	printf("%d %s\n", philo->id, str);
	pthread_mutex_unlock(philo->print_mutex);
}
