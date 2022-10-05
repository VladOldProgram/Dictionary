/**
 * dict2.c -- программа чтения словаря, поиска и печати словарных статей по заданному шаблону
 *
 * Copyright (c) 2020, Vladislav Shkut <shkut@cs.petrsu.ru>
 *
 * This code is licensed under a MIT-style license.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

#define MAXLINE 1024
#define MEMORY 10485760 /* 10 МБ для помещения всего словаря в буфер */

/* Загружает данные из файла словаря dict в буфер dictionary.
     Возвращает dictionary при успешной загрузке, иначе - NULL */
char* load_dictionary(char *dictionary, FILE *dict);

/* Находит статьи по заданному шаблону, сохраняя их в entries. Возвращает entries */
char* filter_dictionary(char *pattern, const char *dictionary, char *entries);

int main(int argc, char *argv[])
{      
    setlocale(LC_ALL, "");

    /* Шаблон для поиска статьи */
    char pattern[MAXLINE] = "";

    /* Если не указан путь к файлу словаря, 
        выводим сообщение и завершаем программу */
    if (argc < 2) {
        fprintf(stderr, "Вы не указали путь к файлу словаря\n");
        exit(EXIT_FAILURE);
    }

    /* Открываем файл словаря */
    FILE *dict = fopen(argv[1], "r");
    
    /* Если запрошенный файл не доступен, 
        выводим сообщение и завершаем программу */
    if (dict == NULL) {
        perror("Открытие словаря");
        exit(EXIT_FAILURE);
    }

    char *dictionary;
    char *entries;

    /* Выделим необходимую память. Завершение программы, если не удалось выделить память */
    if ((dictionary = malloc(MEMORY)) == NULL) {
        fprintf(stderr, "No memory can be allocated, exiting.. \n");
        exit(EXIT_FAILURE);
    }

    /* Выделим необходимую память. Завершение программы, если не удалось выделить память */
    if ((entries = malloc(MEMORY)) == NULL) {
        fprintf(stderr, "No memory can be allocated, exiting.. \n");
        exit(EXIT_FAILURE);
    }

    /* Загружаем данные из файла словаря dict в буфер dictionary */
    dictionary = load_dictionary(dictionary, dict);

    /* Если запрошенный файл не доступен, 
        выводим сообщение и завершаем программу */
    if (dictionary == NULL) {
        perror("Чтение словаря");
        exit(EXIT_FAILURE);
    }

    /* Цикл для многоразового ввода шаблона и поиска статей */
    while (fprintf(stdout, "Введите слово для поиска статей: \n") > 0 && fgets(pattern, MAXLINE, stdin) != 0) {
        /* Обнуляем выделенную для entries память, чтобы записать туда новый результат поиска */
        memset(entries, 0, MEMORY);
        entries = filter_dictionary(pattern, dictionary, entries);
        if (entries != NULL)
            fprintf(stdout, "%s", entries);
    }

    /* Завершаем работу с файлом словаря */
    fclose(dict);

    /* Освобождаем выделенную память */
    free(entries);
    free(dictionary);

    return 0;
}

/* Загружает данные из файла словаря dict в буфер dictionary.
     Возвращает dictionary при успешной загрузке, иначе - NULL */
char* load_dictionary(char *dictionary, FILE *dict)
{
    /* Найдем длинну файла
        (переведем курсор в конец файла, считаем количество символов до него, вернем курсор в начало файла) */
    fseek(dict, 0, SEEK_END);
    if (fseek(dict, 0, SEEK_END) == -1) {
        perror("Чтение словаря");
        exit(EXIT_FAILURE);
    }
    long dictionary_lenght = ftell(dict);
    if (ftell(dict) == -1) {
        perror("Чтение словаря");
        exit(EXIT_FAILURE);
    }
    fseek(dict, 0, SEEK_SET);
    if (fseek(dict, 0, SEEK_SET) == -1) {
        perror("Чтение словаря");
        exit(EXIT_FAILURE);
    }
    /* Сравним количество успешно считанных символов со значением длинны исходного файла */
    size_t result = fread(dictionary, sizeof(char), dictionary_lenght, dict);

    /* Если они не совпадают, возвращаем NULL */
    if (result != dictionary_lenght)
        return NULL;
    else
        return dictionary;
}

/* Находит статьи по заданному шаблону, сохраняя их в entries. Возвращает entries */
char* filter_dictionary(char *pattern, const char *dictionary, char *entries)
{
    int i, j = 0;
    int k = 0; /* Счетчик найденных статей */
    int current_point = 0; /* Текущее местоположение в dictionary */
    int space = 0; /* Расстояние, на которое надо сдвинуться от начала entries, чтобы записать туда новую статью */

    /* Вспомогательный шаблон */
    char help_pattern[MAXLINE] = "";

    /* Текущая строка */
    char current_line[MAXLINE] = ""; 

    /* Вспомогательная строка */
    char help_current_line[MAXLINE] = "";

    /* Флаг соответствия текущей статьи условию отбора */
    int matched_entry = 0;

    /* Просматриваем словарь, печатая строки запрошенной статьи */
    while (dictionary[current_point] != '\0') {
        int n = 0;
        for (j = current_point; dictionary[j] != '\n'; j++) {
            current_line[n] = dictionary[j];
            n++;
        }
        current_line[n] = '\n';
        current_line[n + 1] = '\0';
        current_point = j + 1;
        /* Если первый символ строки не является пробельным разделителем,
            найдено начало новой словарной статьи */
        if (!isspace(current_line[0])) {
            /* Определяем, выполнено ли условие отбора для данной статьи */
            /* Если шаблон не имеет специальных символов для поиска */
            if (pattern[0] != '^' && pattern[strlen(pattern) - 2] != '$') {
                /* Ищем в строке фрагмент первого совпадения с шаблоном  */
	        for (i = 0; i <= strlen(current_line); i++) {
                    if (strncmp(current_line + i, pattern, strlen(pattern) - 1) == 0) {
                        matched_entry = 1;
                        k++;
                    }
                    else
                        matched_entry = 0;
                    if (matched_entry == 1)
                        break; 
                }
            }
            /* Если шаблон начинается со специального символа поиска "^" */
            if (pattern[0] == '^' && pattern[strlen(pattern) - 2] != '$') {
                /* Сначала преобразуем шаблон, убрав "^" */
                for (i = 1; i < strlen(pattern) - 1; i++)
                    help_pattern[i - 1] = pattern[i];
                 /* Найдем слова, которые начинаются с заданного шаблона */
                 if (strncmp(current_line, help_pattern, strlen(help_pattern)) == 0) {                          
                        matched_entry = 1;
                        k++;
                    }
                    else
                        matched_entry = 0;
            }
            /* Если шаблон оканчивается на специальный символ поиска "$" */
            if (pattern[0] != '^' && pattern[strlen(pattern) - 2] == '$') {  
                /* Сначала преобразуем шаблон, убрав "$" */                 
                for (i = 0; pattern[i] != '$'; i++)
                    help_pattern[i] = pattern[i];
                /* Найдем слова, которые оканчиваются на заданный шаблон */
                if (strncmp(current_line + strlen(current_line) - strlen(help_pattern) - 1, help_pattern, strlen(help_pattern)) == 0) {
                    matched_entry = 1;
                    k++;
                }
                else
                    matched_entry = 0;
            }
            /* Если шаблон имеет оба специальных символа поиска одновременно */
            if (pattern[0] == '^' && pattern[strlen(pattern) - 2] == '$') { 
                /* Сначала преобразуем шаблон, убрав "^" и "$" */
                for (i = 1; pattern[i] != '\0'; i++) {
                    help_pattern[i - 1] = pattern[i];
                    if (help_pattern[i - 1] == '$')
                        help_pattern[i - 1] = '\n';
                }
                help_pattern[strlen(help_pattern) - 1] = '\0';
                /* Найдем слово, в точности совпадающее с заданным шаблоном */
                if (strcmp(current_line, help_pattern) == 0) {
                    matched_entry = 1;
                    k++;
                }
                else
                    matched_entry = 0;
            }
            /* Если выполнено условие отбора, дополняем entries этой статьей */
            if (matched_entry) {
                sprintf(entries + space, "%s", current_line);
                space += strlen(current_line);	
                current_line[0] = dictionary[j];                
                while (dictionary[current_point] != '\0' && isspace(current_line[0])) {
                    int n = 0;
                    for (j = current_point; dictionary[j] != '\n'; j++) {
                        current_line[n] = dictionary[j];
                        n++;
                    }
                    current_line[n] = '\n';
                    current_line[n + 1] = '\0';
                    current_point = j + 1;
                    sprintf(entries + space, "%s", current_line);
                    space += strlen(current_line);
                    if (!isspace(dictionary[current_point]))
                        break;
                }
            }
        }
    }
    /* Если счетчик найденных статей равен нулю, выводим соответствующее сообщение */
    if (k == 0)
	fprintf(stdout, "Совпадений не найдено\n");
    return entries;
}