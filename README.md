# On Demand (state machine) Scheduler

## Goals

The objective of the ODS is to allow the creation of an infrastructure capable of executing functions or state machines cyclically, for a certain time and with a configurable execution rate.

In a scenario where there are already several ways to schedule cyclical tasks, what is the reason for creating yet another mechanism? It may be important to clarify this issue.

The purpose of the SDG is not exactly what we have in similar strategies, such as:

* **RTOSes**: an RTOS would certainly be a solution in this case. But this would come along with the computational cost and memory cost (volatile or not) of the solution. In this case, RTOS can present itself as a very powerful solution to solve a simple problem. Furthermore, since the original focus of the ODS project is to be activated on event-based and ultra-low power systems, it was also not desired for there to be a tick counter. A tickless RTOS would perhaps be an acceptable solution, although more complex.
* **Coroutines**: the author's initial intention was precisely to use coroutines. In C, implementations typically generate solutions where the local stack of the function that represents the coroutine may have problems being used (switch case-based solutions, for example). The author would like to have a solution where the stack is handled conventionally.
* **Schedulers**: a simple scheduler could be a solution (round-robin, non-preemptive, no time slicing). However, it was desired to be able to have different schedulers, with customized execution rates. However, the author did not find an open solution with an applicable license for commercial use.

In this way, ODS is a non-preemptive schedule, without priorities or time slicing, which executes functions in a cyclical manner (round-robin) and with a configurable rate. Multiple schedulers can be active at the same time.

## ODS features and operation

ODS only requires that a timer be created for each scheduler. No additional resources are required. The project also does not use any dynamic allocations. However, it is important that each scheduled task avoids running indefinitely.

ODS also provides sleep and event handling functions. However, the use of these functions requires some additional care since the call made will only be carried out after the current execution has ended. These points will be explained later.

Running tasks can also add other tasks to the current scheduler. However, this should only be done by tasks belonging to the current scheduler  to prevent concurrent accesses to the execution queue.

## Use cases

### Very simple scheduler with two tasks

[Source code](
https://github.com/marcelobarrosufu/ods//tests/main.c)



## Portability layer

For ODS to function, a thin layer of portability is necessary. The necessary services have been organized into modules and some of them may not be used depending on the characteristics of your system. These modules are written in driver format for greater flexibility.

In the hal/inc directory the necessary modules are listed:

* **hal_cpu.h**: basic services related to the processor, especially critical sections and platform initialization. In a microcontroller version where all timers associated with the scheduler execution have the same priority, only the hal_cpu_sleep_ms() call would need to be implemented (if you use it in the main loop).

* **hal_rtc.h**: only used to generate a timestamp when printing debug messages. It may not exist or be replaced by a common counter such as the systick.

* **hal_tmr.h**: this service is essential and should allow cyclic timers to be created and deleted.

As a reference, implementations were provided for Windows, Linux and STM32F411 (Black Pill) within the **port/[platform]** directory.






