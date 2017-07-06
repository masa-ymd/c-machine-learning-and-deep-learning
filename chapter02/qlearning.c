#include <stdio.h>
#include <stdlib.h>

#define GENMAX 1000  // 学習回数
#define NODENO 15    // Q値のノード数
#define ALPHA 0.1    // 学習係数
#define GAMMA 0.9    // 次の状態のQ値を更新に使う際の割引率
#define EPSILON 0.3  // Q値に従うかランダムに行動するかを決める割合
#define SEED 32767  // 乱数のシード

int rand100();                           // 0〜100の乱数を返す
int rand01();                            // 0or1を返す
double rand1();                          // 0〜1を返す
void printqvalue(int qvalue[NODENO]);    // Q値を表示する
int selecta(int s, int qvalue[NODENO]);  // 次の行動を選択する
int updateq(int s, int qvalue[NODENO]);  // Q値を更新する

int main() {
    int s;               // 状態
    int t;               // 時刻
    int qvalue[NODENO];  // Q値

    // 乱数を初期化
    srand(SEED);

    // Q値を初期化
    for (int i = 0; i < NODENO; i++) {
        qvalue[i] = rand100();
    }
    printqvalue(qvalue);

    // 学習
    for (int i = 0; i < GENMAX; i++) {
        s = 0;                         // 行動を初期化
        for (int t = 0; t < 3; t++) {  // 3階層分選択を繰り返す
            // 行動を選択
            s = selecta(s, qvalue);
            // Q値を更新
            qvalue[s] = updateq(s, qvalue);
        }
        // Q値を出力
        printqvalue(qvalue);
    }
    return 0;
}

// Q値を更新する関数
int updateq(int s, int qvalue[NODENO]) {
    int qv;    // 更新されるQ値(returnする)
    int qmax;  // Q値の最大値

    // 最下段の場合
    if (s > 6) {
        if (s == 14) {  // s14で報酬を与える
            qv = qvalue[s] + ALPHA * (1000 - qvalue[s]);
        } else if (s == 11) {  // s11で少なめの報酬を与える
            qv = qvalue[s] + ALPHA * (500 - qvalue[s]);
        } else {
            qv = qvalue[s];
        }
        // 最下段ではない場合
    } else {
        // Qの最大値がどちらになるか判定
        if ((qvalue[2 * s + 1]) > (qvalue[2 * s + 2])) {
            qmax = qvalue[2 * s + 1];
        } else {
            qmax = qvalue[2 * s + 2];
        }
        qv = qvalue[s] + ALPHA * (GAMMA * qmax - qvalue[s]);
    }

    return qv;
}

// 次の行動を選択する関数
int selecta(int olds, int qvalue[NODENO]) {
    int s;  // 次の状態(returnする)

    // ε-greedy法による行動選択
    if (rand1() < EPSILON) {
        // ランダムに行動
        if (rand01() == 0) {
            s = 2 * olds + 1;
        } else {
            s = 2 * olds + 2;
        }
    } else {
        // Q値の最大値を選択
        if ((qvalue[2 * olds + 1]) > (qvalue[2 * olds + 2])) {
            s = 2 * olds + 1;
        } else {
            s = 2 * olds + 2;
        }
    }
    return s;
}

// Q値を出力する
void printqvalue(int qvalue[NODENO]) {
    for (int i = 1; i < NODENO; i++) {
        printf("%d\t", qvalue[i]);
    }
    printf("\n");
}

// 0〜100の乱数を返す
int rand100() {
    int rnd;

    // 乱数の最大値を除く
    while ((rnd = rand()) == RAND_MAX)
        ;

    // 乱数を返す
    return (int)((double)rnd / RAND_MAX * 100);
}

// 0 or 1を返す
int rand01() {
    int rnd;

    // 乱数の最大値を除く
    while ((rnd = rand()) == RAND_MAX)
        ;

    // 乱数を返す
    return (int)((double)rnd / RAND_MAX * 2);
}

// 0〜1を返す
double rand1() {
    // 乱数を返す
    return (double)rand() / RAND_MAX;
}
