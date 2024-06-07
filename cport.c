int read_csq(char* filename)
{
  char buf[BUF_SIZE];

  FILE* fp = fopen(filename, "r");
  if (!fp)
  {
    ierror("Cannot open .csq file");
    exit(1);
  }

  int lines_count = 0;
  while (fgets(buf, sizeof(buf), fp)) lines_count++;
  fclose(fp);

  return lines_count;
}

void get_line(char* filename, int line_num, char* buf)
{

  char fb[BUF_SIZE];
  FILE* fp = fopen(filename, "r");
  memset(fb, '\0', sizeof(fb));
  memset(buf, '\0', sizeof(buf));
  
  for (int i = 0; i <= line_num; i++)
  {
    fgets(fb, sizeof(fb), fp);
  }

  fclose(fp);
  strcpy(buf, fb);
}

void send_request(struct com_dat comport_data, char* buf)
{
  char ib[BUF_SIZE];
  write(comport_data.fd, comport_data.current_command, sizeof(comport_data.current_command));


  read(comport_data.fd, &ib, sizeof(ib));

  strcpy(buf, ib);
}

void comport_reload(struct com_dat* comport_data)
{
  close(comport_data->fd);
  comport_data->fd = open(comport_data->name, O_RDWR);
  struct termios tty;

  if (comport_data->fd < 0) {
    ierror("Could not open port");
    exit(1);
  }

  if (tcgetattr(comport_data->fd, &tty) != 0) {
    ifunerror("tcgetattr");
    exit(1);
  }

  // setting up tty
  tty.c_cflag &= ~PARENB;  // disable parity
  tty.c_cflag &= ~CSTOPB;  // one stop bit
  tty.c_cflag &= ~CSIZE;   // clear all size bits
  tty.c_cflag |= CS8;      // byte size => 8 bit
  tty.c_cflag &= ~CRTSCTS; // disable hardware flow control
  tty.c_cflag |= CREAD | CLOCAL;

  tty.c_lflag &= ~ICANON; // disable canonical mode
  tty.c_lflag &= ~ECHO;   // disable echo
  tty.c_lflag &= ~ECHOE;
  tty.c_lflag &= ~ECHONL;
  tty.c_lflag &= ~ISIG; // disable signals

  tty.c_iflag &= ~(IXON | IXOFF | IXANY);
  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

  tty.c_oflag &= ~OPOST;
  tty.c_oflag &= ~ONLCR;

  tty.c_cc[VTIME] = 10; // 1 second
  tty.c_cc[VMIN] = 0;

  // set speed
  cfsetispeed(&tty, comport_data->nspeed);
  cfsetospeed(&tty, comport_data->nspeed);

  if (tcsetattr(comport_data->fd, TCSANOW, &tty) != 0) {
    ifunerror("tcsetattr");
    exit(1);
  }
}

int com_speeds[] = 
    {
      0,
      50,
      75,
      110,
      134,
      150,
      200,
      300,
      600,
      1200,
      1800,
      2400,
      4800,
      9600,
      19200,
      38400,
      57600,
      115200,
      230400,
    }; 

int get_bod(int speed)
{
  return find_index(com_speeds, sizeof(com_speeds), speed);
}
