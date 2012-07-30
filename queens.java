/* The eight queens solver */

int main(string[ ] args) {
  int N;
  //int[] row, col;
  int[] row;
  int[] col;
  int[][] d;
  
  N = 8;

  row = new int[N];
  col = new int[N];
  d = new int[][2];
  d[0] = new int[N+N-1];
  d[1] = new int[N+N-1];

  // fillIntArray is a contributed function
  fillIntArray(row, 0);
  fillIntArray(col, 0);
  fillIntArray(d[0], 0);
  fillIntArray(d[1], 0);
  
  search(N, row, col, d, 0);
  
  return 0;
}

int printBoard(int[] col) {
  //int i, j;
  int i;
  int j;

  for (i = 0; i < col.length; i = i+1) {
    for (j = 0; j < col.length; j = j+1) {
      if (col[i] == j) {
        printString(" O");
      } else {
        printString(" .");
      }
    }
    printChar('\n');
  }
  printChar('\n');
}

int search(int N, int[] row, int[] col, int[][] d, int c) {
  int r;
  
  if (c == N) {
    printBoard(col);
  } else {
    for (r = 0; r < N; r = r+1) {
      if (row[r] == 0 && d[0][r+c] == 0 && d[1][r+N-1-c] == 0) {
        row[r] = d[0][r+c] = d[1][r+N-1-c] = 1;
        col[c] = r;
        search(N, row, col, d, c+1);
        row[r] = d[0][r+c] = d[1][r+N-1-c] = 0;
      }
    }
  }
}

native int fillIntArray(int[] a, int v);
native int readInt();
native char readChar();
native string readString();
native string readLine();
native int printInt(int i);
native int printChar(char c);
native int printString(string s);
native int printLine(string s);
