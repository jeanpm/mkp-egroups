#include "model.h"
#include "global.h"

#include <algorithm>

model::model(float learning_rate, int size) : lr(learning_rate), n(size)
{
    k = 0;
    pr = new float[size];
    Q  = new float[size];
    std::fill_n(pr, n, 0.5);
}

void model::normalize01(float* eff)
{
    auto minmax = std::minmax_element(eff, eff + n);
    // .first is the minimum value, .second is the maximum value
    for (int i = 0; i < n; i++)
        eff[i] = (eff[i] - *minmax.first)/(*minmax.second - *minmax.first);
}

void model::bias_towards(float* target, float bias, bool normalize=false)
{
    // Normalize between 0 and 1 if required. Disabled by default.
    if (normalize) normalize01(target);
    // The difference is added to pr[i] towards target[i]. The step size is
    // controled by the bias, 0...1
    for (int i = 0; i < n; i++)
        pr[i] += bias * (target[i] - pr[i]);
}

// Update the marginal probability of random variable X_i
void model::updatei(int* sol, int i, int m, float value, float target)
{
    pr[i] += lr * float(sol[i] - pr[i]);
}

void model::sample(int* sol)
{
    for (int j = 0; j < n; j++)
        sol[j] = (int) (unif(gen) < pr[j]);
}

int model::count_free_variables()
{
    const float MIN = 0.001;
    int free_variables = 0;
    for (int i = 0; i < n; i++) {
        if (pr[i] < (1.0-MIN) && pr[i] > MIN) free_variables++;
    }
    return free_variables;
}
