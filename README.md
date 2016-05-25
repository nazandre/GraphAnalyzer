# GraphAnalyzer

GraphAnalyzer is a tool to efficiently analyze undirected and unweighted graphs. For now, it only print the diameter of the graph in input.

### Dependencies

GraphAnalyzer requires OpenMP and Boost libraries.

In order to install Boost library on Ubuntu:

```
$ sudo apt-get install libboost-all-dev
```

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
Options:
-h : print this usage and exit
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

### Example

Below is a full example of how to compile and use GraphAnalyzer. In this example, `graphs/example.aj` is analyzed.

```
$ cd build/
$ make
$ ./graphAnalyzer -i ../graphs/example.aj 
Input: ../graphs/example.aj
Graph: 14 nodes
Computing all pairs shortest paths...
Breadth-First Search (BFS) based algorithm...
Parallel optimization using OpenMP...
Diameter: 7
```