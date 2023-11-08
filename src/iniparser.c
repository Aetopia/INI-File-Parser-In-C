#include "iniparser.h"

char *strtrim(char *src)
{
    char *str = src;
    int length = strlen(str);

    while (isspace(str[length - 1]))
        str[--length] = 0;
    while (*str && isspace(*str))
    {
        ++str;
        --length;
    }

    str[strlen(str) + 1] = '\0';
    return memmove(src, str, length + 1);
}

char **strtokens(long *count, char *delimiter, char *src)
{
    char **tokens = NULL;
    char *str = strcpy(alloca(strlen(src) + 1), src);
    char *token = strtok(str, delimiter);
    char *tokencpy = NULL;

    while (token)
    {
        tokencpy = strcpy(malloc(strlen(token) + 1), token);
        if (strlen(strtrim(tokencpy)))
        {
            (tokens = realloc(tokens, sizeof(char *) * (*count += 1)))[*count - 1] =
                realloc(tokencpy, strlen(tokencpy) + 1);
        }
        else
            free(tokencpy);
        token = strtok(NULL, "\n");
    }

    return tokens;
}

char *fileread(char *filename)
{
    char *dstbuf = NULL;
    long size = 0;

    FILE *file = fopen(filename, "r");

    if (!file)
        return NULL;

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    dstbuf = calloc(sizeof(char), size + 1);

    rewind(file);
    fread(dstbuf, size, sizeof(char), file);
    fclose(file);

    return dstbuf;
}

inifile iniparsetokens(char **tokens, long count)
{
    inifile file = {};
    section *section = NULL;
    for (long i = 0; i < count; i += 1)
    {
        if (tokens[i][0] == '[' &&
            tokens[i][strlen(tokens[i]) - 1] == ']')
        {
            file.count += 1;
            file.sections =
                realloc(file.sections, sizeof(section) + (file.count - 1));
            file.sections[file.count - 1].section =
                strcpy(malloc(strlen(tokens[i]) + 1), tokens[i]);

            if (strlen(file.sections[file.count - 1].section))
            {

                file.sections[file.count - 1].section[0] = ' ';
                file.sections[file.count - 1].section[strlen(tokens[i]) - 1] = '\0';

                file.sections[file.count - 1].section =
                    realloc(strtrim(file.sections[file.count - 1].section),
                            strlen(file.sections[file.count - 1].section) + 1);

                section = &file.sections[file.count - 1];
                section->count = 0;
                section->keyvalues = NULL;
                printf("[%s]\n", section->section);
            }
            else
            {
                free(file.sections);
                free(file.sections[file.count - 1].section);
                if (file.count)
                    file.count -= 1;
            }
        }
        else if (strchr(tokens[i], '=') &&
                 tokens[i][0] != '[' &&
                 tokens[i][strlen(tokens[i]) - 1] != ']' &&
                 section)
        {
            char *str = strcpy(_alloca(strlen(tokens[i]) + 1), tokens[i]),
                 *token = strtok(str, "="),
                 *key = NULL,
                 *value = NULL;

            if (token)
                key = strcpy(malloc(strlen(token) + 1), token);
            token = strtok(NULL, "=");
            if (token)
                value = strcpy(malloc(strlen(token) + 1), token);

            if (key && value)
            {
                printf("%s = %s\n", key, value);
                section->count += 1;
                section->keyvalues = realloc(section->keyvalues, sizeof(keyvalue) * section->count);
                section->keyvalues[section->count - 1].key = key;
                section->keyvalues[section->count - 1].value = value;
            }
            else
            {
                free(key);
                free(value);
            }
        }
    }

    return file;
}

inifile iniread(char *filename)
{
    char *dstbuf = fileread(filename);
    long count = 0;
    char **tokens = strtokens(&count, "\n", dstbuf);
    inifile file = iniparsetokens(tokens, count);

    for (long i = 0; i < count; i += 1)
        free(tokens[i]);
    free(tokens);
    free(dstbuf);

    return file;
}