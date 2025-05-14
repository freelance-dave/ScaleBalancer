# ScaleBalancer
[Balancing Scale](./Development-Problem.pdf): Given a CSV file representing interconnected weighing scales and masses, this application computes and reports the additional weights required on each side to achieve balance.

## Description
ScaleBalancer constructs a directed graph of nested scales and weights and then recursively computes the balancing mass required on each side of every scale. The application ensures correct balancing across arbitrarily deep configurations of nested scales.

### Time Complexity
- **Parsing and Graph Construction**: O(N), where N is the number of input lines (scales).
- **Balancing Computation**: O(N), assuming each scale is visited once in a depth-first traversal.

### Space Complexity
- **Graph Storage**: O(N), for storing the structure and relationships of all scales.
- **Call Stack**: O(D), where D is the maximum depth of nested scales (in recursive balance computation).

## Usage Example

### Input Format (CSV)
Each line of input defines a scale in the format:
```
scale_name,left_side,right_side
```
- A side can be either:
  - An integer weight (e.g., `5`)
  - The name of a different scale, not the current scale (e.g., `S2`)

### Example Input
```
S1,S2,1
S2,2,3
```
### Example Output
```
S1,0,6
S2,1,0
```
### Visual Representation
```
       S1
      /  \
    S2    1
    / \
   2   3
```
The diagram illustrates the structure of nested scales: S1 features a left side composed of scale S2 and a right side with a weight of 1 kg. Scale S2 itself has weights of 2 kg and 3 kg on its sides. The computed output indicates how much additional mass needs to be added to each side to balance the structure.

## Building and Testing
This project uses CMake for building and CTest for running unit tests.

### Build the Application
```bash
mkdir -p build
cd build
cmake .. -G Ninja -Wno-dev -DBUILD_TESTING=On
cmake --build .
```
This will generate the `scalebalancer` executable in the `build` directory.

### Run Unit Tests
After building the project, run the following command from the `build` directory:
```bash
ctest
```
Or, to see detailed output from the tests:
```bash
ctest --verbose
```

