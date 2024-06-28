#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH \
    80  //ширина и высота через define - define по сути просто макрос который заменяет все "WIDTH" и "HEIGHT"
        //в тексте на цифры
#define HEIGHT 25

void char_to_int(int **array_int, char **array_char);
int neighbours(int **array, int x, int y);  // подсчет соседей в подмассивах 3 на 3
void replace(int **array, int **tmp);  // замена количества соседов на живой или не живой
void display(char **array);             // функция вывода на экран
void input(int **array);                // ввод с терминала
void init_tmp(int **array, int **tmp);  // запись в тмп массив оригинального массива, потому что если менять
                                        // оригинальный массив прямо во время работы, он начинает менятся.
void calc(int **array, int **tmp);  // запуск подсчета соседей в каждой из клеток
void int_to_char(int **array_int, char **array_char);

int main() {
    initscr();
    int **array_of_n =
        malloc(HEIGHT * sizeof(int *));  // создаем массив для записи количества ближайших точек
    int *values_array_of_n =
        malloc(HEIGHT * WIDTH * sizeof(int));  // создаем массив для записи количества ближайших точек

    for (int i = 0; i < HEIGHT; i++) {  // создаем массив для записи количества ближайших точек
        array_of_n[i] =
            values_array_of_n + WIDTH * i;  // создаем массив для записи количества ближайших точек
    }
    for (int i = 0; i < HEIGHT; i++) {  // заполняем массив нулями
        for (int j = 0; j < WIDTH; j++) {
            array_of_n[i][j] = 0;
        }
    }

    char **array_of_char =
        malloc(HEIGHT * sizeof(char *));  // создаем массив для записи количества ближайших точек
    char *values_array_of_char =
        malloc(HEIGHT * WIDTH * sizeof(char));  // создаем массив для записи количества ближайших точек

    for (int i = 0; i < HEIGHT; i++) {  // создаем массив для записи количества ближайших точек
        array_of_char[i] =
            values_array_of_char + WIDTH * i;  // создаем массив для записи количества ближайших точек
    }
    for (int i = 0; i < HEIGHT; i++) {  // заполняем массив пробелами
        for (int j = 0; j < WIDTH; j++) {
            array_of_char[i][j] = '.';
        }
    }

    int **tmp_array = malloc(HEIGHT * sizeof(int *));              // временный массив
    int *values_tmp_array = malloc(HEIGHT * WIDTH * sizeof(int));  // временный массив

    for (int i = 0; i < HEIGHT; i++) {                // временный массив
        tmp_array[i] = values_tmp_array + WIDTH * i;  // временный массив
    }

    nodelay(stdscr, TRUE);

    input(array_of_n);
    if (freopen("/dev/tty", "r", stdin)) {
        int a = -1;
        int speed = 100;
        while (a != 'q') {
            clear();
            napms(speed);
            curs_set(0);
            noecho();
            a = getch();
            if (a == '+') {
                speed -= 5;
            }
            if (a == '-') {
                speed += 5;
            }
            int_to_char(array_of_n, array_of_char);
            display(array_of_char);
            init_tmp(array_of_n, tmp_array);
            calc(array_of_n, tmp_array);
            replace(array_of_n, tmp_array);
            refresh();
        }
    }

    free(array_of_n);
    free(values_array_of_n);
    free(array_of_char);
    free(values_array_of_char);
    free(tmp_array);
    free(values_tmp_array);  // чистим память
    endwin();
    return 0;
}

void display(char **array) {
    for (int i = 0; i < HEIGHT; i++) {  // вывод массива через вложенный цикл
        for (int j = 0; j < WIDTH; j++) {
            printw("%c", array[i][j]);
        }
        printw("\n");
    }
}

void replace(int **array, int **tmp) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (tmp[i][j] == 1) {
                if (array[i][j] == 3 || array[i][j] == 2) {
                    array[i][j] = 1;
                }
            } else if (tmp[i][j] == 0) {
                if (array[i][j] == 3) {
                    array[i][j] = 1;
                } else if (array[i][j] == 2) {
                    array[i][j] = 0;
                }
            } else {
                array[i][j] = 0;
            }
        }
    }
}

void calc(int **array, int **tmp) {
    for (int i = 0; i < HEIGHT; i++) {  // меняем массив цифр в зависимости от того куда хотим вставить точки
        for (int j = 0; j < WIDTH; j++) {
            if (tmp[i][j] == 1) {
                if (neighbours(tmp, j, i) == 3 || neighbours(tmp, j, i) == 2) {
                    array[i][j] = 1;
                } else {
                    array[i][j] = 0;
                }
            } else if (tmp[i][j] == 0) {
                if (neighbours(tmp, j, i) == 3) {
                    array[i][j] = 1;
                } else {
                    array[i][j] = 0;
                }
            }
        }
    }
}

int neighbours(int **array, int x, int y) {  // ищем количество соседей
    int dy, dx;
    int count_of_alive = 0;
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            dx = (x + j + WIDTH) % WIDTH;
            dy = (y + i + HEIGHT) % HEIGHT;
            if ((dx == x) && (dy == y)) continue;
            if (array[dy][dx] == 1) {
                count_of_alive += 1;
            }
        }
    }
    // скорее всего тут уменьшить на 1
    return count_of_alive;
}

void init_tmp(int **array, int **tmp) {
    for (int i = 0; i < HEIGHT; i++) {  // заполняем временный массив нулями
        for (int j = 0; j < WIDTH; j++) {
            tmp[i][j] = array[i][j];
        }
    }
}

void int_to_char(int **array_int, char **array_char) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {  // конвертируем массив с числами в массив с символами
            if (array_int[i][j] == 1) {
                array_char[i][j] = '#';
            } else if (array_int[i][j] == 0) {
                array_char[i][j] = '.';
            }
        }
    }
}

void input(int **array) {
    char c;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            do {
                c = getch();
            } while (c == '\n');
            if (c == '#') {
                array[i][j] = 1;
            } else if (c == '.') {
                array[i][j] = 0;
            }
        }
    }
}
