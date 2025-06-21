# Звіт з виконання лабораторної роботи №8

**Виконав: Гнилицький Дмитро**

**Група: ТВ-33**

**Система: Linux Mint 22.1 'Xia' MATE Edition (VirtualBox)**

**Варіант №3**

## Завдання 8.1:

**Питання:** Чи може виклик `count = write(fd, buffer, nbytes)` повернути значення, відмінне від `nbytes`?

**Відповідь:** Так, може. Це відбувається в таких випадках:
- При записі в pipe з обмеженим буфером
- При перериванні сигналом
- При недостатньому місці на диску
- При записі в неблокуючий дескриптор

**Код програми:**
```c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    int flags = fcntl(pipefd[1], F_GETFL);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        exit(1);
    }
    if (fcntl(pipefd[1], F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        exit(1);
    }

    size_t nbytes = 1000000;
    char *buffer = (char *)malloc(nbytes);
    if (!buffer) {
        perror("malloc");
        exit(1);
    }
    memset(buffer, 'A', nbytes);

    ssize_t count = write(pipefd[1], buffer, nbytes);
    if (count < 0) {
        perror("write");
        printf("Помилка: %s\n", strerror(errno));
    } else {
        printf("Записано: %zd з %zu байт\n", count, nbytes);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    free(buffer);
    return 0;
}
```

**Компіляція та виконання:**
```bash
mkdir -p ~/aspz8/task8.1
cd ~/aspz8/task8.1
nano write_example.c 
gcc write_example.c -o write_example
./write_example
```

**Вивід:**

![image](https://github.com/user-attachments/assets/c44183e7-bcda-4386-a0fb-061b4a5bd210)


```
dima@dima-VirtualBox:~/aspz8/task8.1$ gcc write_example.c -o write_example
./write_example
Записано: 65536 з 1000000 байт
```

**Висновок:** Програма демонструє, що виклик `write()` може повернути значення менше за `nbytes`. У даному випадку було записано лише 65536 байт у pipe через обмеження буфера.

---

## Завдання 8.2:

**Питання:** Що буде в буфері після виконання операцій зсуву та читання?

**Відповідь:** Буфер міститиме 4 байти, починаючи з 3-го байта: 2 (байт 3), 3 (байт 4), 3 (байт 5), 7 (байт 6).

**Код програми:**
```c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = open("data.bin", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    unsigned char data[] = {4, 5, 2, 2, 3, 3, 7, 9, 1, 5};
    ssize_t n = write(fd, data, sizeof(data));
    if (n != sizeof(data)) {
        perror("write");
        close(fd);
        return 1;
    }

    if (lseek(fd, 3, SEEK_SET) < 0) {
        perror("lseek");
        close(fd);
        return 1;
    }

    unsigned char buffer[4];
    n = read(fd, buffer, 4);
    if (n != 4) {
        perror("read");
        close(fd);
        return 1;
    }

    printf("Буфер: %d, %d, %d, %d\n", buffer[0], buffer[1], buffer[2], buffer[3]);
    close(fd);
    return 0;
}
```

**Компіляція та виконання:**
```bash
mkdir -p ~/aspz8/task8.2
cd ~/aspz8/task8.2
nano read_example.c  # вставити код вище
gcc read_example.c -o read_example
./read_example
```

**Вивід:**

![image](https://github.com/user-attachments/assets/08bcd654-56ba-4bc6-9e5e-3cf11b24a7a5)


```
dima@dima-VirtualBox:~/aspz8/task8.2$ gcc read_example.c -o read_example
./read_example
Буфер: 2, 3, 3, 7

```

**Висновок:** Результат виконання програми підтверджує, що після зсуву на 3 байти та читання 4 байтів буфер містить значення 2, 3, 3, 7.

---

## Завдання 8.3:

**Завдання:** Дослідити найгірші випадки для qsort та реалізувати тести коректності.

**Код програми:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int int_compare(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

int is_sorted(int *array, size_t n) {
    if (n <= 1) return 1;
    for (size_t i = 0; i < n-1; i++) {
        if (array[i] > array[i+1]) return 0;
    }
    return 1;
}

void generate_array(int *array, size_t n, int type) {
    switch (type) {
        case 0: // Випадкові дані
            for (size_t i = 0; i < n; i++) array[i] = rand() % n;
            break;
        case 1: // Відсортовано
            for (size_t i = 0; i < n; i++) array[i] = i;
            break;
        case 2: // Зворотньо відсортовано
            for (size_t i = 0; i < n; i++) array[i] = n - i;
            break;
        case 3: // "Органна труба"
            for (size_t i = 0; i < n/2; i++) array[i] = i;
            for (size_t i = n/2; i < n; i++) array[i] = n - i;
            break;
        case 4: // Медіанний "убивця"
            for (size_t i = 0; i < n/2; i++) {
                array[2*i] = i;
                array[2*i+1] = n - i;
            }
            if (n % 2) array[n-1] = n/2;
            break;
    }
}

void test_correctness() {
    int tests[][20] = {
        {0},       // Тест 1: 0 елементів
        {1},       // Тест 2: 1 елемент
        {2,1},     // Тест 3: 2 елементи (не відсортовані)
        {1,2},     // Тест 4: 2 елементи (відсортовані)
        {5,5,5},   // Тест 5: однакові елементи
        {1,2,3,4,5}, // Тест 6: відсортований масив
        {5,4,3,2,1}, // Тест 7: зворотньо відсортований
        {3,1,4,2,5}  // Тест 8: випадковий порядок
    };
    int sizes[] = {0,1,2,2,3,5,5,5};

    for (int i = 0; i < sizeof(sizes)/sizeof(int); i++) {
        qsort(tests[i], sizes[i], sizeof(int), int_compare);
        if (is_sorted(tests[i], sizes[i])) {
            printf("Тест %d: PASSED\n", i+1);
        } else {
            printf("Тест %d: FAILED\n", i+1);
        }
    }
}

void test_performance(int type) {
    size_t n = 10000000;
    int *array = malloc(n * sizeof(int));
    generate_array(array, n, type);

    clock_t start = clock();
    qsort(array, n, sizeof(int), int_compare);
    clock_t end = clock();

    double time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Тип %d: %.3f секунд\n", type, time);
    free(array);
}

int main() {
    srand(time(NULL));
    printf("=== Коректність ===\n");
    test_correctness();

    printf("\n=== Продуктивність (n=10,000,000) ===\n");
    for (int i = 0; i <= 4; i++) {
        test_performance(i);
    }
    return 0;
}
```

**Компіляція та виконання:**
```bash
mkdir -p ~/aspz8/task8.3
cd ~/aspz8/task8.3
nano qsort_test.c  # вставити код вище
gcc qsort_test.c -o qsort_test
./qsort_test
```

**Вивід:**

![image](https://github.com/user-attachments/assets/21e8759c-8fa6-481a-ac3c-8fb0307b9bfd)


```
dima@dima-VirtualBox:~/aspz8/task8.3$ gcc qsort_test.c -o qsort_test
./qsort_test
=== Коректність ===
Тест 1: PASSED
Тест 2: PASSED
Тест 3: PASSED
Тест 4: PASSED
Тест 5: PASSED
Тест 6: PASSED
Тест 7: PASSED
Тест 8: PASSED

=== Продуктивність (n=10,000,000) ===
Тип 0: 4.013 секунд
Тип 1: 1.677 секунд
Тип 2: 1.887 секунд
Тип 3: 1.873 секунд
Тип 4: 2.856 секунд

```

**Висновки:**
1. Найгірші випадки для qsort:
   - Тип 4 ("медіанний убивця") - 2.856 сек
   - Тип 0 (випадкові дані) - 4.013 сек
2. Найкращі випадки:
   - Тип 1 (відсортований) - 1.677 сек
   - Тип 3 ("органна труба") - 1.873 сек
3. Усі тести коректності пройдені успішно

---

## Завдання 8.4:

**Питання:** Який результат виконання програми з fork()?

**Відповідь:** Програма виведе два значення:
1. У батьківському процесі: PID дочірнього процесу
2. У дочірньому процесі: 0

**Код програми:**
```c
#include <stdio.h>
#include <unistd.h>

int main() {
  int pid;
  pid = fork();
  printf("%d\n", pid);
  return 0;
}
```

**Компіляція та виконання:**
```bash
cd ~/aspz8
nano fork_example.c  # вставити код вище
gcc fork_example.c -o fork_example
./fork_example
```

**Вивід:**

![image](https://github.com/user-attachments/assets/d0834a82-0c42-4044-a24f-22949892ec3e)

```
dima@dima-VirtualBox:~/aspz8$ gcc fork_example.c -o fork_example
./fork_example
2043
0
```

**Висновок:** Програма демонструє роботу системного виклику fork(), який створює новий процес. У батьківському процесі повертається PID дочірнього процесу, а у дочірньому - 0.

---

## Варіант 3: Зміна файлу без зміни позиції

**Завдання:** Створити програму, яка змінює вміст відкритого файлу без переміщення вказівника позиції.

**Рішення:** Використовуємо системний виклик pwrite(), який виконує запис у файл за вказаним зміщенням, не змінюючи поточний вказівник позиції.

**Код програми:**
```c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {
    int fd = open("testfile.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    const char *initial_data = "Hello World!";
    ssize_t written = write(fd, initial_data, strlen(initial_data));
    if (written < 0) {
        perror("write");
        close(fd);
        return 1;
    }
    
    off_t current_pos = lseek(fd, 0, SEEK_CUR);
    printf("Поточна позиція після запису: %ld\n", current_pos);

    const char *new_data = "LINUX";
    written = pwrite(fd, new_data, strlen(new_data), 0);
    if (written < 0) {
        perror("pwrite");
        close(fd);
        return 1;
    }

    current_pos = lseek(fd, 0, SEEK_CUR);
    printf("Позиція після pwrite: %ld\n", current_pos);

    if (lseek(fd, 0, SEEK_SET) < 0) {
        perror("lseek");
        close(fd);
        return 1;
    }

    char buffer[20];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("read");
        close(fd);
        return 1;
    }
    buffer[bytes_read] = '\0';
    printf("Новий вміст файлу: %s\n", buffer);

    close(fd);
    return 0;
}
```

**Компіляція та виконання:**
```bash
cd ~/aspz8
nano pwrite_example.c  # вставити код вище
gcc pwrite_example.c -o pwrite_example
./pwrite_example
```

**Вивід:**

![image](https://github.com/user-attachments/assets/81f1c5cb-fb18-46e8-90f9-8409db71299a)


```
dima@dima-VirtualBox:~/aspz8$ gcc pwrite_example.c -o pwrite_example
./pwrite_example
Поточна позиція після запису: 12
Позиція після pwrite: 12
Новий вміст файлу: LINUX World!

```

**Додаткова програма (change_file.c):**
```c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {
    int fd = open("test.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    const char *text = "Hello, World!";
    ssize_t written = write(fd, text, strlen(text));
    if (written < 0) {
        perror("write");
        close(fd);
        return 1;
    }

    off_t pos = lseek(fd, 0, SEEK_CUR);
    printf("Поточна позиція: %ld\n", pos);

    const char *new_text = "Hi!!!";
    written = pwrite(fd, new_text, strlen(new_text), 0);
    if (written < 0) {
        perror("pwrite");
        close(fd);
        return 1;
    }

    pos = lseek(fd, 0, SEEK_CUR);
    printf("Позиція після pwrite: %ld\n", pos);

    if (lseek(fd, 0, SEEK_SET) < 0) {
        perror("lseek");
        close(fd);
        return 1;
    }

    char buf[20];
    ssize_t bytes_read = read(fd, buf, sizeof(buf) - 1);
    if (bytes_read < 0) {
        perror("read");
        close(fd);
        return 1;
    }
    buf[bytes_read] = '\0';
    
    printf("Вміст файлу: %s\n", buf);

    close(fd);
    return 0;
}
```

**Компіляція та виконання:**
```bash
mkdir -p ~/aspz8/task8.4
cd ~/aspz8/task8.4
nano change_file.c  # вставити код вище
gcc change_file.c -o change_file
./change_file
```

**Вивід:**

![image](https://github.com/user-attachments/assets/dbef37ff-b4d1-444e-b3d2-419a1a98800f)


```
dima@dima-VirtualBox:~/aspz8/task8.4$ gcc change_file.c -o change_file
./change_file
Поточна позиція: 13
Позиція після pwrite: 13
Вміст файлу: Hi!!!, World!u

```

**Висновок:** Обидві програми демонструють, що pwrite() дозволяє змінювати вміст файлу у вказаній позиції, не змінюючи поточний вказівник позиції у файлі.
