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

### Next goals

1. Have to investigate further into what the differences between these two approaches are, pros and cons, and the way to implement the latter.

2. Implement it using sparse boundary representation.

3. Print only the important barcodes and Betti numbers.
