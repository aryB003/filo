/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryan <aryan@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 22:42:00 by aryan             #+#    #+#             */
/*   Updated: 2026/06/08 15:01:57 by aryan            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	monitor_philo(t_table *t, size_t i, size_t *last_ate, size_t *meals)
{
	pthread_mutex_lock(&t->philos[i].meal_lock);
	*last_ate = t->philos[i].time_last_ate;
	*meals = t->philos[i].n_meals_eaten;
	pthread_mutex_unlock(&t->philos[i].meal_lock);
}

static int	check_philos(t_table *t)
{
	size_t	last_ate;
	size_t	meals;
	size_t	i;
	int		all_full;

	i = 0;
	all_full = 1;
	while (i < t->n_philo)
	{
		monitor_philo(t, i, &last_ate, &meals);
		if (now_ms() - last_ate > t->time_to_die)
		{
			log_death(&t->philos[i]);
			return (1);
		}
		if (t->must_eat != (size_t) - 1 && meals < t->must_eat)
			all_full = 0;
		i++;
	}
	if (t->must_eat != (size_t) - 1 && all_full)
	{
		stop_simulation(t);
		return (1);
	}
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_table	*t;

	t = (t_table *)arg;
	while (!is_stopped(t))
	{
		if (check_philos(t))
			return (NULL);
		usleep(500);
	}
	return (NULL);
}
