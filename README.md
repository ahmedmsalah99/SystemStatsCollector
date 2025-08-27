# System Stats Collector

A lightweight system monitoring tool that collects and logs memory and CPU utilization statistics for processes. Designed for embedded systems with standard logging and configuration directories.

## Features

- Collects memory (in MB) and CPU (%) usage statistics
- Logs data in timestamp-delimited format
- Tracks top N processes by CPU utilization
- Configurable through easy-to-edit configuration files
- Uses standard system directories for logging and configuration
- Built on procps-ng library for reliable system statistics

## Log Format

The application creates log files named `name_pid.log` with the following format:
```
timestamp mem(MB) cpu(%)
```


## Dependencies

- procps-ng library (https://gitlab.com/procps-ng/procps)
- CMake (version 3.10 or higher)
- C++ compiler with C++11 support

## Installation

### Ubuntu/Debian
You can get procps using the link mentioned above and install it using

```bash
./autogen.sh
./configure
make
make install
```

### Building from Source

```bash
mkdir build
cd build
cmake ..
make stat_collector
```

## Configuration

The application reads configuration from files in the `conf/` directory. Configuration files should be placed in the system's standard configuration directory (typically `/etc/stat_collector/`).

defconfig is the default if the application is not installed.
### Configuration Options

- `LOGGING_DIR`: Path where log files will be stored
- `TOP_N`: Number of top CPU-intensive processes to track

## Usage

Run the compiled binary:
```bash
./stat_collector
```

Or if installed system-wide:
```bash
stat_collector
```

## Deployment on Embedded Systems

This application is designed to work on embedded devices with the following characteristics:

- Uses standard logging directories (typically `/var/log/`)
- Reads configuration from standard system directories
- Minimal resource footprint
- No unusual dependencies beyond procps-ng

## Support

For issues and questions, please check the documentation or create an issue in the project repository.