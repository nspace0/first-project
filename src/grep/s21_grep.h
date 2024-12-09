#ifndef GREP_H
#define GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Переименованные структуры
typedef struct {
  bool e;
  bool i;
  bool v;
  bool c;
  bool l;
  bool n;
  bool h;
  bool s;
  bool f;
  bool o;
  bool err;
} opts_t;

typedef struct {
  char **data;
  unsigned int size;
  unsigned int cap;
} tmpls_t;

// Переименованные функции
void check_capacity(tmpls_t *);
void parse_args(int, char **, opts_t *, tmpls_t *);
void read_f(int, char **, const opts_t *, tmpls_t *, int);
void print(FILE *, const char *, const opts_t *, const tmpls_t *, int, int);
unsigned long process_ln(char *, const char *, const opts_t *, const tmpls_t *,
                         int, unsigned long, int);
void print_ln(const char *, const char *, const opts_t *, unsigned long, int);
int search_ln(const char *, const opts_t *, const tmpls_t *, int,
              regmatch_t **);
bool stop_check(const opts_t *);
void handle_e(opts_t *, tmpls_t *);
void handle_f(opts_t *, tmpls_t *);
void tmpls_parser(tmpls_t *);
void fileTmpls_parser(tmpls_t *, opts_t *);
int get_flags(const opts_t *);
void process_f(const char *, const opts_t *, const tmpls_t *, int, int);
int filter_matches(const regmatch_t *matches, int n_matches);

#endif  // GREP_H
