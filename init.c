/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryan <aryan@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 22:42:00 by aryan             #+#    #+#             */
/*   Updated: 2026/05/27 16:13:23 by aryan            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	init_forks(t_table *t)
{
	size_t	i;

	i = 0;
	while (i < t->n_philo)
	{
		if (pthread_mutex_init(&t->forks[i], NULL) != 0)
			return (0);
		i++;
	}
	return (1);
}

static int	init_philos(t_table *t)
{
	size_t	i;

	i = 0;
	while (i < t->n_philo)
	{
		t->philos[i].id = i + 1;
		t->philos[i].time_last_ate = t->start_ms;
		t->philos[i].n_meals_eaten = 0;
		t->philos[i].r_fork = &t->forks[i];
		t->philos[i].l_fork = &t->forks[(i + 1) % t->n_philo];
		t->philos[i].table = t;
		if (pthread_mutex_init(&t->philos[i].meal_lock, NULL) != 0)
			return (0);
		i++;
	}
	return (1);
}

int	init_table(t_table *t)
{
	t->forks = malloc(sizeof(pthread_mutex_t) * t->n_philo);
	t->philos = malloc(sizeof(t_philo) * t->n_philo);
	if (!t->forks || !t->philos)
	{
		free(t->forks);
		free(t->philos);
		return (0);
	}
	t->start_ms = now_ms();
	t->stop = false;
	pthread_mutex_init(&t->print_lock, NULL);
	pthread_mutex_init(&t->stop_lock, NULL);
	if (!init_forks(t) || !init_philos(t))
		return (0);
	return (1);
}

void	cleanup_table(t_table *t)
{
	size_t	i;

	i = 0;
	while (i < t->n_philo)
	{
		pthread_mutex_destroy(&t->philos[i].meal_lock);
		pthread_mutex_destroy(&t->forks[i]);
		i++;
	}
	pthread_mutex_destroy(&t->print_lock);
	pthread_mutex_destroy(&t->stop_lock);
	free(t->philos);
	free(t->forks);
}
