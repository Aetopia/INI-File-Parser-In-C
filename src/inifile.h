#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>

typedef struct keyvalue
{
    char *key;
    char *value;
} keyvalue;

typedef struct section
{
    long count;
    char *section;
    keyvalue *keyvalues;
} section;

typedef struct inifile
{
    long count;
    section *sections;
} inifile;

inifile iniread(char *filename);