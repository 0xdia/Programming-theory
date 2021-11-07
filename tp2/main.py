W = 5
N = 5
Wi = list(range(N+1))
Vi = list(range(N+1))

dp = []

def P(i, j):
    if i==0 or j==0:
        return 0, [0]*i
    if j < Wi[i]:
        return dp[i-1][j][0], dp[i-1][j][1]+[0]
    if dp[i-1][j][0] > dp[i-1][j-Wi[i]][0]+Vi[i]:
        return dp[i-1][j][0], dp[i-1][j][1]+[0]
    else:
        return dp[i-1][j-Wi[i]][0]+Vi[i], dp[i-1][j-Wi[i]][1]+[1]

for i in range(N+1):
    dp.append([])
    for j in range(W+1):
        dp[i].append(P(i, j))

print(dp[N][W])
    