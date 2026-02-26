/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: finarako <finarako@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 15:42:49 by finarako          #+#    #+#             */
/*   Updated: 2026/02/25 14:44:17 by finarako         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif
# include <stdbool.h>
# include <stdlib.h>
# include <unistd.h>

bool	find_n(char *s1);
char	*ft_strjoin(char *s1, char *s2);
int		ft_strlen(char *s1);
char	*get_next_line(int fd);

#endif