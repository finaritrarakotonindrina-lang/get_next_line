/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next _line.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: finarako <finarako@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 08:43:56 by finarako          #+#    #+#             */
/*   Updated: 2026/02/24 15:14:08 by finarako         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "get_next_line.h"

char	*search_n(int fd, char *src, char *temp)
{
	int	take_read;

	while (find_n(temp) == false)
	{
		take_read = read(fd, src, BUFFER_SIZE);
		if (take_read <= 0)
			break;
		src[take_read] = '\0';
		temp = ft_strjoin(temp, src);
	}
	free(src);
	return (temp);
}
char	*display_n(char *temp)
{
	int		i;
	int lengh;
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
	display[i]= '\0';
	return (display);
}
char	*move_temp(char *temp)
{
	char	*new_temp;
	int lengh;

	lengh = 0;
	while (temp[lengh] && temp[lengh] != '\n')
	lengh++;
	if (temp[lengh] == '\n')
	lengh++;
	new_temp = ft_strjoin(NULL,temp + lengh);
	return (new_temp);
}
char	*get_next_line(int fd)
{
	char	*src;
	static char	*temp;
	char	*display;
	char	*old_temp;

	if ( fd < 0 || BUFFER_SIZE <= 0)
	return (0);
	src = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!src)
	return (0);
	temp = search_n(fd, src, temp);
	if (!temp || temp[0] == '\0')
	{
		free(temp);
		temp = NULL;
		return (NULL);
	}
	display = display_n(temp);
	old_temp = temp;
	temp = move_temp(temp);
	free(old_temp);
	return (display);
}
