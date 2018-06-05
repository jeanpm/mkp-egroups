# Usage information
```
make debug

./cbga -i instances/chu-beasley/30.100-00

	-i string 	MKP instance file
	-n integer	population size
	-m option 	mutation type:
                0   GroupMutation::RANDOM_SWAP
                1   GroupMutation::RANDOM_SHUFFLE
                2   GroupMutation::SHUFFLE_ALL
                3   GroupMutation::NONE
	-d integer	decimal cases for rounding
                1 Round the efficiencies to the first decimal case
                2 Round the efficiencies to the second decimal case
	-t integer	mutation threshold
                - Activate the efficiency-group mutation if the number of
                  improving solutions found in a genaration is <= t


	-b float  	bias towards LP solution
                - bias the initial random population towards an optimal solution
                  for the relaxed LP problem
	-l float  	learning rate
                - defines how strongly the univariate probabilistic model is
                  updated according to the solutions found during the search
	-e long   	max evaluations
                - defines the maximum number of objective-function evaluations
                  tha can be performed before stoping the search.
```
