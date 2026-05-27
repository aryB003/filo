/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryan <aryan@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 22:42:00 by aryan             #+#    #+#             */
/*   Updated: 2026/05/26 22:42:00 by aryan            ###   ########.fr       */
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

void	*monitor_routine(void *arg)
{
	t_table	*t;

	t = (t_table *)arg;
	(void)t;
	return (NULL);
}
