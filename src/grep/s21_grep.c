#define _GNU_SOURCE

#include "s21_grep.h"

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: grep [OPTION]... PATTERNS [FILE]...\n");
    fprintf(stderr, "Try 'grep --help' for more information.\n");
    return 0;
  }

  opts_t opts = {false};
  tmpls_t tmpls;
  tmpls.size = 0;
  tmpls.cap = 2;
  tmpls.data = malloc(sizeof(char*) * tmpls.cap);
  parse_args(argc, argv, &opts, &tmpls);
  if (!opts.err) {
    int flags = get_flags(&opts);
    read_f(argc, argv, &opts, &tmpls, flags);
  }
  for (unsigned i = 0; i < tmpls.size; ++i) {
    free(tmpls.data[i]);
  }
  free(tmpls.data);
  return 0;
}

void print(FILE* fp, const char* fname, const opts_t* opts,
           const tmpls_t* tmpls, int flags, int fcount) {
  unsigned long lnum = 1, cnt = 0;
  char* line = NULL;
  size_t len = 0;

  while (getline(&line, &len, fp) != -1) {
    if (line[strlen(line) - 1] == '\n') {
      line[strlen(line) - 1] = '\0';
    }
    cnt += process_ln(line, fname, opts, tmpls, flags, lnum, fcount);
    ++lnum;
  }
  free(line);

  if (opts->l && cnt > 0) {
    printf("%s\n", fname);
  } else if (opts->c && !opts->l) {
    if (!opts->h && fcount > 1) {
      printf("%s:", fname);
    }
    printf("%lu\n", cnt);
  }
}

unsigned long process_ln(char* line, const char* fname, const opts_t* opts,
                         const tmpls_t* tmpls, int flags, unsigned long lnum,
                         int fcount) {
  bool matched = false;
  unsigned long cnt = 0;
  regmatch_t* matches = NULL;
  int n_matches = search_ln(line, opts, tmpls, flags, &matches);

  matched = (n_matches > 0);

  if (!opts->l && !opts->c && !opts->o &&
      ((!opts->v && matched) || (opts->v && !matched))) {
    print_ln(line, fname, opts, lnum, fcount);
  }

  if (opts->o && n_matches > 0) {
    n_matches =
        filter_matches(matches, n_matches);  // Вызов функции filter_matches
    for (int i = 0; i < n_matches; ++i) {
      if (!opts->h && fcount > 1) {
        printf("%s:", fname);
      }
      if (opts->n) {
        printf("%lu:", lnum);
      }
      for (int j = matches[i].rm_so; j < matches[i].rm_eo; ++j) {
        printf("%c", line[j]);
      }
      printf("\n");
    }
  }

  if (matches) free(matches);
  cnt += (!opts->v && matched) || (opts->v && !matched);

  return cnt;
}

int filter_matches(const regmatch_t* matches, int n_matches) {
  // Пример реализации фильтрации совпадений
  // Возвращает количество уникальных совпадений
  int unique_matches = 0;
  for (int i = 0; i < n_matches; ++i) {
    if (matches[i].rm_so != matches[i].rm_eo) {
      ++unique_matches;
    }
  }
  return unique_matches;
}

void print_ln(const char* line, const char* fname, const opts_t* opts,
              unsigned long lnum, int fcount) {
  if (!opts->h && fcount > 1) {
    printf("%s:", fname);
  }
  if (opts->n) {
    printf("%lu:", lnum);
  }
  printf("%s\n", line);
}

int search_ln(const char* line, const opts_t* opts, const tmpls_t* tmpls,
              int flags, regmatch_t** matches_out) {
  int matches_cap = 2;
  int n_matches = 0;
  *matches_out = (regmatch_t*)calloc(matches_cap, sizeof(regmatch_t));
  regex_t regex;
  bool matched = false;

  for (unsigned int i = 0; i < tmpls->size; ++i) {
    int offset = 0;
    regcomp(&regex, tmpls->data[i], flags);
    regmatch_t match[1];
    while (regexec(&regex, line + offset, 1, match, 0) == 0) {
      matched = true;
      if (stop_check(opts)) {
        regfree(&regex);
        return matched;
      }
      if (n_matches + 1 == matches_cap) {
        matches_cap *= 2;
        *matches_out = (regmatch_t*)realloc(*matches_out,
                                            matches_cap * sizeof(regmatch_t));
      }
      match[0].rm_so += offset;
      match[0].rm_eo += offset;
      (*matches_out)[n_matches++] = match[0];
      offset = match[0].rm_eo;
    }
    regfree(&regex);
  }

  return n_matches;
}

bool stop_check(const opts_t* opts) {
  return !opts->o || (opts->v && opts->o) || opts->l || opts->c;
}

void parse_args(int argc, char** argv, opts_t* opts, tmpls_t* tmpls) {
  char* short_opts = "e:ivclnhsf:o";
  int opt;

  while ((opt = getopt(argc, argv, short_opts)) != -1) {
    switch (opt) {
      case 'e':
        handle_e(opts, tmpls);
        break;
      case 'i':
        opts->i = true;
        break;
      case 'v':
        opts->v = true;
        break;
      case 'c':
        opts->c = true;
        break;
      case 'l':
        opts->l = true;
        break;
      case 'n':
        opts->n = true;
        break;
      case 'h':
        opts->h = true;
        break;
      case 's':
        opts->s = true;
        break;
      case 'f':
        handle_f(opts, tmpls);
        break;
      case 'o':
        opts->o = true;
        break;
      default:
        opts->err = true;
        break;
    }
  }
}

void handle_e(opts_t* opts, tmpls_t* tmpls) {
  opts->e = true;
  tmpls_parser(tmpls);
}

void handle_f(opts_t* opts, tmpls_t* tmpls) {
  opts->f = true;
  fileTmpls_parser(tmpls, opts);
}

void read_f(int argc, char** argv, const opts_t* opts, tmpls_t* tmpls,
            int flags) {
  if (!opts->e && !opts->f) {
    int len = strlen(argv[optind]);
    tmpls->data[tmpls->size] = calloc(len + 1, sizeof(char));
    strcpy(tmpls->data[tmpls->size++], argv[optind++]);
  }

  for (int i = optind; i < argc; ++i) {
    process_f(argv[i], opts, tmpls, flags, argc - optind);
  }
}

void process_f(const char* fname, const opts_t* opts, const tmpls_t* tmpls,
               int flags, int fcount) {
  FILE* fp = fopen(fname, "r");
  if (fp == NULL) {
    if (!opts->s) {
      fprintf(stderr, "grep: %s: No such file or directory\n", fname);
    }
    return;
  }
  print(fp, fname, opts, tmpls, flags, fcount);
  fclose(fp);
}

int get_flags(const opts_t* opts) {
  int flags = 0;
  if (opts->e) {
    flags |= REG_EXTENDED;
  }
  if (opts->i) {
    flags |= REG_ICASE;
  }
  return flags;
}

void tmpls_parser(tmpls_t* tmpls) {
  check_capacity(tmpls);
  int len = strlen(optarg);
  tmpls->data[tmpls->size] = calloc(len + 1, sizeof(char));
  strcpy(tmpls->data[tmpls->size], optarg);
  tmpls->size++;
}

void fileTmpls_parser(tmpls_t* tmpls, opts_t* opts) {
  FILE* fp = fopen(optarg, "r");
  if (fp == NULL) {
    fprintf(stderr, "grep: %s: No such file or directory\n", optarg);
    opts->err = true;
  } else {
    unsigned long len = 0;
    int size;
    char* line = NULL;
    while ((size = getline(&line, &len, fp)) != -1) {
      check_capacity(tmpls);
      int len = strlen(line);
      tmpls->data[tmpls->size] = calloc(len + 1, sizeof(char));
      strcpy(tmpls->data[tmpls->size], line);
      if (tmpls->data[tmpls->size][len - 1] == '\n') {
        tmpls->data[tmpls->size][len - 1] = '\0';
      }
      tmpls->size++;
    }
    free(line);
    fclose(fp);
  }
}

void check_capacity(tmpls_t* tmpls) {
  if (tmpls->size == tmpls->cap) {
    tmpls->cap *= 2;
    tmpls->data = realloc(tmpls->data, sizeof(char*) * tmpls->cap);
  }
}
