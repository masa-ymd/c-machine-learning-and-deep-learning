#include <stdio.h>
#include <stdlib.h>

#define NOA 10     // 蟻の個体数
#define ILIMIT 50  // 学習の繰り返し回数
#define Q 3        // フェロモン更新の定数
#define RHO 0.8    // フェロモン蒸発の定数
#define STEP 10    // 巣穴から餌場までのステップ数
#define EPSILON 0.15  // 行動選択の際、フェロモンに従わずランダムに動く確率
#define SEED 4321  // 乱数のシード

void printp(double pheromone[2][STEP]);  // フェロモンを表示
void printstep(int mstep[NOA][STEP]);    // 蟻の歩いた経路を表示
void walk(int cost[2][STEP], double pheromone[2][STEP],
          int mstep[NOA][STEP]);  // 蟻を歩かせる
void update(int cost[2][STEP], double pheromone[2][STEP],
            int mstep[NOA][STEP]);  // フェロモン濃度を更新
int rand01();                       // 0か1を返す
double rand1();                     // 0〜1を返す

int main() {
    // 各ステップ間の経路のコスト
    int cost[2][STEP] = {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                         {5, 5, 5, 5, 5, 5, 5, 5, 5, 5}};
    // 各ステップ間の経路におけるフェロモン濃度（0で初期化）
    double pheromone[2][STEP] = {0};
    // 蟻が歩いた経路
    int mstep[NOA][STEP];
    // 繰り返し処理のカウンタ
    int i;

    // 乱数を初期化
    srand(SEED);

    // 最適化
    for (i = 0; i < ILIMIT; i++) {
        // フェロモンの状態を表示
        printf("%d:\n", i);
        printp(pheromone);

        // 蟻を歩かせる
        walk(cost, pheromone, mstep);

        // フェロモンを更新する
        update(cost, pheromone, mstep);
    }

    // フェロモンの状態を表示
    printf("%d:\n", i);
    printp(pheromone);

    return 0;
}

// フェロモン濃度を更新
void update(int cost[2][STEP], double pheromone[2][STEP],
            int mstep[NOA][STEP]) {
    int m;              // 蟻の個体番号
    int lm;             // 蟻の歩いた距離
    double sum_lm = 0;  // 蟻の歩いた合計距離

    // フェロモンを蒸発させる
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < STEP; j++) {
            pheromone[i][j] *= RHO;
        }
    }

    // 蟻が歩いたところはフェロモンを上塗する
    for (int m = 0; m < NOA; m++) {
        // 個体mの移動距離lmを計算
        lm = 0;
        for (int i = 0; i < STEP; i++) {
            lm += cost[mstep[m][i]][i];
        }
        // フェロモンを上塗り
        for (int i = 0; i < STEP; i++) {
            pheromone[mstep[m][i]][i] += Q * (1.0 / lm);
        }
        sum_lm += lm;
    }

    // 蟻が歩いた平均距離を表示
    printf("%lf\n", sum_lm / NOA);
}

// 蟻を歩かせる
void walk(int cost[2][STEP], double pheromone[2][STEP], int mstep[NOA][STEP]) {
    int m;  // 蟻の個体番号
    int s;  // 蟻の現在ステップ

    for (int m = 0; m < NOA; m++) {
        for (int s = 0; s < STEP; s++) {
            // ε-greedy法による行動選択
            if ((rand1() < EPSILON) ||
                (abs(pheromone[0][s] - pheromone[1][s]) < 1e-9)) {
                // ランダムに行動
                mstep[m][s] = rand01();
            } else {
                // フェロモン濃度により選択
                if (pheromone[0][s] > pheromone[1][s]) {
                    mstep[m][s] = 0;
                } else {
                    mstep[m][s] = 1;
                }
            }
        }
    }
    // 蟻の歩いた経路を表示
    printstep(mstep);
}

// 蟻の歩いた経路を表示
void printstep(int mstep[NOA][STEP]) {
    printf("*mstep\n");
    for (int i = 0; i < NOA; i++) {
        for (int j = 0; j < STEP; j++) {
            printf("%d ", mstep[i][j]);
        }
        printf("\n");
    }
}

// フェロモンの表示
void printp(double pheromone[2][STEP]) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < STEP; j++) {
            printf("%4.2lf ", pheromone[i][j]);
        }
        printf("\n");
    }
}

// 0か1を返す
int rand01() {
    int rnd;

    // 乱数の最大値を防ぐ
    while ((rnd = rand()) == RAND_MAX)
        ;

    // 乱数を返す
    return (int)((double)rnd / RAND_MAX * 2);
}

// 0〜1を返す
double rand1() { return (double)rand() / RAND_MAX; }
