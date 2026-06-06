/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryan <aryan@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 19:47:51 by aryan             #+#    #+#             */
/*   Updated: 2026/05/27 14:26:59 by aryan            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

size_t	now_ms(void)
{
	struct timeval	tv;
	size_t			milli_s;

	if (gettimeofday (&tv, NULL) == 0)
	{
		milli_s = ((size_t)tv.tv_sec * 1000) + (tv.tv_usec / 1000);
		return (milli_s);
	}
	else
		write(2, "gettimeofday() error\n", 21);
	return (0);
}

bool	is_stopped(t_table *t)
{
	bool	result;

	pthread_mutex_lock(&t->stop_lock);
	result = t->stop;
	pthread_mutex_unlock(&t->stop_lock);
	return (result);
}

void	stop_simulation(t_table *t)
{
	pthread_mutex_lock(&t->stop_lock);
	t->stop = true;
	pthread_mutex_unlock(&t->stop_lock);
}

void	smart_sleep(size_t ms, t_table *t)
{
	size_t	start;

	start = now_ms();
	while (now_ms() - start < ms)
	{
		if (is_stopped(t))
			return ;
		usleep(500);
	}
}
