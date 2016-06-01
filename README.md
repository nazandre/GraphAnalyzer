# GraphAnalyzer

GraphAnalyzer is a tool to efficiently analyze undirected and unweighted graphs.
It is efficient both in terms of computation time and memory usage. GraphAnalyzer is optimized for sparse graphs ($$m << n^2$$).

```tex
\sum_{x=0}^n f(x^2)
```

GraphAnalyzer can compute and display on demand the diameter, the radius, the center and the centroid of the input graph. Moreover, it can display the eccentricity and the farness of some nodes if specified in argument.

### Dependency

GraphAnalyzer requires OpenMP.

### Compilation

To compile GraphAnalyzer, go into the `build/` folder and enter `make`.

```
$ cd build/
$ make
```

### Usage

GraphAnalyzer works as following:

```
Usage: GraphAnalyzer -i <input GraphAnalyzer (.aj) file> [options]
Complexity: O(n) BFSes, i.e., O(n(n+m)) time and  O(n*#threads) memory space
Options:
-t <# threads>:     max number of threads to use (default: 8)
-d:                 fast diameter and radius computation 
                      does not support  parallelism
                      (disable center/centroid computation)
-n <node id>:       print the eccentricity and farness of the node with the
                      id "node id". Support multiple node queries (e.g.,
                      "-n 4 -n 6 -n 8" to display information about nodes
                      4, 6 and 8
-h:                 print this usage and exit
```

Below is a description of the GraphAnalyzer input (aj) file format.

```
<Number of nodes>
<nodeId1> <# of neighbors with graphId > 1 > <graphID neighbors ... >
<nodeId2> <# of neighbors with graphId > 2 > <graphID neighbors ... >
...
```

For instance, the GraphAnalyzer input file format for a line graph G of 3 nodes, such that G = 1<->2<->3, contains:

```
3
1 1 2
2 1 3
3 0
```

### Complexity

There are two modes of graph analysis: the default mode and the fast mode (option -d). While the default mode compute a wide variety of information (diameter, radius, center, centroid, ... ), the fast mode is faster but only computes the diameter and the radius of the input graph.

Both use Breadth-First Search (BFSes) technique. Let n be the number of nodes of the input graph, and m its number of edges. Performing a BFS from a node requires O(n+m) and O(n) memory space.  

The default mode of analysis performs exaclty n BFSes with at most t BFSes in parallel at a time. Thus, the default mode uses O(n(n+m)) time and O(n*t) memory space.

The fast mode analysis is inpired by the algorithm presented in [1]. Our implementation is actually a less performant version of [1]'s algorithm. In the worst-case, the fast mode requires O(n) BFSesas the default mode. However, as show in [1], it uses in practice only O(1) BFSes. The fast mode does not support parallelism. It has a worst-case complexity of O(n(n+m)) time and O(n) space.

[1] Borassi, M., Crescenzi, P., Habib, M., Kosters, W., Marino, A., & Takes, F. (2014, July). On the solvability of the six degrees of kevin bacon game. In Fun with Algorithms (pp. 52-63). Springer International Publishing.

### Example

Below is a full example of how to compile and use GraphAnalyzer. In this example, `graphs/example.aj` is analyzed.

```
$ cd build/
$ make
$ ./graphAnalyzer -i ../graphs/example.aj
Input: ../graphs/example.aj
Graph: 14 nodes, 15 edges
Compute: 
Breadth-First Search (BFS) based algorithm...
Parallel optimization using OpenMP. Max number of threads: 8
Diameter: 7
Radius: 4
Min Farness: 28
Center: 1, 2
Centroid: 2, 3
```