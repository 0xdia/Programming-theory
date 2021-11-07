W = 4
N = 5
Wi = list(range(N+1))
Vi = list(range(N+1))

dp = []

def P(i, j):
    if j < Wi[i]:
        return dp[i-1][j][0], dp[i-1][j][1]+[0]
    else:
        if dp[i-1][j][0] > dp[i-1][j-Wi[i]][0]+Vi[i]:
            return dp[i-1][j][0], dp[i-1][j][1]+[0]
        elif dp[i-1][j][0] < dp[i-1][j-Wi[i]][0]+Vi[i]:
            return dp[i-1][j-Wi[i]][0]+Vi[i], dp[i-1][j-Wi[i]][1]+[1]
        else:
            return dp[i-1][j][0], dp[i-1][j][1]+[0], dp[i-1][j-Wi[i]][1]+[1]

for i in range(N+1):
    dp.append([])
    for j in range(W+1):
        if i==0 or j==0:
            dp[i].append((0, [0]*i))
        else:
            dp[i].append(P(i, j))

print(dp[N][W])
    