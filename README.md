# Single-Lane Bridge Simulation with Multi-threading

## Overview

This project simulates a **single-lane bridge crossing problem** with multiple farmers arriving from two directions: **North** and **South**.
The bridge allows only one direction at a time, with a maximum of **3 consecutive farmers** crossing from the same side before giving priority to the opposite side.

The simulation is implemented in **C** using **POSIX threads (pthreads)** and **mutexes/condition variables** to manage synchronization.

---

## Features

* **Multi-threaded simulation:** Each farmer is represented by a separate thread.
* **Mutual exclusion:** Only farmers from one direction can cross at a time.
* **Fairness constraint:** Maximum of 3 consecutive farmers allowed from the same side.
* **Random crossing time:** Farmers take 1–3 seconds to cross.
* **Safe random number generation:** `rand()` is protected with a mutex for thread safety.
* **Verbose logging:** Prints messages for each farmer waiting, starting, and finishing crossing.

---

## Requirements

* GCC or any C compiler supporting **pthreads**
* POSIX-compliant system (Linux/macOS/WSL on Windows)

---

## How to Compile

```bash
gcc -o bridge_simulation bridge_simulation.c -lpthread
```

---

## How to Run

```bash
./bridge_simulation
```

You will see output similar to:

```
Northbound farmer 0 waiting...
Southbound farmer 0 started crossing. [1 on bridge]
Southbound farmer 1 waiting...
Northbound farmer 0 started crossing. [1 on bridge]
...
```

This output shows which farmers are **waiting**, **crossing**, and **finished crossing**, along with the number of farmers currently on the bridge.

---

## Implementation Details

### Threads

* Each farmer is represented by a **thread** (`pthread_create`).
* Threads are created alternately from **North** and **South**, but may vary due to randomness in creation timing.

### Synchronization

* **Mutex `lock`:** Ensures safe access to shared variables.
* **Condition variables `north_cond` and `south_cond`:** Used to signal waiting threads when they can cross.
* **Variables:**

  * `north_on_bridge` / `south_on_bridge` — number of farmers currently on the bridge.
  * `north_waiting` / `south_waiting` — number of farmers waiting.
  * `turn` — whose turn is it to cross.
  * `consecutive_count` — count of consecutive farmers from the same direction.

### Crossing Rules

1. Only one direction on the bridge at a time.
2. Maximum **3 consecutive farmers** allowed from the same side before giving priority to the other side if waiting.
3. Farmers wait if the bridge is occupied by the opposite direction or the fairness rule applies.

### Randomness

* Crossing time is random: **1–3 seconds**.
* `usleep` introduces random creation intervals between threads: **50–150 ms**.
* `safe_rand()` ensures thread-safe random number generation.

---

## Customization

* **Number of farmers per side:** Modify `NUM_FARMERS` in the code.
* **Maximum consecutive crossings:** Modify `MAX_CONSECUTIVE` in the code.
* **Crossing time:** Adjust `sleep(safe_rand(1, 3))` for faster/slower simulation.

---

## License

This project is open source and available under the **MIT License**.

---

## Author

**Mohammad Javad Dehfroz**

* GitHub: [MohammadJavadQM](https://github.com/MohammadJavadQM)
* Email: [mohammadjavad.m@gmail.com](mailto:mohammadjavad.m@gmail.com)
