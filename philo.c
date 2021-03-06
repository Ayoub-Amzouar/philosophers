/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aamzouar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/14 11:35:56 by aamzouar          #+#    #+#             */
/*   Updated: 2021/06/14 11:35:57 by aamzouar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long int	get_time_in_millis(void)
{
	struct timeval	time; 

	if (gettimeofday(&time, NULL))
		return (printf("Error: gettimeofday didn't work\n") * 0 - 1);
	return (time.tv_sec * 1000);
}

int		valid_number(char *str)
{
	int		i;

	i = 0;
	if (str[0] == 48)
		return (1);
	while (str[i] != '\0')
	{
		if (str[i] < 48 || str[i] > 57)
			return (1);
		i++;
	}
	return (0);
}

int		check_arg_errors(int ac, char *av[])
{
	int		i;

	if (ac < 5 || ac > 6)
		return (printf("Error: wrong number of arguments\n") * 0 + 1);
	i = 1;
	while (i < ac)
	{
		if (valid_number(av[i]))
			return (printf("Error: argument [%d] isn't valid\n", i) * 0 + 1);
		i++;
	}
	return (0);
}

void	parse_the_data(int ac, char *av[])
{
	g_philo_attr.nb_of_philo = ft_atoi(av[1]);
	g_philo_attr.time_to_die = ft_atoi(av[2]);
	g_philo_attr.time_to_eat = ft_atoi(av[3]);
	g_philo_attr.time_to_sleep = ft_atoi(av[4]);
	if (ac == 6)
		g_philo_attr.times_to_eat = ft_atoi(av[5]);
	else
		g_philo_attr.times_to_eat = 0;
}

void	print_state(char *state, int id, char *color)
{
	long int	time;

	time = get_time_in_millis();
	printf("%ld %d %s%s\033[0m\n", time, id, color, state);
}

void mysleep(long long arg)
{
	
	struct timeval t;
	long long		d;
	long long		microb;

	gettimeofday(&t, NULL);
	long long k = (t.tv_usec + (t.tv_sec * 1000000));
	microb = k;
	d = arg - 50;
	usleep(d * 1000);
	while ((k - microb) < (arg * 1000))
	{	
		gettimeofday(&t, NULL);
		k = (t.tv_usec + (t.tv_sec * 1000000));
	}
	
}

void	take_forks_and_eat(int id)
{
	int		left;
	int		right;

	// left = id - 1;
	// right = (left + 1) % g_philo_attr.nb_of_philo;
	pthread_mutex_lock(&g_philo_attr.mutex[id - 1]);
	print_state("has taken a fork 1\n", id, BLUE);
	pthread_mutex_lock(&g_philo_attr.mutex[id % g_philo_attr.nb_of_philo]);
	print_state("has taken a fork 2\n", id, BLUE);
	print_state("is eating\n", id, YELLOW);
	mysleep(g_philo_attr.time_to_eat);
	pthread_mutex_unlock(&g_philo_attr.mutex[id - 1]);
	pthread_mutex_unlock(&g_philo_attr.mutex[id % g_philo_attr.nb_of_philo]);
	mysleep(100);
}


void	restricted_loop(int id)
{
	int		i;

	i = 0;
	while (i < g_philo_attr.times_to_eat)
	{
		take_forks_and_eat(id);
		i++;
	}
}

void	*philosopher(void *arg)
{
	int		id;
	
	id = *(int *)arg;
	if (g_philo_attr.times_to_eat)
		restricted_loop(id);
	/*else
		infinite_loop();*/
	free(arg);
	return NULL;
}

int		alloc_init_mutex(void)
{
	int		i;

	i = 0;
	g_philo_attr.mutex = malloc(sizeof(pthread_mutex_t) * g_philo_attr.nb_of_philo);
	while (i < g_philo_attr.nb_of_philo)
	{
		if (pthread_mutex_init(&g_philo_attr.mutex[i], NULL) == -1)
			return (1);
		i++;
	}
	return (0);
}

int		destroy_mutex(void)
{
	int		i;

	i = 0;
	while (i < g_philo_attr.nb_of_philo)
	{
		if (pthread_mutex_destroy(&g_philo_attr.mutex[i]))
			return (1);
		i++;
	}
	return (0);
}

int		create_philosophers(void)
{
	int			i;
	pthread_t	th[g_philo_attr.nb_of_philo];
	int			*nb;

	i = 0;
	if (alloc_init_mutex())
		return (printf("Error: Can't initialize mutex\n") * 0 + 1);
	while (i < g_philo_attr.nb_of_philo)
	{
		nb = malloc(sizeof(int));
		*nb = i + 1;
		if (pthread_create(&th[i], NULL, philosopher, nb))
			return (printf("Error: can't create the thread [%d]\n", i) * 0 + 1);
		usleep(100);
		i++;
	}
	i = 0;
	while (i < g_philo_attr.nb_of_philo)
	{
		if (pthread_join(th[i], NULL))
			return (printf("Error: can't join the thread [%d]\n", i) * 0 + 1);
		i++;
	}
	if (destroy_mutex())
		return (printf("Error: can't destroy mutex\n") * 0 + 1);
	return (0);
}

int     main(int ac, char *av[])
{
	if (check_arg_errors(ac, av))
		return (1);
	parse_the_data(ac, av);
	if (create_philosophers())
		return (1);
	return (0);
}