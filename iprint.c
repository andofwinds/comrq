#include <stdio.h>
#include <errno.h>

enum colors
{
  BLACK = 30,
  RED,
  GREEN,
  YELLOW,
  BLUE,
  PURPLE,
  CYAN,
  WHITE,
};

void set_color(enum colors color)
{
  printf("\033[1;%im", color);
}

void reset_color()
{
  printf("\033[0m");
}

void ilog(char* msg)
{
  set_color(PURPLE);
  printf("[ LOG ] ");

  reset_color();
  printf("%s\n", msg);
}

void ierror(char* msg)
{
  set_color(RED);
  printf("[ FAIL ] ");

  reset_color();
  printf("%s: %s (code: %i)\n", msg, strerror(errno), errno);
}

void ierrorn(char* msg)
{
  set_color(RED);
  printf("[ FAIL ] ");

  reset_color();
  printf("Internal error: %s\n", msg);
}

void ifunerror(char* fun_name)
{
  set_color(RED);
  printf("[ FUN FAIL ] ");

  reset_color();
  printf("`%s()` caused error: %s (code: %i)\n", fun_name, strerror(errno), errno);
}

void ierrortoml(char* table, char* value)
{
  set_color(RED);
  printf("[ TOML FAIL ] ");

  reset_color();
  printf("Cannot find value `%s` in table `%s` of configuration file", value, table);
}

void iwarntomldef(char* table, char* value, char* default_value)
{
  set_color(YELLOW);
  printf("[ DEFAULT VALUE WARN ] ");

  reset_color();
  printf("Cannot find value `%s` in table `%s`. The default value `%s` will be used\n",
         value, table, default_value);
}
