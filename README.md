HydroTerrain
CIS 660 Authoring Tool
Alice Yang | Cheng Xie
 
!!!ALWAYS TEST YOUR CODE BEFORE CHECKING IN. DO NOT SUBMIT CODE WITH ERRORS!!!
 
========== NOTES ==========
Documentation for this tool can be found in _README.txt in /HydroTerrain




========== TASKS ==========
Task List for Alpha: 
All of task 1.
Tasks 2.1, 2.3, 2.6
Tasks 3.1, 3.2

1. Build Framework (Alice)
1.1. Implement user interface in MEL [1d]
1.2. Create source images (river slope, terrain slope) [1d]
1.3. Implement command plug­in framework in C++ [1d]
1.3.1. Create code stubs for plug­in initialization, compute, etc.

2.1. Create data structures
2.1.1. Individual river nodes with priority index (Horton­Strahler’s number),
position and elevation [3d]
2.1.2. River node graph [1d]

2.3. Implement L­system expansion with initial candidate nodes for hierarchical
drainage network growth. [6d]

2.6. Integrate voronoi partitioning library [3d]

3.1. Take expanded nodes and implement voronoi partitioning (Cheng & Alice) [4d]
3.1.1. Connect the points for each voronoi cell to get a patch.
3.2. Assign height values to voronoi cells [1d]