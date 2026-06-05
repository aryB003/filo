/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryan <aryan@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 22:42:00 by aryan             #+#    #+#             */
/*   Updated: 2026/06/05 16:10:48 by aryan            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** monitor_routine — runs in the monitor thread.
**
** Job: poll every philosopher until somebody dies or all are full.
**
** Loop until simulation ends:
**   For each philo[i] in 0..n_philo-1:
**     - lock philo[i].meal_lock; read time_last_ate and n_meals_eaten;
**       unlock meal_lock.
**     - if now_ms() - time_last_ate > time_to_die:
**           log_death(&philo[i]);  // prints "X died" and sets stop flag.
**           return NULL;
**   After all philos checked:
**     - if must_eat is set (!= (size_t)-1) AND every philo has
**       n_meals_eaten >= must_eat:
**           stop_simulation(t);
**           return NULL;
**     - else: usleep(500) and continue.
**
** Tips:
**   - Don't hold meal_lock while calling log_death (log_death takes other
**     locks). Read values under meal_lock, release, then act.
**   - Poll every ~500us so 10ms death-detection deadline is easy.
*/

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
