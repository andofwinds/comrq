# comrq
### a simple way to serial port polling

## About
comrq is a simple serial port poller developed by andofwinds with some features like live-reloading.

## Compile
```bash
git clone https://github.com/andofwinds/comrq.git
cd comrq
gcc comrq.c -ltoml -o comrq
```

Dependency `toml` is required.

## Available options
#### -C
Specify the command that will be sent instead of commands from *.csq file

#### -c
Specify the configuration file (default is `./comrq_configuration.toml`)


## Syntax

### Command sequence file (.csq)
.csq syntax is simple.
Every command must start from new line.

Also you can place the comment by typing `#` symbol on the start of line.

### Configuration file (.toml)
This is the most common .toml v1.0 syntax. Every field you need explained by comments in file.
