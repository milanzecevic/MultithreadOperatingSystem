# Multithreaded Operating System Kernel (C/C++)

A custom-built operating system kernel developed as part of the Operating Systems course at the School of Electrical Engineering (ETF), University of Belgrade.

## 🛠 Project Overview
This project involves designing and implementing a functional OS kernel with a focus on core system management, concurrency, and performance. Developed using **C++** and **Assembly**, it demonstrates deep understanding of low-level system interactions.

## 🏗 Key Features
- **Thread Management:** Implemented a preemptive thread scheduler with context switching.
- **Synchronization Primitives:** Custom implementation of Semaphores and Mutexes to handle race conditions.
- **Memory Management:** Efficient memory allocation system for kernel and user-space processes.
- **Time Management:** Interrupt-driven timer for thread quantum management.
- **System Call API:** A robust interface allowing user-space applications to interact with kernel services.

## 💻 Tech Stack & Tools
- **Language:** C++, Assembly (x86)
- **IDE:** CLion
- **Toolchain:** GCC / GDB for debugging
- **Environment:** Bare-metal simulation

## 🔍 Implementation Highlights
- Solved complex synchronization issues using **semaphores**.
- Managed low-level **interrupt routines** and hardware abstraction layers.
- Optimized the **context switching** mechanism to reduce latency.
