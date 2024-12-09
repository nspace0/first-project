#include "s21_cat.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

void print_file(FILE* file, flags current_flags, FILE* output);

int main(int argc, char** argv) {
  flags current_flags = parser(argc, argv);

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') continue;
    FILE* file = fopen(argv[i], "r");
    if (file == NULL) {
      fprintf(stderr, "%s %s No such file or Directory\n", argv[0], argv[i]);
      continue;
    }
    print_file(file, current_flags, stdout);
    fclose(file);
  }
  return 0;
}

flags parser(int argc, char** argv) {
  flags current_flags = {0};
  int opt;
  static struct option long_options[] = {{"number", 0, 0, 'n'},
                                         {"number-nonblank", 0, 0, 'b'},
                                         {"squeeze-blank", 0, 0, 's'},
                                         {0, 0, 0, 0}};
  while ((opt = getopt_long(argc, argv, "bnsveETt", long_options, 0)) != -1) {
    switch (opt) {
      case 'b':
        current_flags.b = 1;
        current_flags.n = 0;
        break;
      case 'n':
        current_flags.n = 1;
        break;

      case 's':
        current_flags.s = 1;
        break;
      case 'E':
        current_flags.e = 1;
        break;
      case 'e':
        current_flags.v = 1;
        current_flags.e = 1;
        break;
      case 'v':
        current_flags.v = 1;
        break;
      case 'T':
        current_flags.t = 1;
        break;
      case 't':
        current_flags.v = 1;
        current_flags.t = 1;
        break;
      case '?':
        perror("error");
        exit(1);
      default:
        break;
    }
  }
  return current_flags;
}
void print_file(FILE* file, flags current_flags, FILE* output) {
  int line_number = 1;
  int ch;
  int prev_ch = '\n';
  int blank_lines = 0;
  int nonblank_line_number = 1;

  while ((ch = fgetc(file)) != EOF) {
    if (current_flags.s && ch == '\n' && prev_ch == '\n') {
      blank_lines++;
      if (blank_lines > 1) continue;
    } else {
      blank_lines = 0;
    }

    if (prev_ch == '\n') {
      if (current_flags.b && ch != '\n') {
        fprintf(output, "%6d\t", nonblank_line_number++);

      } else if (current_flags.n) {
        fprintf(output, "%6d\t", line_number++);
      }
    }

    if (current_flags.e && ch == '\n') {
      fputc('$', output);
    }

    if (current_flags.t && ch == '\t') {
      fputs("^I", output);
    } else if (current_flags.v &&
               ((ch < 32 && ch != '\n' && ch != '\t') || ch == 127)) {
      if (ch == 127) {
        fputs("^?", output);
      } else {
        fprintf(output, "^%c", ch + 64);
      }
    } else if (current_flags.v && ch >= 128) {
      if (ch < 160) {
        fprintf(output, "M-^%c", ch - 128 + 64);
      } else {
        fprintf(output, "M-%c", ch - 128);
      }
    } else {
      fputc(ch, output);
    }

    prev_ch = ch;
  }
}
