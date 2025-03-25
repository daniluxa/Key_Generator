#pragma warning (disable : 4996)
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <locale.h>

#define MAX_ID_LENGTH 30
#define MAX_SEGMENTS 10

const char valid_letters[] = "ABCEHIKLNOPRSTUWXYZ";
const char valid_digits[] = "123456789";
const char forbidden_chars[] = "DFGJMQV0";

// Проверка символа на принадлежность к строке
bool char_in_string(char c, const char* str) 
{
    for (int i = 0; str[i]; i++) 
    {
        if (c == str[i]) return true;
    }
    return false;
}

// Проверка корректности использования дефисов
bool validate_hyphens(const char* id) 
{
    // Не может начинаться или заканчиваться дефисом
    if (id[0] == '-' || id[strlen(id) - 1] == '-') 
    {
        fprintf(stderr, "Дефис не может быть в начале или конце ID\n");
        return false;
    }

    // Не может содержать несколько дефисов подряд
    for (int i = 0; id[i]; i++) 
    {
        if (id[i] == '-' && id[i + 1] == '-') 
        {
            fprintf(stderr, "Найдено несколько дефисов подряд\n");
            return false;
        }
    }

    return true;
}

// Проверка, является ли ID последним возможным
bool is_last_id(const char* id) 
{
    char copy[MAX_ID_LENGTH];
    strncpy(copy, id, MAX_ID_LENGTH);

    if (copy[sizeof(copy) - 1] != 0)
    {
        copy[sizeof(copy) - 1] = 0;
    }

    int segment_count = 0;
    char* segments[MAX_SEGMENTS] = { 0 };
    char* token = strtok(copy, "-");

    while (token != NULL) 
    {
        segments[segment_count++] = token;
        token = strtok(NULL, "-");
    }

    // Проверяем все сегменты на максимальные значения
    for (int i = 0; i < segment_count; i++) 
    {
        if (segments[i][0] != 'Z' || segments[i][1] != '9')
        {
            return false;
        }
    }

    return segment_count == MAX_SEGMENTS;
}


bool validate_id(const char* id) 
{
    // Проверка длины
    if (strlen(id) == 0 || strlen(id) >= MAX_ID_LENGTH) 
    {
        fprintf(stderr, "Недопустимая длина ID\n");
        return false;
    }

    // Проверка дефисов
    if (!validate_hyphens(id))
    {
        return false;
    }

    // Проверка запрещенных символов
    for (int i = 0; id[i]; i++) 
    {
        if (char_in_string(id[i], forbidden_chars)) 
        {
            fprintf(stderr, "Обнаружен запрещенный символ '%c'\n", id[i]);
            return false;
        }
    }

    // Проверка последнего возможного идентификатора
    if (is_last_id(id)) 
    {
        fprintf(stderr, "Достигнут последний возможный идентификатор\n");
        return false;
    }

    // Разбиение на сегменты
    char copy[MAX_ID_LENGTH];
    strncpy(copy, id, MAX_ID_LENGTH);

    if (copy[sizeof(copy) - 1] != 0)
    {
        copy[sizeof(copy) - 1] = 0;
    }

    int segment_count = 0;
    char* segments[MAX_SEGMENTS] = { 0 };
    char* token = strtok(copy, "-");

    while (token != NULL && segment_count < MAX_SEGMENTS) 
    {
        segments[segment_count++] = token;
        token = strtok(NULL, "-");
    }

    // Проверка каждого сегмента
    for (int i = 0; i < segment_count; i++) 
    {
        if (strlen(segments[i]) != 2) 
        {
            fprintf(stderr, "Сегмент '%s' должен состоять из 2 символов\n", segments[i]);
            return false;
        }

        if (!char_in_string(segments[i][0], valid_letters))
        {
            fprintf(stderr, "Недопустимая буква '%c' в сегменте '%s'\n", segments[i][0], segments[i]);
            return false;
        }

        if (!char_in_string(segments[i][1], valid_digits))
        {
            fprintf(stderr, "Недопустимая цифра '%c' в сегменте '%s'\n", segments[i][1], segments[i]);
            return false;
        }
    }

    return true;
}

// Увеличение буквы
char increment_letter(char c) 
{
    for (int i = 0; valid_letters[i]; i++) 
    {
        if (valid_letters[i] == c) 
        {
            return valid_letters[i + 1] ? valid_letters[i + 1] : valid_letters[0];
        }
    }
    return 'A';
}

// Увеличение цифры
char increment_digit(char d)
{
    for (int i = 0; valid_digits[i]; i++) 
    {
        if (valid_digits[i] == d) 
        {
            return valid_digits[i + 1] ? valid_digits[i + 1] : valid_digits[0];
        }
    }
    return '1';
}

// Генерация следующего ID
bool get_next_id(const char* current_id, char* next_id, size_t buffer_size) 
{
    // Проверка входного ID
    if (!validate_id(current_id)) 
    {
        return false;
    }

    char temp[MAX_ID_LENGTH];
    strncpy(temp, current_id, MAX_ID_LENGTH);
    temp[MAX_ID_LENGTH - 1] = '\0';

    // Разбиваем на сегменты
    int segment_count = 0;
    char* segments[MAX_SEGMENTS] = { 0 };
    char* token = strtok(temp, "-");

    while (token != NULL && segment_count < MAX_SEGMENTS) 
    {
        segments[segment_count++] = token;
        token = strtok(NULL, "-");
    }

    // Обрабатываем каждый сегмент с конца
    for (int i = segment_count - 1; i >= 0; i--) 
    {
        char letter = segments[i][0];
        char digit = segments[i][1];

        // Увеличиваем цифру
        char new_digit = increment_digit(digit);
        segments[i][1] = new_digit;

        if (new_digit > digit) break;

        // Увеличиваем букву
        char new_letter = increment_letter(letter);
        segments[i][0] = new_letter;

        if (new_letter > letter) break;

        // Если достигли начала и нужно добавить новый сегмент
        if (i == 0 && segment_count < MAX_SEGMENTS) 
        {
            for (int j = segment_count; j > 0; j--) 
            {
                segments[j] = segments[j - 1];
            }
            static char new_segment[3] = "A1";
            segments[0] = new_segment;
            segment_count++;
            break;
        }
    }

    // Формируем выходной идентификатор
    next_id[0] = '\0';
    for (int i = 0; i < segment_count; i++) 
    {
        if (i > 0) 
        {
            strncat(next_id, "-", buffer_size - strlen(next_id) - 1);
        }
        strncat(next_id, segments[i], buffer_size - strlen(next_id) - 1);
    }

    return true;
}

int main() {
    setlocale(LC_ALL, "Russian");
    char current_id[MAX_ID_LENGTH+1];
    char next_id[MAX_ID_LENGTH+1];

    printf("Введите текущий ID: ");
    if (fgets(current_id, sizeof(current_id), stdin)) 
    {
        current_id[strcspn(current_id, "\n")] = '\0';

        if (get_next_id(current_id, next_id, sizeof(next_id)))
        {
            printf("Следующий ID: %s\n", next_id);
        }
        else 
        {
            printf("Ошибка генерации следующего ID\n");
        }
    }

    return 0;
}