class Knapsack:
    def __init__(self, W, N, Wi, Vi, solve=True):
        self.W = W
        self.N = N
        self.Wi = Wi
        self.Vi = Vi
        self.dp = []
        if solve:
            self.solve()

    def __P(self, i, j):
        if i==0 or j==0:
            return 0, [0]*i
        if j < self.Wi[i]:
            return self.dp[i-1][j][0], self.dp[i-1][j][1]+[0]
        if self.dp[i-1][j][0] > self.dp[i-1][j-self.Wi[i]][0]+self.Vi[i]:
            return self.dp[i-1][j][0], self.dp[i-1][j][1]+[0]
        else:
            return self.dp[i-1][j-self.Wi[i]][0]+self.Vi[i], self.dp[i-1][j-self.Wi[i]][1]+[1]
    
    def solve(self):
        for i in range(self.N+1):
            self.dp.append([])
            for j in range(self.W+1):
                self.dp[i].append(self.__P(i, j))
    
    def get_solution(self):
        return self.dp[self.N][self.W]
