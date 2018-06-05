#ifndef MODEL_H
#define MODEL_H

struct model {
	float* pr; // marginal probabilities
	float  lr; // learning rate (0,1)
	int    n; // number of random variables
	float* Q; // action-value estimates of real itens' profit
	int    k;
	float  lpvalue;

    model(float learning_rate, int size);

    void normalize01(float* eff);

    void bias_towards(float* target, float bias, bool normalize);

    // Update the marginal probability of random variable X_i
    void updatei(int* sol, int i, int m, float value, float target);

    void sample(int* sol);

    int count_free_variables();
};
#endif
