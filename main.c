#include <fcntl.h>
#include <stdio.h>
int main()
{
	int  fd;
    char *line;
    int  i;

    i = 1;
    fd = open("fichier.txt", O_RDONLY);
    if (fd < 0)
    {
        printf("Erreur ouverture fichier\n");
        return (1);
    }
    while ((line = get_next_line(fd)) != NULL)
    {
        printf("ligne %d: [%s]\n", i++, line);
        free(line);
    }
    close(fd);
    return (0);
}