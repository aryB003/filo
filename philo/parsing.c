/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryan <aryan@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 20:38:00 by aryan             #+#    #+#             */
/*   Updated: 2026/05/26 20:38:01 by aryan            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	is_digit_str(const char *s)
{
	int	i;

	i = 0;
	if (!s || !s[0])
		return (0);
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static int	ft_atoi_safe(const char *s, size_t *out)
{
	long long	result;
	int			i;

	if (!is_digit_str(s))
		return (0);
	result = 0;
	i = 0;
	while (s[i])
	{
		result = result * 10 + (s[i] - '0');
		if (result > 2147483647)
			return (0);
		i++;
	}
	*out = (size_t)result;
	return (1);
}

static int	parse_required(char **av, t_table *t)
{
	size_t	value;

	if (!ft_atoi_safe(av[1], &value) || value == 0)
		return (0);
	t->n_philo = value;
	if (!ft_atoi_safe(av[2], &value) || value == 0)
		return (0);
	t->time_to_die = value;
	if (!ft_atoi_safe(av[3], &value) || value == 0)
		return (0);
	t->time_to_eat = value;
	if (!ft_atoi_safe(av[4], &value) || value == 0)
		return (0);
	t->time_to_sleep = value;
	return (1);
}

int	parse_args(int ac, char **av, t_table *t)
{
	size_t	value;

	if (ac != 5 && ac != 6)
		return (0);
	if (!parse_required(av, t))
		return (0);
	t->must_eat = (size_t)-1;
	if (ac == 6)
	{
		if (!ft_atoi_safe(av[5], &value) || value == 0)
			return (0);
		t->must_eat = value;
	}
	return (1);
}
