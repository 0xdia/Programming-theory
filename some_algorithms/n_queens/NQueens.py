#!/usr/bin/env python3

"""
  This problem is solvable only for n=1 and n>=4
"""

class NQueens:
  def __init__(self, N):
    self.N = N
    self.Solution = []

  def Solve(self):
    positions = [-1 for _ in range(self.N)]
    columns = set()
    posDiag = set()
    negDiag = set()

    self.solve(positions, 0, columns, posDiag, negDiag)
    return self.Solution

  def solve(self, positions, row, columns, posDiag, negDiag):
    print(row)
    print(columns)
    if row > self.N:
      self.Solution = positions 
    else:
      for j in range(self.N):
        if j in columns or row+j in posDiag or row-j in negDiag:
          continue

        positions[row] = j
        columns.add(j)
        posDiag.add(row+j)
        negDiag.add(row-j)

        self.solve(positions, row+1, columns, posDiag, negDiag)

        if len(self.Solution) == self.N:
          break


instance = NQueens(4)
solution = instance.Solve()

grid = ['_'*4 for _ in range(4)]

for i in range(len(solution)):
  grid[i][j] = 'Q'

for i in range(4):
    print(grid[i])
