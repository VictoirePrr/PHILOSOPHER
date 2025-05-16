# Philosophers

A solution to the dining philosophers problem implemented in C using threads and mutexes.

## Overview

This project is an implementation of the classic dining philosophers problem, where multiple philosophers sit around a table, alternating between thinking, eating, and sleeping. The challenge is to prevent deadlocks and ensure fair resource (forks) distribution.

## Features

- Multiple philosophers dining simultaneously
- Thread-safe implementation using mutexes
- Death detection system
- Configurable number of philosophers and timing parameters
- Resource management to prevent deadlocks
- Performance optimized with even/odd philosopher scheduling

## Usage

### Compilation

```bash
make        # Compile the program
make clean  # Remove object files
make fclean # Remove object files and executable
make re     # Recompile the program
```

### Running the Program

```bash
./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
```

#### Parameters

- `number_of_philosophers`: The number of philosophers at the table
- `time_to_die`: Time in milliseconds until a philosopher dies from starvation
- `time_to_eat`: Time in milliseconds it takes to eat
- `time_to_sleep`: Time in milliseconds it takes to sleep
- `number_of_times_each_philosopher_must_eat` (optional): Program stops when all philosophers eat this many times

### Examples

```bash
./philo 5 800 200 200     # 5 philosophers, no meal limit
./philo 4 410 200 200 5   # 4 philosophers, must eat 5 times each
```

## Implementation Details

### Key Components

- **Mutex Management**: Handles fork access and prevents data races
- **Thread Safety**: Ensures safe concurrent execution
- **Death Monitoring**: Separate monitoring thread checks philosopher status
- **Resource Management**: Prevents deadlocks through proper fork allocation
- **Performance Optimization**: Even/odd philosopher scheduling reduces contention

### Data Structures

- `t_philo`: Philosopher-specific data
- `t_data`: Global program data
- `t_rules`: Simulation rules and parameters
- `t_fork`: Fork status and mutex
- `t_shared`: Shared state between threads

## Error Handling

The program includes robust error handling for:
- Invalid arguments
- Thread creation failures
- Mutex initialization failures
- Memory allocation failures
- Resource cleanup

## Contributing

Feel free to submit issues and enhancement requests ! :)

## License

This project is part of the 42 School curriculum.