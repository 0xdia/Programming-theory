/*********************************************************************/
/*********************************************************************/
/***							      	   ***/
/***         	TP MinMax / Module TPGO / ESI 2016-2017            ***/
/***							      	   ***/
/*** Il est demandé d'améliorer la fonction estim(...) ci-dessous. ***/
/*** Son rôle est d'évaluer la qualité d'une configuration donnée. ***/
/*** Elle doit retourner une valeur dans ]-100 , +100[		   ***/
/***							      	   ***/
/*********************************************************************/
/*********************************************************************/

#include <limits.h>  // pour INT_MAX
#include <stdio.h>
#include <stdlib.h>

#define MAX +1  // Niveau Maximisant
#define MIN -1  // Niveau Minimisant
#define INFINI INT_MAX

// Type d'une configuration formant l'espace de recherche
struct config {
    char mat[8][8];           // l'échiquier
    int val;                  // estimation de la config
    char xrN, yrN, xrB, yrB;  // positions des rois Noir et Blanc
    char roqueN, roqueB;      // indicateurs de roque pour N et B :
                              // 'g'rand roque non réalisable
                              // 'p'etit roque non réalisable
                              // 'n'on réalisable des 2 cotés
                              // 'r'éalisable (valeur initiale)
                              // 'e'ffectué
};

// vecteurs des déplacements par type de piece ...
// cavalier
int dC[8][2] = {{-2, +1}, {-1, +2}, {+1, +2}, {+2, +1}, {+2, -1}, {+1, -2}, {-1, -2}, {-2, -1}};
// fou (indices impairs), tour (indices pairs), reine et roi (indices pairs et impairs) ...
int D[8][2] = {{+1, 0}, {+1, +1}, {0, +1}, {-1, +1}, {-1, 0}, {-1, -1}, {0, -1}, {+1, -1}};

// evalue avec alpha beta la configuration 'conf' du joueur 'mode' en descendant de 'niv' niveaux
int minmax_ab(struct config conf, int mode, int niv, int min, int max);

void generer_succ(struct config conf, int mode, struct config T[], int *n);

int better_estim(struct config conf);

/* Copie la configuration c1 dans c2  */
void copier(struct config *c1, struct config *c2) {
    int i, j;

    for (i = 0; i < 8; i++)
        for (j = 0; j < 8; j++)
            c2->mat[i][j] = c1->mat[i][j];

    c2->val = c1->val;
    c2->xrB = c1->xrB;
    c2->yrB = c1->yrB;
    c2->xrN = c1->xrN;
    c2->yrN = c1->yrN;

    c2->roqueB = c1->roqueB;
    c2->roqueN = c1->roqueN;
}  // copier

/* Teste si les conf c1 et c2 sont égales */
int egal(char c1[8][8], char c2[8][8]) {
    int i, j;

    for (i = 0; i < 8; i++)
        for (j = 0; j < 8; j++)
            if (c1[i][j] != c2[i][j]) return 0;
    return 1;
}  // egal

/***********************************************************/
/*********** Partie:  Evaluations et Estimations ***********/
/***********************************************************/

#define PAWN 0
#define KNIGHT 1
#define BISHOP 2
#define ROOK 3
#define QUEEN 4
#define KING 5

/* board representation */
#define WHITE 0
#define BLACK 1

#define WHITE_PAWN (2 * PAWN + WHITE)
#define BLACK_PAWN (2 * PAWN + BLACK)
#define WHITE_KNIGHT (2 * KNIGHT + WHITE)
#define BLACK_KNIGHT (2 * KNIGHT + BLACK)
#define WHITE_BISHOP (2 * BISHOP + WHITE)
#define BLACK_BISHOP (2 * BISHOP + BLACK)
#define WHITE_ROOK (2 * ROOK + WHITE)
#define BLACK_ROOK (2 * ROOK + BLACK)
#define WHITE_QUEEN (2 * QUEEN + WHITE)
#define BLACK_QUEEN (2 * QUEEN + BLACK)
#define WHITE_KING (2 * KING + WHITE)
#define BLACK_KING (2 * KING + BLACK)
#define EMPTY (BLACK_KING + 1)

int matching(char pc) {
    switch (pc) {
        case 'p':
            return WHITE_PAWN;
        case 'c':
            return WHITE_KNIGHT;
        case 'f':
            return WHITE_BISHOP;
        case 't':
            return WHITE_ROOK;
        case 'n':
            return WHITE_QUEEN;
        case 'r':
            return WHITE_KING;
        case -'p':
            return BLACK_PAWN;
        case -'c':
            return BLACK_KNIGHT;
        case -'f':
            return BLACK_BISHOP;
        case -'t':
            return BLACK_ROOK;
        case -'n':
            return BLACK_QUEEN;
        case -'r':
            return BLACK_KING;
        case 0:
            return EMPTY;
        default:
            return EMPTY;
    }
}

#define PCOLOR(p) ((p)&1)

#define FLIP(sq) ((sq) ^ 56)
#define OTHER(side) ((side) ^ 1)

/* side2win (machine) is BLACK but minmax expects cost while better_estim returns
evaluation not cost. A solution is to inverse side2win*/
int side2move = OTHER(BLACK);

int mg_value[6] = {82, 337, 365, 477, 1025, 0};
int eg_value[6] = {94, 281, 297, 512, 936, 0};

/* piece/sq tables */
/* values from Rofchade: http://www.talkchess.com/forum3/viewtopic.php?f=2&t=68311&start=19 */

int mg_pawn_norm = 703 - (-201);
int mg_pawn_table[64] = {
      0,   0,   0,   0,   0,   0,  0,   0,
     98, 134,  61,  95,  68, 126, 34, -11,
     -6,   7,  26,  31,  65,  56, 25, -20,
    -14,  13,   6,  21,  23,  12, 17, -23,
    -27,  -2,  -5,  12,  17,   6, 10, -25,
    -26,  -4,  -4, -10,   3,   3, 33, -12,
    -35,  -1, -20, -23, -15,  24, 38, -22,
      0,   0,   0,   0,   0,   0,  0,   0,
};

int eg_pawn_norm = 1274 - (-51);
int eg_pawn_table[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,
    178, 173, 158, 134, 147, 132, 165, 187,
     94, 100,  85,  67,  56,  53,  82,  84,
     32,  24,  13,   5,  -2,   4,  17,  17,
     13,   9,  -3,  -7,  -7,  -8,   3,  -1,
      4,   7,  -6,   1,   0,  -5,  -1,  -8,
     13,   8,   8,  10,  13,   0,   2,  -7,
      0,   0,   0,   0,   0,   0,   0,   0,
};
int mg_knight_norm = 213 - (-274);
int mg_knight_table[64] = {
    -167, -89, -34, -49,  61, -97, -15, -107,
     -73, -41,  72,  36,  23,  62,   7,  -17,
     -47,  60,  37,  65,  84, 129,  73,   44,
      -9,  17,  19,  53,  37,  69,  18,   22,
     -13,   4,  16,  13,  28,  19,  21,   -8,
     -23,  -9,  12,  10,  19,  17,  25,  -16,
     -29, -53, -12,  -3,  -1,  18, -14,  -19,
    -105, -21, -58, -33, -17, -28, -19,  -23,
};

int eg_knight_norm = 47 - (-162);
int eg_knight_table[64] = {
    -58, -38, -13, -28, -31, -27, -63, -99,
    -25,  -8, -25,  -2,  -9, -25, -24, -52,
    -24, -20,  10,   9,  -1,  -9, -19, -41,
    -17,   3,  22,  22,  22,  11,   8, -18,
    -18,  -6,  16,  25,  16,  17,   4, -18,
    -23,  -3,  -1,  15,  10,  -3, -20, -22,
    -42, -20, -10,  -5,  -2, -20, -23, -44,
    -29, -51, -23, -15, -22, -18, -50, -64,
};

int mg_bishop_norm = 109 - (-124);
int mg_bishop_table[64] = {
    -29,   4, -82, -37, -25, -42,   7,  -8,
    -26,  16, -18, -13,  30,  59,  18, -47,
    -16,  37,  43,  40,  35,  50,  37,  -2,
     -4,   5,  19,  50,  37,  37,   7,  -2,
     -6,  13,  13,  26,  34,  12,  10,   4,
      0,  15,  15,  15,  14,  27,  18,  10,
      4,  15,  16,   0,   7,  21,  33,   1,
    -33,  -3, -14, -21, -13, -12, -39, -21,
};

int eg_bishop_norm = 33 - (-51);
int eg_bishop_table[64] = {
    -14, -21, -11,  -8, -7,  -9, -17, -24,
     -8,  -4,   7, -12, -3, -13,  -4, -14,
      2,  -8,   0,  -1, -2,   6,   0,   4,
     -3,   9,  12,   9, 14,  10,   3,   2,
     -6,   3,  13,  19,  7,  10,  -3,  -9,
    -12,  -3,   8,  10, 13,   3,  -7, -15,
    -14, -18,  -7,  -1,  4,  -9, -15, -27,
    -23,  -9, -23,  -5, -9, -16,  -5, -17,
};

int mg_rook_norm = 147 - (-116);
int mg_rook_table[64] = {
     32,  42,  32,  51, 63,  9,  31,  43,
     27,  32,  58,  62, 80, 67,  26,  44,
     -5,  19,  26,  36, 17, 45,  61,  16,
    -24, -11,   7,  26, 24, 35,  -8, -20,
    -36, -26, -12,  -1,  9, -7,   6, -23,
    -45, -25, -16, -17,  3,  0,  -5, -33,
    -44, -16, -20,  -9, -1, 11,  -6, -71,
    -19, -13,   1,  17, 16,  7, -37, -26,
};

int eg_rook_norm = 33 - (-36);
int eg_rook_table[64] = {
    13, 10, 18, 15, 12,  12,   8,   5,
    11, 13, 13, 11, -3,   3,   8,   3,
     7,  7,  7,  5,  4,  -3,  -5,  -3,
     4,  3, 13,  1,  2,   1,  -1,   2,
     3,  5,  8,  4, -5,  -6,  -8, -11,
    -4,  0, -5, -1, -7, -12,  -8, -16,
    -6, -6,  0,  2, -9,  -9, -11,  -3,
    -9,  2,  3, -1, -5, -13,   4, -20,
};

int mg_queen_norm = 59 - (-50);
int mg_queen_table[64] = {
    -28,   0,  29,  12,  59,  44,  43,  45,
    -24, -39,  -5,   1, -16,  57,  28,  54,
    -13, -17,   7,   8,  29,  56,  47,  57,
    -27, -27, -16, -16,  -1,  17,  -2,   1,
     -9, -26,  -9, -10,  -2,  -4,   3,  -3,
    -14,   2, -11,  -2,  -5,   2,  14,   5,
    -35,  -8,  11,   2,   8,  15,  -3,   1,
     -1, -18,  -9,  10, -15, -25, -31, -50,
};

int eg_queen_norm = 58 - (-41);
int eg_queen_table[64] = {
     -9,  22,  22,  27,  27,  19,  10,  20,
    -17,  20,  32,  41,  58,  25,  30,   0,
    -20,   6,   9,  49,  47,  35,  19,   9,
      3,  22,  24,  45,  57,  40,  57,  36,
    -18,  28,  19,  47,  31,  34,  39,  23,
    -16, -27,  15,   6,   9,  17,  10,   5,
    -22, -23, -30, -16, -16, -23, -36, -32,
    -33, -28, -22, -43,  -5, -32, -20, -41,
};

int mg_king_norm = 29 - (-65);
int mg_king_table[64] = {
    -65,  23,  16, -15, -56, -34,   2,  13,
     29,  -1, -20,  -7,  -8,  -4, -38, -29,
     -9,  24,   2, -16, -20,   6,  22, -22,
    -17, -20, -12, -27, -30, -25, -14, -36,
    -49,  -1, -27, -39, -46, -44, -33, -51,
    -14, -14, -22, -46, -44, -30, -15, -27,
      1,   7,  -8, -64, -43, -16,   9,   8,
    -15,  36,  12, -54,   8, -28,  24,  14,
};

int eg_king_norm = 45 - (-74);
int eg_king_table[64] = {
    -74, -35, -18, -18, -11,  15,   4, -17,
    -12,  17,  14,  17,  17,  38,  23,  11,
     10,  17,  23,  15,  20,  45,  44,  13,
     -8,  22,  24,  27,  26,  33,  26,   3,
    -18,  -4,  21,  24,  27,  23,   9, -11,
    -19,  -3,  11,  21,  23,  16,   7,  -9,
    -27, -11,   4,  13,  14,   4,  -5, -17,
    -53, -34, -21, -11, -28, -14, -24, -43
};

int mg_normalizer;
int eg_normalizer;

int *mg_pesto_table[6] =
    {
        mg_pawn_table,
        mg_knight_table,
        mg_bishop_table,
        mg_rook_table,
        mg_queen_table,
        mg_king_table};

int *eg_pesto_table[6] =
    {
        eg_pawn_table,
        eg_knight_table,
        eg_bishop_table,
        eg_rook_table,
        eg_queen_table,
        eg_king_table};

int gamephaseInc[12] = {0, 0, 1, 1, 1, 1, 2, 2, 4, 4, 0, 0};
int mg_table[12][64];
int eg_table[12][64];

void init_estim_tables() {
    int pc, p, sq;
    for (p = PAWN, pc = WHITE_PAWN; p <= KING; pc += 2, p++) {
        for (sq = 0; sq < 64; sq++) {
            mg_table[pc][sq] = mg_value[p] + mg_pesto_table[p][sq];
            eg_table[pc][sq] = eg_value[p] + eg_pesto_table[p][sq];
            mg_table[pc + 1][sq] = mg_value[p] + mg_pesto_table[p][FLIP(sq)];
            eg_table[pc + 1][sq] = eg_value[p] + eg_pesto_table[p][FLIP(sq)];
        }
    }
    mg_normalizer = mg_pawn_norm + mg_value[KNIGHT] + mg_knight_norm + mg_value[BISHOP] + mg_bishop_norm +
    mg_value[ROOK] + mg_rook_norm + mg_value[QUEEN] + mg_queen_norm + mg_value[KING] + mg_king_norm;
    eg_normalizer = eg_value[PAWN] + eg_pawn_norm + eg_value[KNIGHT] + eg_knight_norm + eg_value[BISHOP] + eg_bishop_norm +
    eg_value[ROOK] + eg_rook_norm + eg_value[QUEEN] + eg_queen_norm + eg_value[KING] + eg_king_norm;
}

/* Teste s'il n'y a aucun coup possible dans la configuration conf */
int AucunCoupPossible(struct config conf) {
    // ... A completer pour les matchs nuls
    // ... vérifier que generer_succ retourne 0 configurations filles ...
    int n;
    struct config T[100];
    // Vérifie les coups possibles pour le joueur B (MAX)
    generer_succ(conf, MAX, T, &n);
    // S'il n'y a pas, vérifie les coups possibles pour le joueur N (MIN)
    if (!n) generer_succ(conf, MIN, T, &n);
    return n ? 0 : 1;

}  // AucunCoupPossible

/* Teste si conf représente une fin de partie et retourne dans 'cout' la valeur associée */
int feuille(struct config conf, int *cout) {
    //int i, j, rbx, rnx, rby, rny;

    *cout = 0;

    // Si victoire pour les Noirs cout = -100
    if (conf.xrB == -1) {
        *cout = -100;
        return 1;
    }

    // Si victoire pour les Blancs cout = +100
    if (conf.xrN == -1) {
        *cout = +100;
        return 1;
    }

    // Si Match nul cout = 0
    if (conf.xrB != -1 && conf.xrN != -1 && AucunCoupPossible(conf))
        return 1;

    // Sinon ce n'est pas une config feuille
    return 0;

}  // feuille

/* Retourne une estimation de la configuration conf */
int estim(struct config conf) {
    // ********************************************************
    // Remplacer ce code par une estimation de meilleur qualité
    // ********************************************************

    if(side2move == OTHER(BLACK)) return better_estim(conf);

    int i, j, ScrQte;
    int pionB = 0, pionN = 0, cfB = 0, cfN = 0, tB = 0, tN = 0, nB = 0, nN = 0;

    // parties : nombre de pièces et occupation du centre
    for (i = 0; i < 8; i++)
        for (j = 0; j < 8; j++) {
            switch (conf.mat[i][j]) {
                case 'p':
                    pionB++;
                    break;
                case 'c':
                case 'f':
                    cfB++;
                    break;
                case 't':
                    tB++;
                    break;
                case 'n':
                    nB++;
                    break;

                case -'p':
                    pionN++;
                    break;
                case -'c':
                case -'f':
                    cfN++;
                    break;
                case -'t':
                    tN++;
                    break;
                case -'n':
                    nN++;
                    break;
            }
        }

    // Somme pondérée de pièces de chaque joueur.
    // Le facteur 1.4 pour ne pas sortir de l'intervalle ]-100 , +100[
    ScrQte = (-(pionB + cfB * 6 + tB * 8 + nB * 20) + (pionN + cfN * 6 + tN * 8 + nN * 20)) * 1.4;

    if (ScrQte > 95) ScrQte = 95;    // pour rétrécir l'intervalle à
    if (ScrQte < -95) ScrQte = -95;  // ]-95 , +95[ car ce n'est qu'une estimation
    return ScrQte;

}  // estim

/* Retourne une estimation de la configuration conf */
int better_estim(struct config conf) {
    // ********************************************************
    // Remplacer ce code par une estimation de meilleur qualité -/
    // ********************************************************
    int mg[2];
    int eg[2];
    int gamePhase = 0;

    mg[WHITE] = 0;
    mg[BLACK] = 0;
    eg[WHITE] = 0;
    eg[BLACK] = 0;

    /* evaluate each piece */
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int pc = matching(conf.mat[i][j]);
            int sq = (7 - i) * 8 + j;
            if (pc != EMPTY) {
                mg[PCOLOR(pc)] += mg_table[pc][sq];
                eg[PCOLOR(pc)] += eg_table[pc][sq];
                gamePhase += gamephaseInc[pc];
            }
        }
    }

    /* tapered eval */
    int mgScore = mg[side2move] - mg[OTHER(side2move)];
    int egScore = eg[side2move] - eg[OTHER(side2move)];
    int mgPhase = gamePhase;
    if (mgPhase > 24) mgPhase = 24; /* in case of early promotion */
    int egPhase = 24 - mgPhase;
    return (mgScore * mgPhase / mg_normalizer * 100 + egScore * egPhase / eg_normalizer * 100) / 24;
}  // better_estim

/***********************************************************/
/*********** Partie:  Génération des Successeurs ***********/
/***********************************************************/

/* Génère dans T les configurations obtenues à partir de conf lorsqu'un pion atteint la limite de l'échiq */
void transformPion(struct config conf, int a, int b, int x, int y, struct config T[], int *n) {
    int signe = +1;
    if (conf.mat[a][b] < 0) signe = -1;
    copier(&conf, &T[*n]);
    T[*n].mat[a][b] = 0;
    T[*n].mat[x][y] = signe * 'n';
    (*n)++;
    copier(&conf, &T[*n]);
    T[*n].mat[a][b] = 0;
    T[*n].mat[x][y] = signe * 'c';
    (*n)++;
    copier(&conf, &T[*n]);
    T[*n].mat[a][b] = 0;
    T[*n].mat[x][y] = signe * 'f';
    (*n)++;
    copier(&conf, &T[*n]);
    T[*n].mat[a][b] = 0;
    T[*n].mat[x][y] = signe * 't';
    (*n)++;

}  // transformPion

// Vérifie si la case (x,y) est menacée par une des pièces du joueur 'mode'
int caseMenaceePar(int mode, int x, int y, struct config conf) {
    int i, j, a, b, stop;

    // menace par le roi ...
    for (i = 0; i < 8; i += 1) {
        // traitement des 8 directions paires et impaires
        a = x + D[i][0];
        b = y + D[i][1];
        if (a >= 0 && a <= 7 && b >= 0 && b <= 7)
            if (conf.mat[a][b] * mode == 'r') return 1;
    }  // for

    // menace par cavalier ...
    for (i = 0; i < 8; i++)
        if (x + dC[i][0] <= 7 && x + dC[i][0] >= 0 && y + dC[i][1] <= 7 && y + dC[i][1] >= 0)
            if (conf.mat[x + dC[i][0]][y + dC[i][1]] * mode == 'c')
                return 1;

    // menace par pion ...
    if ((x - mode) >= 0 && (x - mode) <= 7 && y > 0 && conf.mat[x - mode][y - 1] * mode == 'p')
        return 1;
    if ((x - mode) >= 0 && (x - mode) <= 7 && y < 7 && conf.mat[x - mode][y + 1] * mode == 'p')
        return 1;

    // menace par fou, tour ou reine ...
    for (i = 0; i < 8; i += 1) {
        // traitement des 8 directions paires et impaires
        stop = 0;
        a = x + D[i][0];
        b = y + D[i][1];
        while (!stop && a >= 0 && a <= 7 && b >= 0 && b <= 7)
            if (conf.mat[a][b] != 0)
                stop = 1;
            else {
                a = a + D[i][0];
                b = b + D[i][1];
            }
        if (stop) {
            if (conf.mat[a][b] * mode == 'f' && i % 2 != 0) return 1;
            if (conf.mat[a][b] * mode == 't' && i % 2 == 0) return 1;
            if (conf.mat[a][b] * mode == 'n') return 1;
        }
    }  // for

    // sinon, aucune menace ...
    return 0;

}  // caseMenaceePar

/* Génere dans T tous les coups possibles de la pièce (de couleur N) se trouvant à la pos x,y */
void deplacementsN(struct config conf, int x, int y, struct config T[], int *n) {
    int i, j, a, b, stop;

    switch (conf.mat[x][y]) {
        // mvmt PION ...
        case -'p':
            //***printf("PION N à la pos (%d,%d) \n", x,y);
            if (x > 0 && conf.mat[x - 1][y] == 0) {  // avance d'une case
                copier(&conf, &T[*n]);
                T[*n].mat[x][y] = 0;
                T[*n].mat[x - 1][y] = -'p';
                (*n)++;
                if (x == 1) transformPion(conf, x, y, x - 1, y, T, n);
            }
            if (x == 6 && conf.mat[5][y] == 0 && conf.mat[4][y] == 0) {  // avance de 2 cases
                copier(&conf, &T[*n]);
                T[*n].mat[6][y] = 0;
                T[*n].mat[4][y] = -'p';
                (*n)++;
            }
            if (x > 0 && y > 0 && conf.mat[x - 1][y - 1] > 0) {  // mange à droite (en descendant)
                copier(&conf, &T[*n]);
                T[*n].mat[x][y] = 0;
                T[*n].mat[x - 1][y - 1] = -'p';
                // cas où le roi adverse est pris...
                if (T[*n].xrB == x - 1 && T[*n].yrB == y - 1) {
                    T[*n].xrB = -1;
                    T[*n].yrB = -1;
                }

                (*n)++;
                if (x == 1) transformPion(conf, x, y, x - 1, y - 1, T, n);
            }
            if (x > 0 && y < 7 && conf.mat[x - 1][y + 1] > 0) {  // mange à gauche (en descendant)
                copier(&conf, &T[*n]);
                T[*n].mat[x][y] = 0;
                T[*n].mat[x - 1][y + 1] = -'p';
                // cas où le roi adverse est pris...
                if (T[*n].xrB == x - 1 && T[*n].yrB == y + 1) {
                    T[*n].xrB = -1;
                    T[*n].yrB = -1;
                }

                (*n)++;
                if (x == 1) transformPion(conf, x, y, x - 1, y + 1, T, n);
            }
            break;

        // mvmt CAVALIER ...
        case -'c':
            for (i = 0; i < 8; i++)
                if (x + dC[i][0] <= 7 && x + dC[i][0] >= 0 && y + dC[i][1] <= 7 && y + dC[i][1] >= 0)
                    if (conf.mat[x + dC[i][0]][y + dC[i][1]] >= 0) {
                        copier(&conf, &T[*n]);
                        T[*n].mat[x][y] = 0;
                        T[*n].mat[x + dC[i][0]][y + dC[i][1]] = -'c';
                        // cas où le roi adverse est pris...
                        if (T[*n].xrB == x + dC[i][0] && T[*n].yrB == y + dC[i][1]) {
                            T[*n].xrB = -1;
                            T[*n].yrB = -1;
                        }

                        (*n)++;
                    }
            break;

        // mvmt FOU ...
        case -'f':
            for (i = 1; i < 8; i += 2) {
                // traitement des directions impaires (1, 3, 5 et 7)
                stop = 0;
                a = x + D[i][0];
                b = y + D[i][1];
                while (!stop && a >= 0 && a <= 7 && b >= 0 && b <= 7) {
                    if (conf.mat[a][b] < 0)
                        stop = 1;
                    else {
                        copier(&conf, &T[*n]);
                        T[*n].mat[x][y] = 0;
                        if (T[*n].mat[a][b] > 0) stop = 1;
                        T[*n].mat[a][b] = -'f';
                        // cas où le roi adverse est pris...
                        if (T[*n].xrB == a && T[*n].yrB == b) {
                            T[*n].xrB = -1;
                            T[*n].yrB = -1;
                        }

                        (*n)++;
                        a = a + D[i][0];
                        b = b + D[i][1];
                    }
                }  // while
            }      // for
            break;

        // mvmt TOUR ...
        case -'t':
            for (i = 0; i < 8; i += 2) {
                // traitement des directions paires (0, 2, 4 et 6)
                stop = 0;
                a = x + D[i][0];
                b = y + D[i][1];
                while (!stop && a >= 0 && a <= 7 && b >= 0 && b <= 7) {
                    if (conf.mat[a][b] < 0)
                        stop = 1;
                    else {
                        copier(&conf, &T[*n]);
                        T[*n].mat[x][y] = 0;
                        if (T[*n].mat[a][b] > 0) stop = 1;
                        T[*n].mat[a][b] = -'t';
                        // cas où le roi adverse est pris...
                        if (T[*n].xrB == a && T[*n].yrB == b) {
                            T[*n].xrB = -1;
                            T[*n].yrB = -1;
                        }

                        if (conf.roqueN != 'e' && conf.roqueN != 'n') {
                            if (x == 7 && y == 0 && conf.roqueN != 'p')
                                T[*n].roqueN = 'g';  // le grand roque ne sera plus possible
                            else if (x == 7 && y == 0)
                                T[*n].roqueN = 'n';  // ni le grand roque ni le petit roque ne seront possibles
                            if (x == 7 && y == 7 && conf.roqueN != 'g')
                                T[*n].roqueN = 'p';  // le petit roque ne sera plus possible
                            else if (x == 7 && y == 7)
                                T[*n].roqueN = 'n';  // ni le grand roque ni le petit roque ne seront possibles
                        }

                        (*n)++;
                        a = a + D[i][0];
                        b = b + D[i][1];
                    }
                }  // while
            }      // for
            break;

        // mvmt REINE ...
        case -'n':
            for (i = 0; i < 8; i += 1) {
                // traitement des 8 directions paires et impaires
                stop = 0;
                a = x + D[i][0];
                b = y + D[i][1];
                while (!stop && a >= 0 && a <= 7 && b >= 0 && b <= 7) {
                    if (conf.mat[a][b] < 0)
                        stop = 1;
                    else {
                        copier(&conf, &T[*n]);
                        T[*n].mat[x][y] = 0;
                        if (T[*n].mat[a][b] > 0) stop = 1;
                        T[*n].mat[a][b] = -'n';
                        // cas où le roi adverse est pris...
                        if (T[*n].xrB == a && T[*n].yrB == b) {
                            T[*n].xrB = -1;
                            T[*n].yrB = -1;
                        }

                        (*n)++;
                        a = a + D[i][0];
                        b = b + D[i][1];
                    }
                }  // while
            }      // for
            break;

        // mvmt ROI ...
        case -'r':
            // vérifier possibilité de faire un roque ...
            if (conf.roqueN != 'n' && conf.roqueN != 'e') {
                if (conf.roqueN != 'g' && conf.mat[7][1] == 0 && conf.mat[7][2] == 0 && conf.mat[7][3] == 0)
                    if (!caseMenaceePar(MAX, 7, 1, conf) && !caseMenaceePar(MAX, 7, 2, conf) &&
                        !caseMenaceePar(MAX, 7, 3, conf) && !caseMenaceePar(MAX, 7, 4, conf)) {
                        // Faire un grand roque ...
                        copier(&conf, &T[*n]);
                        T[*n].mat[7][4] = 0;
                        T[*n].mat[7][0] = 0;
                        T[*n].mat[7][2] = -'r';
                        T[*n].xrN = 7;
                        T[*n].yrN = 2;
                        T[*n].mat[7][3] = -'t';
                        T[*n].roqueN = 'e';  // aucun roque ne sera plus possible à partir de cette config
                        (*n)++;
                    }
                if (conf.roqueN != 'p' && conf.mat[7][5] == 0 && conf.mat[7][6] == 0)
                    if (!caseMenaceePar(MAX, 7, 4, conf) && !caseMenaceePar(MAX, 7, 5, conf) &&
                        !caseMenaceePar(MAX, 7, 6, conf)) {
                        // Faire un petit roque ...
                        copier(&conf, &T[*n]);
                        T[*n].mat[7][4] = 0;
                        T[*n].mat[7][7] = 0;
                        T[*n].mat[7][6] = -'r';
                        T[*n].xrN = 7;
                        T[*n].yrN = 6;
                        T[*n].mat[7][5] = -'t';
                        T[*n].roqueN = 'e';  // aucun roque ne sera plus possible à partir de cette config
                        (*n)++;
                    }
            }

            // vérifier les autres mouvements du roi ...
            for (i = 0; i < 8; i += 1) {
                // traitement des 8 directions paires et impaires
                a = x + D[i][0];
                b = y + D[i][1];
                if (a >= 0 && a <= 7 && b >= 0 && b <= 7)
                    if (conf.mat[a][b] >= 0) {
                        copier(&conf, &T[*n]);
                        T[*n].mat[x][y] = 0;
                        T[*n].mat[a][b] = -'r';
                        T[*n].xrN = a;
                        T[*n].yrN = b;
                        // cas où le roi adverse est pris...
                        if (T[*n].xrB == a && T[*n].yrB == b) {
                            T[*n].xrB = -1;
                            T[*n].yrB = -1;
                        }

                        T[*n].roqueN = 'n';  // aucun roque ne sera plus possible à partir de cette config
                        (*n)++;
                    }
            }  // for
            break;
    }

}  // deplacementsN

/* Génere dans T tous les coups possibles de la pièce (de couleur B) se trouvant à la pos x,y */
void deplacementsB(struct config conf, int x, int y, struct config T[], int *n) {
    int i, j, a, b, stop;

    switch (conf.mat[x][y]) {
        // mvmt PION ...
        case 'p':
            if (x < 7 && conf.mat[x + 1][y] == 0) {  // avance d'une case
                copier(&conf, &T[*n]);
                T[*n].mat[x][y] = 0;
                T[*n].mat[x + 1][y] = 'p';
                (*n)++;
                if (x == 6) transformPion(conf, x, y, x + 1, y, T, n);
            }
            if (x == 1 && conf.mat[2][y] == 0 && conf.mat[3][y] == 0) {  // avance de 2 cases
                copier(&conf, &T[*n]);
                T[*n].mat[1][y] = 0;
                T[*n].mat[3][y] = 'p';
                (*n)++;
            }
            if (x < 7 && y > 0 && conf.mat[x + 1][y - 1] < 0) {  // mange à gauche (en montant)
                copier(&conf, &T[*n]);
                T[*n].mat[x][y] = 0;
                T[*n].mat[x + 1][y - 1] = 'p';
                // cas où le roi adverse est pris...
                if (T[*n].xrN == x + 1 && T[*n].yrN == y - 1) {
                    T[*n].xrN = -1;
                    T[*n].yrN = -1;
                }

                (*n)++;
                if (x == 6) transformPion(conf, x, y, x + 1, y - 1, T, n);
            }
            if (x < 7 && y < 7 && conf.mat[x + 1][y + 1] < 0) {  // mange à droite (en montant)
                copier(&conf, &T[*n]);
                T[*n].mat[x][y] = 0;
                T[*n].mat[x + 1][y + 1] = 'p';
                // cas où le roi adverse est pris...
                if (T[*n].xrN == x + 1 && T[*n].yrN == y + 1) {
                    T[*n].xrN = -1;
                    T[*n].yrN = -1;
                }

                (*n)++;
                if (x == 6) transformPion(conf, x, y, x + 1, y + 1, T, n);
            }
            break;

        // mvmt CAVALIER ...
        case 'c':
            for (i = 0; i < 8; i++)
                if (x + dC[i][0] <= 7 && x + dC[i][0] >= 0 && y + dC[i][1] <= 7 && y + dC[i][1] >= 0)
                    if (conf.mat[x + dC[i][0]][y + dC[i][1]] <= 0) {
                        copier(&conf, &T[*n]);
                        T[*n].mat[x][y] = 0;
                        T[*n].mat[x + dC[i][0]][y + dC[i][1]] = 'c';
                        // cas où le roi adverse est pris...
                        if (T[*n].xrN == x + dC[i][0] && T[*n].yrN == y + dC[i][1]) {
                            T[*n].xrN = -1;
                            T[*n].yrN = -1;
                        }

                        (*n)++;
                    }
            break;

        // mvmt FOU ...
        case 'f':
            for (i = 1; i < 8; i += 2) {
                // traitement des directions impaires (1, 3, 5 et 7)
                stop = 0;
                a = x + D[i][0];
                b = y + D[i][1];
                while (!stop && a >= 0 && a <= 7 && b >= 0 && b <= 7) {
                    if (conf.mat[a][b] > 0)
                        stop = 1;
                    else {
                        copier(&conf, &T[*n]);
                        T[*n].mat[x][y] = 0;
                        if (T[*n].mat[a][b] < 0) stop = 1;
                        T[*n].mat[a][b] = 'f';
                        // cas où le roi adverse est pris...
                        if (T[*n].xrN == a && T[*n].yrN == b) {
                            T[*n].xrN = -1;
                            T[*n].yrN = -1;
                        }

                        (*n)++;
                        a = a + D[i][0];
                        b = b + D[i][1];
                    }
                }  // while
            }      // for
            break;

        // mvmt TOUR ...
        case 't':
            for (i = 0; i < 8; i += 2) {
                // traitement des directions paires (0, 2, 4 et 6)
                stop = 0;
                a = x + D[i][0];
                b = y + D[i][1];
                while (!stop && a >= 0 && a <= 7 && b >= 0 && b <= 7) {
                    if (conf.mat[a][b] > 0)
                        stop = 1;
                    else {
                        copier(&conf, &T[*n]);
                        T[*n].mat[x][y] = 0;
                        if (T[*n].mat[a][b] < 0) stop = 1;
                        T[*n].mat[a][b] = 't';
                        // cas où le roi adverse est pris...
                        if (T[*n].xrN == a && T[*n].yrN == b) {
                            T[*n].xrN = -1;
                            T[*n].yrN = -1;
                        }

                        if (conf.roqueB != 'e' && conf.roqueB != 'n') {
                            if (x == 0 && y == 0 && conf.roqueB != 'p')
                                T[*n].roqueB = 'g';  // le grand roque ne sera plus possible
                            else if (x == 0 && y == 0)
                                T[*n].roqueB = 'n';  // ni le grand roque ni le petit roque ne seront possibles
                            if (x == 0 && y == 7 && conf.roqueB != 'g')
                                T[*n].roqueB = 'p';  // le petit roque ne sera plus possible
                            else if (x == 0 && y == 7)
                                T[*n].roqueB = 'n';  // ni le grand roque ni le petit roque ne seront possibles
                        }

                        (*n)++;
                        a = a + D[i][0];
                        b = b + D[i][1];
                    }
                }  // while
            }      // for
            break;

        // mvmt REINE ...
        case 'n':
            for (i = 0; i < 8; i += 1) {
                // traitement des 8 directions paires et impaires
                stop = 0;
                a = x + D[i][0];
                b = y + D[i][1];
                while (!stop && a >= 0 && a <= 7 && b >= 0 && b <= 7) {
                    if (conf.mat[a][b] > 0)
                        stop = 1;
                    else {
                        copier(&conf, &T[*n]);
                        T[*n].mat[x][y] = 0;
                        if (T[*n].mat[a][b] < 0) stop = 1;
                        T[*n].mat[a][b] = 'n';
                        // cas où le roi adverse est pris...
                        if (T[*n].xrN == a && T[*n].yrN == b) {
                            T[*n].xrN = -1;
                            T[*n].yrN = -1;
                        }

                        (*n)++;
                        a = a + D[i][0];
                        b = b + D[i][1];
                    }
                }  // while
            }      // for
            break;

        // mvmt ROI ...
        case 'r':
            // vérifier possibilité de faire un roque ...
            if (conf.roqueB != 'n' && conf.roqueB != 'e') {
                if (conf.roqueB != 'g' && conf.mat[0][1] == 0 && conf.mat[0][2] == 0 && conf.mat[0][3] == 0)
                    if (!caseMenaceePar(MIN, 0, 1, conf) && !caseMenaceePar(MIN, 0, 2, conf) &&
                        !caseMenaceePar(MIN, 0, 3, conf) && !caseMenaceePar(MIN, 0, 4, conf)) {
                        // Faire un grand roque ...
                        copier(&conf, &T[*n]);
                        T[*n].mat[0][4] = 0;
                        T[*n].mat[0][0] = 0;
                        T[*n].mat[0][2] = 'r';
                        T[*n].xrB = 0;
                        T[*n].yrB = 2;
                        T[*n].mat[0][3] = 't';
                        T[*n].roqueB = 'e';  // aucun roque ne sera plus possible à partir de cette config
                        (*n)++;
                    }
                if (conf.roqueB != 'p' && conf.mat[0][5] == 0 && conf.mat[0][6] == 0)
                    if (!caseMenaceePar(MIN, 0, 4, conf) && !caseMenaceePar(MIN, 0, 5, conf) &&
                        !caseMenaceePar(MIN, 0, 6, conf)) {
                        // Faire un petit roque ...
                        copier(&conf, &T[*n]);
                        T[*n].mat[0][4] = 0;
                        T[*n].mat[0][7] = 0;
                        T[*n].mat[0][6] = 'r';
                        T[*n].xrB = 0;
                        T[*n].yrB = 6;
                        T[*n].mat[0][5] = 't';
                        T[*n].roqueB = 'e';  // aucun roque ne sera plus possible à partir de cette config
                        (*n)++;
                    }
            }

            // vérifier les autres mouvements du roi ...
            for (i = 0; i < 8; i += 1) {
                // traitement des 8 directions paires et impaires
                a = x + D[i][0];
                b = y + D[i][1];
                if (a >= 0 && a <= 7 && b >= 0 && b <= 7)
                    if (conf.mat[a][b] <= 0) {
                        copier(&conf, &T[*n]);
                        T[*n].mat[x][y] = 0;
                        T[*n].mat[a][b] = 'r';
                        T[*n].xrB = a;
                        T[*n].yrB = b;
                        // cas où le roi adverse est pris...
                        if (T[*n].xrN == a && T[*n].yrN == b) {
                            T[*n].xrN = -1;
                            T[*n].yrN = -1;
                        }

                        T[*n].roqueB = 'n';  // aucun roque ne sera plus possible à partir de cette config
                        (*n)++;
                    }
            }  // for
            break;
    }

}  // deplacementsB

/* Génère les successeurs de la configuration conf dans le tableau T, 
   retourne aussi dans n le nb de configurations filles générées */
void generer_succ(struct config conf, int mode, struct config T[], int *n) {
    int i, j, k, stop;

    *n = 0;

    if (mode == MAX) {  // mode == MAX
        for (i = 0; i < 8; i++)
            for (j = 0; j < 8; j++)
                if (conf.mat[i][j] > 0)
                    deplacementsB(conf, i, j, T, n);

        // vérifier si le roi est en echec, auquel cas on ne garde que les succ évitants l'échec...
        for (k = 0; k < *n; k++) {
            i = T[k].xrB;
            j = T[k].yrB;  // pos du roi B dans T[k]
            // vérifier s'il est menacé dans la config T[k] ...
            if (caseMenaceePar(MIN, i, j, T[k])) {
                T[k] = T[(*n) - 1];  // alors supprimer T[k] de la liste des succ...
                (*n)--;
                k--;
            }
        }  // for k
    }

    else {  // mode == MIN
        for (i = 0; i < 8; i++)
            for (j = 0; j < 8; j++)
                if (conf.mat[i][j] < 0)
                    deplacementsN(conf, i, j, T, n);

        // vérifier si le roi est en echec, auquel cas on ne garde que les succ évitants l'échec...
        for (k = 0; k < *n; k++) {
            i = T[k].xrN;
            j = T[k].yrN;
            // vérifier s'il est menacé dans la config T[k] ...
            if (caseMenaceePar(MAX, i, j, T[k])) {
                T[k] = T[(*n) - 1];  // alors supprimer T[k] de la liste des succ...
                (*n)--;
                k--;
            }
        }  // for k
    }      // if (mode == MAX) ... else ...

}  // generer_succ

/***********************************************************************/
/*********** Partie:  AlphaBeta, Initialisation et affichahe ***********/
/***********************************************************************/

/* MinMax avec elagage alpha-beta */
int minmax_ab(struct config conf, int mode, int niv, int alpha, int beta) {
    int n, i, score, score2;
    struct config T[100];

    if (feuille(conf, &score))
        return score;

    if (niv == 0)
        return estim(conf);

    if (mode == MAX) {
        generer_succ(conf, MAX, T, &n);

        score = alpha;
        for (i = 0; i < n; i++) {
            score2 = minmax_ab(T[i], MIN, niv - 1, score, beta);
            if (score2 > score) score = score2;
            if (score > beta) {
                // Coupe Beta
                return beta;
            }
        }
    } else {  // mode == MIN

        generer_succ(conf, MIN, T, &n);

        score = beta;
        for (i = 0; i < n; i++) {
            score2 = minmax_ab(T[i], MAX, niv - 1, alpha, score);
            if (score2 < score) score = score2;
            if (score < alpha) {
                // Coupe Alpha
                return alpha;
            }
        }
    }

    if (score == +INFINI) score = +100;
    if (score == -INFINI) score = -100;

    return score;

}  // minmax_ab

/* Intialise la disposition des pieces dans la configuration initiale conf */
void init(struct config *conf) {
    int i, j;

    for (i = 0; i < 8; i++)
        for (j = 0; j < 8; j++)
            conf->mat[i][j] = 0;  // Les cases vides sont initialisées avec 0

    conf->mat[0][0] = 't';
    conf->mat[0][1] = 'c';
    conf->mat[0][2] = 'f';
    conf->mat[0][3] = 'n';
    conf->mat[0][4] = 'r';
    conf->mat[0][5] = 'f';
    conf->mat[0][6] = 'c';
    conf->mat[0][7] = 't';

    for (j = 0; j < 8; j++) {
        conf->mat[1][j] = 'p';
        conf->mat[6][j] = -'p';
        conf->mat[7][j] = -conf->mat[0][j];
    }

    conf->xrB = 0;
    conf->yrB = 4;
    conf->xrN = 7;
    conf->yrN = 4;

    conf->roqueB = 'r';
    conf->roqueN = 'r';

    conf->val = 0;

}  // init

/* Affiche la configuration conf */
void affich(struct config conf) {
    int i, j, k;
    for (i = 0; i < 8; i++)
        printf("\t  %c", i + 'a');
    printf("\n");

    for (i = 0; i < 8; i++)
        printf("\t----- ");
    printf("\n");

    for (i = 8; i > 0; i--) {
        printf("    %d", i);
        for (j = 0; j < 8; j++)
            if (conf.mat[i - 1][j] < 0)
                printf("\t -%c", -conf.mat[i - 1][j]);
            else if (conf.mat[i - 1][j] > 0)
                printf("\t +%c", conf.mat[i - 1][j]);
            else
                printf("\t  ");
        printf("\n");

        for (k = 0; k < 8; k++)
            printf("\t----- ");
        printf("\n");
    }
    printf("\n");

}  // affich

/*******************************************/
/*********** Programme princiapl ***********/
/*******************************************/

int main(int argc, char *argv[]) {
    char sy, dy, ch[10];
    int sx, dx, n, i, j, score, cout, cout2, legal, hauteur, sauter;
    int cmin, cmax;

    struct config T[100], conf, conf1;

    if (argc == 1)
        hauteur = 2;  // par défaut on fixe la profondeur d'évaluation à 4
    else
        hauteur = atoi(argv[1]);  // sinon elle est récupérée depuis la ligne de commande

    printf("\n\nProfondeur d'exploration = %d\n\n", hauteur);

    // Initialise les tableaux d'estimation PeSTO (Piece-Square Tables Only)
    init_estim_tables();

    // Initialise la configuration de départ
    init(&conf);

    printf("\n\nVous êtes les + (Blancs) et je suis les - (Noirs)\n\n");
    printf("\n\nMachine vs Machine\n\n");

    // Boucle principale du dérouleùment d'une partie ...
    while (1) {
        printf("Tour du joueur Blanc ...\n");
        side2move = OTHER(WHITE);
        generer_succ(conf, MAX, T, &n);
        score = +INFINI;
        j = -1;
        for (i = 0; i < n; i++) {
            cout = minmax_ab(T[i], MIN, hauteur, -INFINI, +INFINI);
            if (cout < score) {  // Choisir le meilleur coup (c-a-d le plus petit score)
                score = cout;
                j = i;
            }
        }
        if (j != -1) {  // jouer le coup et aller à la prochaine itération ...
            copier(&T[j], &conf);
            conf.val = score;
        } else {  // S'il n'y a pas de successeur possible, l'ordinateur à perdu
            printf(" *** J'ai perdu (joueur Blanc) ***\n");
            break;
        }
        affich(conf);

        printf("Tour du joueur Noir ...\n");
        side2move = OTHER(BLACK);
        generer_succ(conf, MIN, T, &n);
        score = +INFINI;
        j = -1;
        for (i = 0; i < n; i++) {
            cout = minmax_ab(T[i], MAX, hauteur, -INFINI, +INFINI);
            if (cout < score) {  // Choisir le meilleur coup (c-a-d le plus petit score)
                score = cout;
                j = i;
            }
        }
        if (j != -1) {  // jouer le coup et aller à la prochaine itération ...
            copier(&T[j], &conf);
            conf.val = score;
        } else {  // S'il n'y a pas de successeur possible, l'ordinateur à perdu
            printf(" *** J'ai perdu (joueur Noir) ***\n");
            break;
        }
        affich(conf);
    }      // while
}
