/**
 * dict1.c -- программа чтения словаря, поиска и печати словарных статей по заданному шаблону
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

/* Находит статьи по заданному шаблону */
void show_entries(const char *pattern, FILE *dict);

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
    
    /* Получаем шаблон для поиска статей */
    fprintf(stdout, "Введите слово для поиска статей: \n");
    fgets(pattern, MAXLINE, stdin);

    /* Просматриваем словарь, печатая строки запрошенной статьи */
    show_entries(pattern, dict);

    /* Завершаем работу с файлом словаря */
    fclose(dict);

    return 0;
}

/* Находит статьи по заданному шаблону */
void show_entries(const char *pattern, FILE *dict)
{
    int i;
    int k = 0; /* Счетчик найденных статей */

    /* Вспомогательный шаблон */
    char help_pattern[MAXLINE] = "";

    /* Текущая строка */
    char current_line[MAXLINE] = ""; 

    /* Флаг соответствия текущей статьи условию отбора */
    int matched_entry = 0;

    /* Просматриваем словарь, печатая строки запрошенной статьи */
    while (fgets(current_line, MAXLINE, dict) != NULL) {
        /* Если первый символ строки не является пробельным разделителем,
            найдено начало новой словарной статьи */
        if (!isspace(current_line[0])) {
            /* Определяем, выполнено ли условие отбора для данной статьи */
            /* Если шаблон не имеет специальных символов для поиска */
            if (pattern[0] != '^' && pattern[strlen(pattern) - 2] != '$'){
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
            /* Если выполнено условие отбора, выведем статью */
            if (matched_entry) {
                fprintf(stdout, "%s", current_line);
                while (fgets(current_line, MAXLINE, dict) && isspace(current_line[0]))
                    fprintf(stdout, "%s", current_line);
            }	
        }
    }
    /* Если счетчик найденных статей равен нулю, выводим соответствующее сообщение */
    if (k == 0)
	fprintf(stdout, "Совпадений не найдено\n");
}