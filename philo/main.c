/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryan <aryan@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 13:53:24 by aryan             #+#    #+#             */
/*   Updated: 2026/05/26 20:53:50 by aryan            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	print_usage(char *prog)
{
	printf("Error: invalid arguments\n");
	printf("Usage: %s n_philo time_to_die time_to_eat ", prog);
	printf("time_to_sleep [must_eat]\n");
}

static int	spawn_threads(t_table *t, pthread_t *monitor)
{
	size_t	i;

	i = 0;
	while (i < t->n_philo)
	{
		if (pthread_create(&t->philos[i].thread, NULL,
				philosopher_routine, &t->philos[i]) != 0)
			return (0);
		i++;
	}
	if (pthread_create(monitor, NULL, monitor_routine, t) != 0)
		return (0);
	return (1);
}

static void	join_threads(t_table *t, pthread_t monitor)
{
	size_t	i;

	i = 0;
	while (i < t->n_philo)
	{
		pthread_join(t->philos[i].thread, NULL);
		i++;
	}
	pthread_join(monitor, NULL);
}

int	main(int ac, char **av)
{
	t_table		table;
	pthread_t	monitor;

	if (!parse_args(ac, av, &table))
	{
		print_usage(av[0]);
		return (1);
	}
	if (!init_table(&table))
	{
		write(2, "init failed\n", 12);
		return (1);
	}
	if (!spawn_threads(&table, &monitor))
	{
		cleanup_table(&table);
		return (1);
	}
	join_threads(&table, monitor);
	cleanup_table(&table);
	return (0);
}
