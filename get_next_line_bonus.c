/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: finarako <finarako@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 15:56:40 by finarako          #+#    #+#             */
/*   Updated: 2026/02/25 14:27:37 by finarako         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

static char	*search_n(int fd, char *src, char *temp)
{
	int	take_read;

	while (find_n(temp) == false)
	{
		take_read = read(fd, src, BUFFER_SIZE);
		if (take_read <= 0)
			break ;
		src[take_read] = '\0';
		temp = ft_strjoin(temp, src);
	}
	free(src);
	return (temp);
}

static char	*display_n(char *temp)
{
	int		i;
	int		lengh;
	char	*display;

	i = 0;
	lengh = 0;
	while (temp[lengh] && temp[lengh] != '\n')
		lengh++;
	display = malloc(sizeof(char) * (lengh + 2));
	if (!display)
		return (NULL);
	while (i < lengh)
	{
		display[i] = temp[i];
		i++;
	}
	if (temp[i] == '\n')
		display[i++] = '\n';
	display[i] = '\0';
	return (display);
}

static char	*move_temp(char *temp)
{
	char	*new_temp;
	int		lengh;

	lengh = 0;
	while (temp[lengh] && temp[lengh] != '\n')
		lengh++;
	if (temp[lengh] == '\n')
		lengh++;
	new_temp = ft_strjoin(NULL, temp + lengh);
	return (new_temp);
}

char	*get_next_line(int fd)
{
	char		*src;
	static char	*temp[1024];
	char		*display;
	char		*old_temp;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (0);
	src = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!src)
		return (0);
	temp[fd] = search_n(fd, src, temp[fd]);
	if (!temp[fd] || *temp[fd] == '\0')
	{
		free(temp[fd]);
		temp[fd] = NULL;
		return (NULL);
	}
	display = display_n(temp[fd]);
	old_temp = temp[fd];
	temp[fd] = move_temp(temp[fd]);
	free(old_temp);
	return (display);
}
