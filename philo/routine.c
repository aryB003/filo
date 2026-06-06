/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryan <aryan@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 22:42:00 by aryan             #+#    #+#             */
/*   Updated: 2026/06/05 23:19:11 by aryan            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** Forks are integers (0 = free, 1 = taken), each guarded by its own mutex.
** grab_one locks a fork's mutex, checks its integer; if free it claims it
** (sets it to 1) and returns. If taken, it unlocks and backs off 100us, then
** retries. Forks are taken one at a time in an asymmetric order (even ids
** left-first, odd ids right-first) to avoid a circular wait.
*/

static int	grab_one(t_philo *p, size_t fork_id)
{
	while (!is_stopped(p->table))
	{
		pthread_mutex_lock(&p->table->forks_mutexes[fork_id]);
		if (p->table->forks[fork_id] == 0)
		{
			p->table->forks[fork_id] = 1;
			pthread_mutex_unlock(&p->table->forks_mutexes[fork_id]);
			log_state(p, "has taken a fork");
			return (1);
		}
		pthread_mutex_unlock(&p->table->forks_mutexes[fork_id]);
		usleep(100);
	}
	return (0);
}

static int	take_forks(t_philo *p)
{
	if (p->id % 2 == 0)
	{
		if (!grab_one(p, p->left) || !grab_one(p, p->right))
			return (0);
	}
	else
	{
		if (!grab_one(p, p->right) || !grab_one(p, p->left))
			return (0);
	}
	return (1);
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
	t_table	*t;

	t = p->table;
	pthread_mutex_lock(&t->forks_mutexes[p->left]);
	t->forks[p->left] = 0;
	pthread_mutex_unlock(&t->forks_mutexes[p->left]);
	pthread_mutex_lock(&t->forks_mutexes[p->right]);
	t->forks[p->right] = 0;
	pthread_mutex_unlock(&t->forks_mutexes[p->right]);
}

void	*philosopher_routine(void *arg)
{
	t_philo	*p;

	p = (t_philo *)arg;
	if (p->table->n_philo == 1)
	{
		pthread_mutex_lock(&p->table->forks_mutexes[p->left]);
		log_state(p, "has taken a fork");
		smart_sleep(p->table->time_to_die, p->table);
		pthread_mutex_unlock(&p->table->forks_mutexes[p->left]);
		return (NULL);
	}
	while (!is_stopped(p->table) && take_forks(p))
	{
		eat(p);
		drop_forks(p);
		log_state(p, "is sleeping");
		smart_sleep(p->table->time_to_sleep, p->table);
		log_state(p, "is thinking");
	}
	return (NULL);
}
