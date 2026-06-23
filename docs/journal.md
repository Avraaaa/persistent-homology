# Journal

## 7th June, 2026

1. Currently I have implemented it using a naive implementation of boundary matrix.

2. For filtration, currently I am doing:

   ```txt
   maxPairwiseDistance = max over all i,j of dist(i,j)
   ```

   However in Ripser, if the user hasn't provided the cutoff distance then:

   ```txt
   enclosingRadius = min over i of max over j dist(i,j)
   ```

## 23rd June, 2026

1. Add 2 more print statements, one for only printing non zero length intervals, another for your betti numbers and the amount of simplices created for each d-dimension.

2. Add support for 3 simplices.

### Next goals

1. Have to investigate further into what the differences between these two approaches are, pros and cons, and the way to implement the latter.

2. Implement it using sparse boundary representation.



