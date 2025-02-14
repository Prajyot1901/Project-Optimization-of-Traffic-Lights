# Optimization Of Traffic Lights

## Overview

This project aims to predict traffic volume for the next 10 minutes and optimize green-light duration at intersections. Built entirely in C++, the program uses queuing theory to compute a cost function and applies a modified genetic algorithm to determine the optimal green-time distribution for each approach at the intersection.

## Key Components

**Traffic Volume Prediction:** Analyzes historical traffic data and predicts the volume of traffic for the next 10 minutes using a mathematical model.

**Signal Timing Optimization:** Utilizes queuing theory to calculate the cost function associated with different traffic signal timings. This helps in minimizing delays and improving traffic flow.

**Modified Genetic Algorithm:** Implements a novel modification to the genetic algorithm. In this approach, the number of child solutions is dynamically adjusted based on the diversity of parent solutions. If parent solutions are significantly different (poles apart), more child solutions are generated to explore a wider solution space.

## Features

**Accurate Traffic Prediction:** Provides precise predictions of traffic volume based on historical data, ensuring optimal traffic signal timing.

**Adaptive Signal Timing:** Adjusts the green-light duration dynamically to balance traffic flow, reducing wait times and improving intersection efficiency.

**Innovative Genetic Algorithm Modification:** Enhances the traditional genetic algorithm by dynamically adjusting the number of child solutions based on parent diversity, allowing for a more robust search of the solution space.

**Scalable C++ Implementation:** The entire project is implemented in C++, making it highly efficient and suitable for real-time applications.
Getting Started



## Prerequisites

**C++ Compiler:** Ensure you have a C++ compiler installed (e.g., GCC, Clang, MSVC).

**Follow the instructions below to download and set up the project.**

Now just copy the source file(.cpp) and all the input files(.csv) files in your computer. Compile the C++ code and run the .exe file
Don't forget to put input files in the same directory as the source code
