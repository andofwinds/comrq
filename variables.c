struct com_dat
{
  char name[32];
  int speed;
  int nspeed;
  char req_file_path[32];
  int timeout;
  char config_file_path[32];
  char custom_command[32];
  char current_command[32];

  char* saved_name;
  int saved_speed;

  struct termios tty;

  int fd;
};
