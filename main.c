#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <libgen.h>
#include <unistd.h>
#include "http_codes.h"

typedef struct cliflags
{
    unsigned int list: 1;
    unsigned int all: 1;
    unsigned int json: 1;
} cliflags;

void usage(char* progname)
{
    printf("Usage: %s [OPTIONS] [HTTP_CODE]\n", basename(progname));
    exit(1);
}

void help(char* progname)
{
    printf(
        "Usage: %s [OPTIONS] [HTTP_CODE]\n"
        "\t-h\thelp\n"
        "\t-l\tlist all HTTP status codes\n"
        "\t-a\tlist all HTTP status codes, text and descriptions\n"
        "\t-j\toutput in JSON format\n"
        ,basename(progname)
    );
    exit(0);
}

void print_json_string(const char *s)
{
    for (; *s; s++)
    {
        switch (*s)
        {
            case '"':  printf("\\\""); break;
            case '\\': printf("\\\\"); break;
            case '\n': printf("\\n");   break;
            case '\t': printf("\\t");   break;
            default:   putchar(*s);     break;
        }
    }
}

void print(cliflags flags, size_t i)
{
    if (flags.json)
    {
        printf("{\"code\": %ld, \"text\": \"%s\", \"description\": \"",
                http_status_code[i], 
                http_status_text[i]);
        print_json_string(http_status_description[i]);
        printf("\"}\n");
    }
    else
        printf("%ld %s\n%s\n", 
                http_status_code[i], 
                http_status_text[i],
                http_status_description[i]
        );
}

int main(int argc, char **argv)
{
    assert(sizeof http_status_code / sizeof(long) == HTTP_STATUS_CODE_NUM);

    int opt= -1;
    cliflags flags = {0};

    if (argc == 1) usage(argv[0]);
    while ((opt= getopt(argc, argv, "hlaj")) != -1)
    {
        switch(opt)
        {
            case 'h':
                help(argv[0]);
                break;
            case 'l':
                flags.list = 1;
                break;
            case 'a':
                flags.all = 1;
                break;
            case 'j':
                flags.json = 1;
                break;
            default:
                break;
        }
    }

    bool found = false;
    char *endptr = NULL;
    char *http_code = NULL;
    long http_code_long = -1;

    if (optind < argc)
    {
        http_code = argv[optind];
        errno = 0;
        http_code_long = strtol(http_code, &endptr, 10);

        if (http_code == endptr || errno != 0)
        {
            printf("%s: argument needs to be a number\n", argv[0]);
            exit(1);
        }
    }

    for (size_t i=0; i<HTTP_STATUS_CODE_NUM; i++)
    {
        if (flags.list && !flags.all) printf("%ld\n", http_status_code[i]);

        if ((!flags.list && http_code_long == http_status_code[i]) || flags.all)
        {
            found = true;
            print(flags, i);
        }
    }
    
    if (!found && !flags.list) puts("Not a standard HTTP status code");

    return 0;
}
