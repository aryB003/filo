/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryan <aryan@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 22:42:00 by aryan             #+#    #+#             */
/*   Updated: 2026/06/05 16:47:36 by aryan            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** philosopher_routine — runs in each philosopher thread.
**
** Flow (in a while loop until is_stopped):
**   1. Take forks   lock r_fork and l_fork (ORDER MATTERS).
**                   log "has taken a fork" after each successful lock.
**   2. Eat          lock meal_lock; set time_last_ate = now_ms();
**                   n_meals_eaten++; unlock meal_lock;
**                   log "is eating"; smart_sleep(time_to_eat).
**   3. Drop forks   unlock both forks.
**   4. Sleep        log "is sleeping"; smart_sleep(time_to_sleep).
**   5. Think        log "is thinking" (optional brief smart_sleep).
**
** Special cases:
**  - n_philo == 1: only one fork exists. Locking it twice deadlocks.
**    Handle before the loop: take the fork, log it, smart_sleep(time_to_die),
**    return. Monitor will record the death.
**  - Stagger start: at the top, even-id philosophers usleep(~1000us) so
**    everyone doesn't grab their first fork at the same instant.
**
** Deadlock prevention (KEY decision):
**   Use asymmetric pickup order. Two common ways:
**     A) odd-id locks r_fork first; even-id locks l_fork first.
**     B) always lock the lower-addressed mutex first.
**   Pick one. Without this, large-N tests deadlock.
**
** Available helpers:
**   - log_state(p, msg)            mutex-protected printf with stop-check.
**   - smart_sleep(ms, p->table)    sleeps but wakes early if simulation stops.
**   - is_stopped(p->table)         true if simulation has ended.
**   - p->table->time_to_die etc.   the parsed timings.
*/

static void	take_forks(t_philo *p)
{
	if (p->id % 2 == 0)
	{
		pthread_mutex_lock(p->l_fork);
		log_state(p, "has taken a fork");
		pthread_mutex_lock(p->r_fork);
		log_state(p, "has taken a fork");
	}
	else
	{
		pthread_mutex_lock(p->r_fork);
		log_state(p, "has taken a fork");
		pthread_mutex_lock(p->l_fork);
		log_state(p, "has taken a fork");
	}
}

static void	eat(t_philo *p)
{
	pthread_mutex_lock(&p->meal_lock);
	p->time_last_ate = now_ms();
	p->n_meals_eaten = p->n_meals_eaten + 1;
	pthread_mutex_unlock(&p->meal_lock);
	log_state(p, "is eating");
	smart_sleep(p->table->time_to_eat, p->table);
}

static void	drop_forks(t_philo *p)
{
	pthread_mutex_unlock(p->l_fork);
	pthread_mutex_unlock(p->r_fork);
}

void	*philosopher_routine(void *arg)
{
	t_philo	*p;

	p = (t_philo *)arg;
	if (p->table->n_philo == 1)
	{
		pthread_mutex_lock(p->l_fork);
		log_state(p, "has taken a fork");
		smart_sleep(p->table->time_to_die, p->table);
		pthread_mutex_unlock(p->l_fork);
		return (NULL);
	}
	if (p->id % 2 == 0)
		usleep(1000);
	while (!is_stopped(p->table))
	{
		take_forks(p);
		eat(p);
		drop_forks(p);
		log_state(p, "is sleeping");
		smart_sleep(p->table->time_to_sleep, p->table);
		log_state(p, "is thinking");
	}
	return (NULL);
}
