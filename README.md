# get_next_line

*This project has been created as part of the 42 curriculum by finarako.*

---

## Description

`get_next_line` is a C function that reads a file descriptor line by line, returning one line per call — including the terminating `\n` when present. It is called repeatedly until the end of the file, at which point it returns `NULL`.

The goal of the project is to understand how to manage persistent state between function calls using `static` variables, how the `read()` syscall works with a configurable buffer size, and how to handle dynamic memory allocation correctly without leaks.

The **bonus** version extends the function to handle multiple file descriptors simultaneously by using a static array indexed by `fd`.

---

## Instructions

### Files

**Mandatory:**
```
get_next_line.c
get_next_line_utils.c
get_next_line.h
```

**Bonus:**
```
get_next_line_bonus.c
get_next_line_utils_bonus.c
get_next_line_bonus.h
```

### Compilation

```bash
# Mandatory — with default BUFFER_SIZE defined in the header
cc -Wall -Wextra -o gnl main.c get_next_line.c get_next_line_utils.c

# Override BUFFER_SIZE at compile time
cc -Wall -Wextra -D BUFFER_SIZE=1   -o gnl main.c get_next_line.c get_next_line_utils.c
cc -Wall -Wextra -D BUFFER_SIZE=42  -o gnl main.c get_next_line.c get_next_line_utils.c
cc -Wall -Wextra -D BUFFER_SIZE=9999 -o gnl main.c get_next_line.c get_next_line_utils.c

# Bonus — multiple file descriptors
cc -Wall -Wextra -D BUFFER_SIZE=42 -o gnl_bonus main_bonus.c get_next_line_bonus.c get_next_line_utils_bonus.c
```

### Memory check

```bash
# With AddressSanitizer (built into gcc/clang)
cc -Wall -Wextra -fsanitize=address -D BUFFER_SIZE=42 -o gnl main.c get_next_line.c get_next_line_utils.c
./gnl

# With Valgrind
cc -g -Wall -Wextra -D BUFFER_SIZE=42 -o gnl main.c get_next_line.c get_next_line_utils.c
valgrind --leak-check=full ./gnl
```

### Main — Mandatory

```c
#include "get_next_line.h"
#include <stdio.h>
#include <fcntl.h>

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
```

### Main — Bonus (3 file descriptors simultaneously)

```c
#include "get_next_line_bonus.h"
#include <stdio.h>
#include <fcntl.h>

int main()
{
    int  fd1;
    int  fd2;
    int  fd3;
    char *l1;
    char *l2;
    char *l3;

    fd1 = open("fichier1.txt", O_RDONLY);
    fd2 = open("fichier2.txt", O_RDONLY);
    fd3 = open("fichier3.txt", O_RDONLY);
    if (fd1 < 0 || fd2 < 0 || fd3 < 0)
    {
        printf("Erreur ouverture fichier\n");
        return (1);
    }
    printf("=== lecture alternee sur 3 fd ===\n");
    l1 = get_next_line(fd1);
    l2 = get_next_line(fd2);
    l3 = get_next_line(fd3);
    while (l1 || l2 || l3)
    {
        if (l1) { printf("fd1: [%s]\n", l1); free(l1); }
        if (l2) { printf("fd2: [%s]\n", l2); free(l2); }
        if (l3) { printf("fd3: [%s]\n", l3); free(l3); }
        l1 = get_next_line(fd1);
        l2 = get_next_line(fd2);
        l3 = get_next_line(fd3);
    }
    close(fd1);
    close(fd2);
    close(fd3);
    return (0);
}
```

---

## Algorithm — Explanation and Justification

The algorithm is split into 4 functions with clear responsibilities.

### Overview

```
get_next_line(fd)
    │
    ├── search_n(fd, src, temp)   → reads the file until finding a ' n' in temp
    │
    ├── display_n(temp)           → extract the line to return
    │
    └── move_temp(temp)           → advance temp after the ' n' for the next call
```

### Why `static char *temp`

`temp` is declared `static` so it persists between calls. Without `static`, `temp` would reset to `NULL` on every call and lose whatever was read beyond the last `\n`.

Example with `BUFFER_SIZE=4` and `"hello\nworld\n"`:
- Call 1: reads `"hell"`, then `"o\nwo"` — finds `\n`, stops. Extracts `"hello\n"`. Saves `"wo"` in `temp` for the next call.
- Call 2: `temp` still holds `"wo"`. Reads `"rld\n"`. Extracts `"world\n"`.
- Call 3: `temp` is empty → `read` returns 0 → returns `NULL`.

For the **bonus**, `temp` becomes `static char *temp[1024]` so each `fd` has its own independent slot: `temp[fd]`. Reading `fd1` never overwrites what was saved for `fd2`.

### Why `src` is NOT static

`src` is a temporary read buffer. It only needs to exist during one call. Declaring it `static` would cause old data from the previous call to mix with new data. A fresh `malloc` each call guarantees a clean buffer.

### How `read()` knows where to continue

The operating system maintains a cursor (offset) for each open file descriptor. Every call to `read(fd, src, BUFFER_SIZE)` automatically advances that cursor by the number of bytes read. The function never needs to track the position manually.

```
BUFFER_SIZE=4, file="hello\nworld"
read 1 → src="hell", cursor moves to byte 4, returns 4
read 2 → src="o\nwo", cursor moves to byte 8, returns 4
read 3 → src="rld",  cursor moves to byte 11, returns 3
read 4 → end of file, returns 0 → break out of loop
```

### `ft_strjoin` — ownership and the `free(s1)` design

`ft_strjoin(s1, s2)` allocates a new string containing `s1 + s2`, then calls `free(s1)`. This means the caller transfers ownership of `s1` to `ft_strjoin`, which handles freeing it. This pattern avoids double-free and simplifies memory management in `search_n`:

```c
temp = ft_strjoin(temp, src);
// the old temp is freed inside ft_strjoin automatically
```

### `old_temp` — why it is necessary

`move_temp` calls `ft_strjoin(NULL, temp + offset)`, which allocates a **new block**. After this call, `temp` is reassigned to the new block. Without saving the old address first, the original block can never be freed → memory leak on every call.

```c
old_temp = temp;          // save address of current block
temp = move_temp(temp);   // temp now points to a new block
free(old_temp);           // free the old block safely
```

### All edge cases handled

| Case | Where | Behaviour |
|---|---|---|
| `fd < 0` or `BUFFER_SIZE <= 0` | start of `get_next_line` | returns `NULL` immediately |
| `malloc` failure | anywhere | returns `NULL` |
| Empty file | `read` returns 0 on first call | `temp` stays `NULL` → returns `NULL` |
| `temp` is `NULL` on first call | `find_n` | guard `if (!s1) return false` → proceeds to read |
| `BUFFER_SIZE = 1` | `search_n` | loops many times, correctness unchanged |
| `BUFFER_SIZE = 9999` | `search_n` | reads entire file in one call, correctness unchanged |
| Last line has no `\n` | `display_n` | copies without appending `\n` |
| End of file after last line | next call: `temp` is empty | `temp[0] == '\0'` check → `free`, return `NULL` |
| Multiple lines in `temp` | `move_temp` | saves remainder for next call via `static` |
| Multiple `fd` (bonus only) | `temp[fd]` array | each `fd` has its own independent slot |

---

## Resources

### Documentation
- [`read()` man page](https://man7.org/linux/man-pages/man2/read.2.html) — Linux manual for the `read` syscall
- [`open()` man page](https://man7.org/linux/man-pages/man2/open.2.html) — how to open file descriptors
- [`malloc` / `free` man page](https://man7.org/linux/man-pages/man3/malloc.3.html) — heap memory management
- [Static variables in C — cppreference](https://en.cppreference.com/w/c/language/storage_duration) — storage duration and linkage

### Tools
- [Valgrind](https://valgrind.org/) — memory error and leak detector
- GCC AddressSanitizer (`-fsanitize=address`) — compile-time memory checker built into GCC/Clang

### AI Usage

Claude (claude.ai) was used during the development of this project for the following tasks:

- **Debugging** — identifying bugs in the original code (unallocated `temp2`, off-by-one in `ft_strjoin`, missing null-terminator after `read`, arithmetic pointer issue with `temp = temp + i`).
- **Memory analysis** — explaining why `free(temp)` after `temp = move_temp(temp)` caused leaks, and why `old_temp` is needed.
- **Testing** — generating test cases for edge cases (empty file, last line without `\n`, `BUFFER_SIZE=1`, `BUFFER_SIZE=9999`, 200 000 character files, multiple file descriptors).
- **Explanations** — understanding `static` variables, how `read()` maintains its cursor, memory zones (stack / heap / static), and Valgrind output interpretation.
- **README writing** — structuring and writing this document.

The core algorithm and logic were designed by me finarako. AI was used as a learning tool, not as a code generator.
