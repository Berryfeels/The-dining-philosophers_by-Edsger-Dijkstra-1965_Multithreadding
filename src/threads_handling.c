/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads_handling.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stdi-pum <stdi-pum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 15:24:35 by stdi-pum          #+#    #+#             */
/*   Updated: 2024/12/15 20:16:26 by stdi-pum         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

int	ft_join_threads(pthread_t *philo, t_args *philo_data, pthread_t death)
{
	int	i;

	i = 0;
	while (i < philo_data->shared->n_philo)
	{
		if (pthread_join(philo[i], NULL) != 0)
			ft_exit_error(EXIT_ERROR_THREAD);
		i++;
	}
	if (pthread_join(death, NULL) != 0)
	{
		free(philo);
		ft_exit_error(EXIT_ERROR_THREAD);
	}
	return (0);
}

static int	create_threads(t_args *philo_d, t_shared *shared)
{
	int			i;
	pthread_t	death;
	pthread_t	*philo;
	
	shared->philos = malloc(sizeof(pthread_t) * shared->n_philo);
	philo = shared->philos;
	if (!philo)
		exit (EXIT_FAILURE);
	i = 0;
	if (pthread_create(&death, NULL, checker, (void *)shared) != 0)
		return (NO);
	while (i < shared->n_philo) 
	{
		if (pthread_create(&philo[i], NULL, routine, (void *)&philo_d[i]) != 0) 
		{
			while (--i >= 0) 
				pthread_mutex_destroy(&philo_d[i].l_fork);
			return (NO);
		}
		i++;
	}
	ft_join_threads(philo, philo_d, death);
	free(philo);
	return (YES);
}

static int	init_forks(t_args *philo_data)
{
	int	i;

	i = 0;
	while (i < philo_data->shared->n_philo) 
	{
		if (pthread_mutex_init(&philo_data[i].l_fork, NULL) != 0) 
		{
			perror("Failed to initialize l_fork mutex");
			while (--i >= 0)
				pthread_mutex_destroy(&philo_data[i].l_fork);
			return (NO);
		}
		i++;
	}
	i = 0;
	while (i < philo_data->shared->n_philo) 
	{
		if (i + 1 == philo_data->shared->n_philo)
			philo_data[i].r_fork = &philo_data[0].l_fork;
		else
			philo_data[i].r_fork = &philo_data[i + 1].l_fork;
		i++;
	}
	return (YES);
}

static void	init_philo(t_args *data, t_args *philo_data)
{
	int	i;

	i = 0;
	while (i < data->shared->n_philo)
	{
		philo_data[i] = *data;
		philo_data[i].philo_id = i + 1;
		i++;
	}
}

void	ft_threads(t_args *data, t_shared *shared)
{
	t_args	*philo_data;

	philo_data = malloc(sizeof(t_args) * shared->n_philo);
	if (!philo_data)
		return ;
	init_philo(data, philo_data);
	if (init_forks(philo_data) == NO) 
	{
		free(philo_data);
		return ;
	}
	if (create_threads(philo_data, shared) == NO) 
	{
		perror("fail creating threads");
		return ;
	}
	ft_destroy_mutexes(philo_data, shared);
	free(philo_data);
}
