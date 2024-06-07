// A program that makes requests to COM port with defined frequency
// ---
// made by andofwinds for Den :з

#define BUF_SIZE 256

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include "include/toml.c"

// A simple C utils
#include "libutils.c"

// better library for log / warn / error messages
#include "iprint.c"

// A com_dat structure
#include "variables.c"

// Port utils
#include "cport.c"



struct com_dat* read_toml(struct com_dat* comport_data, char* config_path)
{

  FILE* fp;
  char errbuf[BUF_SIZE];

  fp = fopen(config_path, "r");
  if (!fp)
  {
    ierror("Failed to open configuration file");
    exit(1);
  }

  toml_table_t* tconf = toml_parse_file(fp, errbuf, sizeof(errbuf));
  fclose(fp);

  if (!tconf)
  {
    ierror("Cannot parse configuration file");
    exit(1);
  }

  // [serial]
  toml_table_t* tserial = toml_table_in(tconf, "serial");
  if (!tserial)
  { 
    ierrorn("Missing [serial]! Cannot continue without this");
    exit(1);
  }

  toml_datum_t name = toml_string_in(tserial, "name");
  if (!name.ok)
  {
    ierrortoml("serial", "name");
    exit(1);
  }

  toml_datum_t speed = toml_int_in(tserial, "speed");
  if (!speed.ok)
  {
    ierrortoml("serial", "name");
    exit(1);
  }

  toml_datum_t timeout = toml_int_in(tserial, "timeout");
  if (!timeout.ok)
  {
    ierrortoml("serial", "timeout");
    exit(1);
  }

  // [metadata]
  toml_table_t* tmetadata = toml_table_in(tconf, "metadata");

  toml_datum_t req_file_path = toml_string_in(tmetadata, "req_file_path");
  if (!req_file_path.ok)
  {
    iwarntomldef("metadata", "req_file_path", "requests.csq");

    //comport_data.req_file_path = "requests.csq";
    strcpy(comport_data->req_file_path, "requests.csq");
  } else
  {
    strcpy(comport_data->req_file_path, req_file_path.u.s);  
  }

  // align name to 32 chars
  char namebuf[32];
  strcpy(namebuf, name.u.s);
 

  comport_data->timeout = timeout.u.i;
  
  strcpy(comport_data->config_file_path, config_path);

  if (strcmp(comport_data->name, namebuf) != 0)
  {
    strcpy(comport_data->name, name.u.s);
    comport_reload(comport_data);
  }

  if (comport_data->speed != speed.u.i)
  {
    comport_data->speed = speed.u.i;

    comport_data->nspeed = get_bod(comport_data->speed);
    comport_reload(comport_data);
  }

  return comport_data;
}

void multiple_commands_loop(struct com_dat comport_data)
{
  char buf[BUF_SIZE];

  int lines_count = 0;

  lines_count = read_csq(comport_data.req_file_path);
  if (lines_count == -1)
  {
    ierrorn("Cannot read Comment Sequence file");
  }

  while (1)
  {
    for (int p = 0; p < lines_count; p++)
    {
      memset(&buf, '\0', sizeof(buf));

      // live reload config & sequence
      read_toml(&comport_data, comport_data.config_file_path);
      lines_count = read_csq(comport_data.req_file_path);
      if (lines_count == -1)
      {
        ierrorn("Cannot read Comment Sequence file");
      }

      get_line(comport_data.req_file_path, p, buf);
      strcpy(comport_data.current_command, buf);

      // parse comment and empty line
      if (buf[0] == '#' || buf[0] == '\n')
      {
        continue;
      }


      send_request(comport_data, buf);

      printf("#%s%s\n\n", comport_data.current_command, buf);

      cwait(comport_data.timeout);
    }
  }
}

void single_command_loop(struct com_dat comport_data)
{
  char buf[BUF_SIZE];
  while (1)
  {
    read_toml(&comport_data, comport_data.config_file_path);

    strcpy(comport_data.current_command, comport_data.custom_command);

    //write(comport_data.fd, comport_data.custom_command, sizeof(comport_data.custom_command));
    send_request(comport_data, buf);

    /*memset(&buf, '\0', sizeof(buf)); // clear buffer

    read(comport_data.fd, &buf, sizeof(buf));*/
    printf("#%s\n%s\n\n", comport_data.current_command, buf);
    cwait(comport_data.timeout);
  }
}

int main(int argc, char* argv[]) {
  struct com_dat comport;

  char* config_path = "comrq_configuration.toml";

  

  int c;
  while ((c = getopt(argc, argv, "C:c:")) != -1)
  {
    switch (c)
    {
      case 'C':
        ilog("Processing with custom request command!");
        //comport.custom_command = optarg;
        strcpy(comport.custom_command, optarg);
        break;

      case 'c':
        ilog("Processing with custom configuration file");
        config_path = optarg;
    }
  } 

  read_toml(&comport, config_path);

  //comport.nspeed = find_index(com_speeds, sizeof(com_speeds), comport.speed); 
  comport.nspeed = get_bod(comport.speed);

  comport_reload(&comport);

  ilog("Initializing COM port...");
  // --- UNSTABLE CODE ---
  cwait(3);
  // --- REMOVE OR EDIT THIS LINE IF CODE TOO SLOW OR ARDUINO CANNOT RECEIVE DATA
  ilog("Everything ready! Processing main loop.");

  // --- MAIN LOOP ---
  

  if (comport.custom_command[0] == '\0') // multiple commands from .csq file
  {
    multiple_commands_loop(comport);
  } else // single command from -C argument
  {
    single_command_loop(comport);
  }


  return 0;
}
