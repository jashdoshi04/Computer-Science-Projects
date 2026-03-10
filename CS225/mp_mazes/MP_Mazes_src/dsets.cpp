/* Your code here! */

#include "dsets.h"

// add unconnected nodes of a length specified by function 
// since they are unconnected all will root nodes and their values will "-1" as per up tree rules 

void DisjointSets::addelements(int num) {
    for(int i  = 0 ; i < num ; i++) {
        disjoint_set.push_back(-1);
    }
}

// next we have the find fucntion 
// we use a recursive call to , since tecnically this a tree 
// I think we can stop if we find a negative element since technically that will give the parameter elements 
// else we just recurse this function on the value of the element we found and go up the tree 
// hence uptree - cool revelation makes so much more sense now 

int DisjointSets::find(int elem) {
    if(disjoint_set[elem] < 0) {
        // if we got negative we return 
        return elem;
    } else {
        // else we compressed tree or in other words go up the tree by assigning the value at that elem to recursive call 
    int root = find(disjoint_set[elem]);
    disjoint_set[elem] = root;
    return root;
    }
}

// now we do set union 
// if tie we return nothing 
// if either one bigger we make the bigger on the bigger size hence union it size 1 + size 2 
// and we update the root of the smaller and bigger sets to new sizes 
void DisjointSets :: setunion(int a, int b) {
   
    int rootA = find(a);
    int rootB = find(b);
    int size_ab = -1*(size(a) + size(b));
    if (size(a) >= size(b)) {
        disjoint_set.at(rootA) = size_ab;
        disjoint_set.at(rootB) = rootA;
    } else {
        disjoint_set.at(rootB) = size_ab;
        disjoint_set.at(rootA) = rootB;
    }
}

int DisjointSets::size(int elem){
    // we return the size of the set 
    // find the root node 
    int element = find(elem);
    // find the value of that node and multiply it by -1 to get the right size 
    int size_vec = -1*(disjoint_set[element]);
    return size_vec;
}
int DisjointSets::getValue(int elem) const {
    // 
    return disjoint_set[elem];
}
