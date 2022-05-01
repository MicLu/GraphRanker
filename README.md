# GraphRanker

GraphRanker is a program that manages the ranking of weighted graphs.

The Graph at the start receives in input 2 parameters:  
-	**d** : number of nodes in the graphs
-	**k** : the length of the ranking

After these 2 configuration input parameters, the program waits for the 2 possible commands: 

## AggiungiGrafo: 
Allows to add a graph, by its Adjacency matrix

## TopK: 

Prints the ranking of all the graphs added by the start of the program before the command “TopK”.

Each graph it’s identified by its unique id (that is assigned during "AggiungiGrafo") and a number considered in the ranking which is the sum of the cost of all the minimum path from the node 0 to all the others nodes of the graph, considering 0 the value of the nodes not reachable by the node 0.

---

### Example

```
3,2
AggiungiGrafo
0,4,3
0,2,0
2,0,0
AggiungiGrafo
0,0,2
7,0,4
0,1,0
AggiungiGrafo
3,1,8
0,0,5
0,9,0
TopK
```

The program prints out: 
```
0 1 
```
the index of the graph number 0 and 1.
