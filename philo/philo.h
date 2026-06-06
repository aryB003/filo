/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryan <aryan@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 13:53:19 by aryan             #+#    #+#             */
/*   Updated: 2026/06/06 00:02:14 by aryan            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H
# include <unistd.h>
# include <stdbool.h>
# include <stdlib.h>
# include <string.h>
# include <pthread.h>
# include <sys/time.h>
# include <stdio.h>

typedef struct s_philo	t_philo;
typedef struct s_table	t_table;

struct s_table
{
	size_t			n_philo;
	size_t			time_to_die;
	size_t			time_to_eat;
	size_t			time_to_sleep;
	size_t			must_eat;
	size_t			start_ms;
	bool			stop;
	t_philo			*philos;
	int				*forks;
	pthread_mutex_t	*forks_mutexes;
	pthread_mutex_t	stop_lock;
	pthread_mutex_t	print_lock;
};

struct s_philo
{
	size_t			id;
	size_t			time_last_ate;
	size_t			n_meals_eaten;
	size_t			left;
	size_t			right;
	pthread_t		thread;
	pthread_mutex_t	meal_lock;
	t_table			*table;
};

int		parse_args(int ac, char **av, t_table *table);
size_t	now_ms(void);
int		init_table(t_table *table);
void	cleanup_table(t_table *table);
void	log_state(t_philo *p, const char *msg);
void	log_death(t_philo *p);
bool	is_stopped(t_table *table);
void	stop_simulation(t_table *table);
void	smart_sleep(size_t ms, t_table *table);
void	*philosopher_routine(void *arg);
void	*monitor_routine(void *arg);

#endif
