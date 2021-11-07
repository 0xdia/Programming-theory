from knapsack import Knapsack

W = int(input("Enter the capacity W="))
N = int(input("Enter the number of items N="))
Wi = list(map(int, input(f"Enter {N} comma separated values for items weights:\n").split(',')))
Vi = list(map(int, input(f"Enter {N} comma separated values for items values:\n").split(',')))
try:
    assert len(Wi)==N and len(Vi)==N
    k = Knapsack(W, N, Wi, Vi)
    print("The solution is ", k.get_solution())
except:
    print("Wrong number of weights or values")
