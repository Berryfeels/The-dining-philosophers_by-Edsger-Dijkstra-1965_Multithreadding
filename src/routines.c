/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routines.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stdi-pum <stdi-pum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 11:06:46 by stdi-pum          #+#    #+#             */
/*   Updated: 2024/11/30 19:51:16 by stdi-pum         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

void	ft_routine_time(t_args *philo_data, long routine_time)
{
	long	start;

	start = get_time();
	while ((dead_check(philo_data) == NO) 
		&& (get_time() - start) < routine_time)
	{
		usleep(500);
	}
}

void	sleeping_routine(t_args *philo_data)
{
	print_message(philo_data, SLEEPING);
	ft_routine_time(philo_data, philo_data->time_to_sleep);
}

void	eating_routine(t_args *philo_data)
{
	if (philo_data->can_i_eat == YES)
	{
		if (philo_data->philo_id % 2 == 0)
		{
			pthread_mutex_lock(&philo_data->l_fork);
			pthread_mutex_lock(philo_data->r_fork);
			print_message(philo_data, FORK);
		}
		else
		{
			pthread_mutex_lock(philo_data->r_fork);
			pthread_mutex_lock(&philo_data->l_fork);
			print_message(philo_data, FORK);
		}
		pthread_mutex_lock(&philo_data->shared->eating);
		philo_data->previous_eat_time = get_time();
		philo_data->shared->ntime_eating += 1;
		philo_data->p_round += 1;
		print_message(philo_data, EATING);
		pthread_mutex_unlock(&philo_data->shared->eating);
		ft_routine_time(philo_data, philo_data->time_to_eat);
		pthread_mutex_unlock(&philo_data->l_fork);
		pthread_mutex_unlock(philo_data->r_fork);
	}
	check_if_i_can_eat(philo_data);
}

void	thinking_routine(t_args *philo_data)
{
	print_message(philo_data, THINKING);
	while (philo_data->can_i_eat == NO)
	{
		dead_check(philo_data);
		check_if_i_can_eat(philo_data);
	}
}

void	*routine(void *arg)
{
	t_args	*philo_data;

	philo_data = (t_args *) arg;
	while (1)
	{
		thinking_routine(philo_data);
		if (stop_program_check(philo_data) == YES)
			break ;
		eating_routine(philo_data);
		if (stop_program_check(philo_data) == YES)
			break ;
		sleeping_routine(philo_data);
		if (stop_program_check(philo_data) == YES)
			break ;
	}
	return (NULL);
}
