from knapsack import Knapsack

W = 4
N = 5
Wi = list(range(N+1))
Vi = list(range(N+1))

k = Knapsack(W, N, Wi, Vi)
print(k.get_solution())
