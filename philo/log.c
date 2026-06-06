/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryan <aryan@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 22:42:00 by aryan             #+#    #+#             */
/*   Updated: 2026/05/27 14:16:03 by aryan            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	log_state(t_philo *p, const char *msg)
{
	size_t	ts;

	pthread_mutex_lock(&p->table->print_lock);
	if (!is_stopped(p->table))
	{
		ts = now_ms() - p->table->start_ms;
		printf("%zu %zu %s\n", ts, p->id, msg);
	}
	pthread_mutex_unlock(&p->table->print_lock);
}

void	log_death(t_philo *p)
{
	size_t	ts;

	pthread_mutex_lock(&p->table->print_lock);
	pthread_mutex_lock(&p->table->stop_lock);
	if (!p->table->stop)
	{
		ts = now_ms() - p->table->start_ms;
		printf("%zu %zu died\n", ts, p->id);
		p->table->stop = true;
	}
	pthread_mutex_unlock(&p->table->stop_lock);
	pthread_mutex_unlock(&p->table->print_lock);
}
