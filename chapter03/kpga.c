#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXVALUE 100                    // 重さと価値の最大値
#define N 30                            // 荷物の個数(遺伝子の数)
#define WEIGHTLIMIT (N * MAXVALUE / 4)  // 重量の制限値
#define POOLSIZE 30                     // プールサイズ(個体数)
#define LASTG 50                        // 打ち切り世代
#define MRATE 0.01                      // 突然変異の確率
#define SEED 32767                      // 乱数のシード
#define YES 1
#define NO 0

void initparcel();        // 荷物の初期化
int evalfit(int gene[]);  // 適応度の計算
void mating(int pool[POOLSIZE][N], int ngpool[POOLSIZE * 2][N]);  // 交叉
void mutation(int ngpool[POOLSIZE * 2][N]);  // 突然変異
void printtp(int pool[POOLSIZE][N]);         // 結果出力
void initpool(int pool[POOLSIZE][N]);        // 初期集団の生成
int rndn(int l);                             // n未満の乱数の生成
int notval(int v);                           // 真理値の判定
int selectp(int roulette[POOLSIZE], int totalfitness);  // 交叉するペアの選択
void crossing(int m[], int p[], int c1[], int c2[]);  // 交叉
void selectnq(int ngpool[POOLSIZE * 2][N],
              int pool[POOLSIZE][N]);  // 次世代の選択

// グローバル変数
int parcel[N][2];  // 荷物

int main() {
    int pool[POOLSIZE][N];        // 染色体プール
    int ngpool[POOLSIZE * 2][N];  // 次世代染色体プール

    // 乱数の初期化
    srand(SEED);

    // 荷物の初期化
    initparcel();

    // 初期集団の生成
    initpool(pool);

    // 打ち切り世代まで交叉、選択を繰り返す
    for (int i = 0; i < LASTG; i++) {
        printf("%d世代\n", i);
        // 交叉
        mating(pool, ngpool);
        // 突然変異
        mutation(ngpool);
        // 次世代を選択
        selectnq(ngpool, pool);
        printtp(pool);
    }
    return 0;
}

// 荷物の初期化
void initparcel() {
    int i = 0;
    while ((i < N) && (scanf("%d %d", &parcel[i][0], &parcel[i][1]) != EOF)) {
        ++i;
    }
}

// 次世代の選択
void selectnq(int ngpool[POOLSIZE * 2][N], int pool[POOLSIZE][N]) {
    int totalfitness = 0;        // 適応度の合計値
    int roulette[POOLSIZE * 2];  // 適応度を格納
    int ball;                    // 選択位置
    int acc = 0;                 // 適応度の積算値
    int c;                       // カウンタ

    // 個体数分選択を繰り返す
    for (int i = 0; i < POOLSIZE; i++) {
        // ルーレットの作成
        totalfitness = 0;
        for (c = 0; c < POOLSIZE * 2; c++) {
            roulette[c] = evalfit(ngpool[c]);
            // 適応度の合計値を計算
            totalfitness += roulette[c];
        }
        // 染色体を選択
        // roulette[c]が大きい染色体が選ばれる確率が高くなる
        ball = rndn(totalfitness);
        acc = 0;
        for (c = 0; c < POOLSIZE * 2; c++) {
            acc += roulette[c];  // 適応度を積算
            if (acc > ball) {
                // breakした時点のcが選ばれた染色体
                break;
            }
        }

        // 染色体のコピー
        for (int j = 0; j < N; j++) {
            pool[i][j] = ngpool[c][j];
        }
    }
}

// 交叉するペアの選択
int selectp(int roulette[POOLSIZE], int totalfitness) {
    int ball;     // 選択位置の数値
    int acc = 0;  // 適応度の積算値
    int i;        // カウンタ

    // 適応度が高い染色体を選ぶ確率を高める
    ball = rndn(totalfitness);
    for (i = 0; i < POOLSIZE; i++) {
        acc += roulette[i];  // 適応度を積算
        if (acc > ball) {
            break;
        }
    }
    return i;
}

// 交叉
void mating(int pool[POOLSIZE][N], int ngpool[POOLSIZE * 2][N]) {
    int totalfitness = 0;    // 適応度の合計値
    int roulette[POOLSIZE];  // 適応度を格納
    int mama, papa;          // 親の遺伝子の番号

    // ルーレットの作成
    for (int i = 0; i < POOLSIZE; i++) {
        roulette[i] = evalfit(pool[i]);
        // 適応度の合計値を計算
        totalfitness += roulette[i];
    }

    // 選択と交叉を繰り返す
    for (int i = 0; i < POOLSIZE; i++) {
        // 選択
        do {
            mama = selectp(roulette, totalfitness);
            papa = selectp(roulette, totalfitness);
        } while (mama == papa);
        // 交叉
        crossing(pool[mama], pool[papa], ngpool[i * 2], ngpool[i * 2 + 1]);
    }
}

// 交叉
void crossing(int m[], int p[], int c1[], int c2[]) {
    int i;   // カウンタ
    int cp;  // 交叉点

    // 交叉点をランダムに決定
    cp = rndn(N);

    // 交叉点より前部分のコピー
    for (i = 0; i < cp; i++) {
        c1[i] = m[i];
        c2[i] = p[i];
    }
    // 交叉点より後部分のコピー
    for (; i < N; i++) {
        c1[i] = p[i];
        c2[i] = m[i];
    }
}

// 適応度の計算
int evalfit(int gene[]) {
    int value = 0;   // 評価値
    int weight = 0;  // 重量

    // 各遺伝子座を調べて、重量と評価値を計算
    for (int i = 0; i < N; i++) {
        weight += parcel[i][0] * gene[i];  // gene[i] = 1のときのみ加算
        value += parcel[i][1] * gene[i];   // gene[i] = 1のときのみ加算
    }

    // 重量オーバーの場合は評価値0
    if (weight >= WEIGHTLIMIT) {
        value = 0;
    }
    return value;
}

// 結果出力
void printtp(int pool[POOLSIZE][N]) {
    int fitness;              // 適応度
    double totalfitness = 0;  // 適応度の合計値
    int elite, bestfit = 0;   // エリート遺伝子の処理用変数

    for (int i = 0; i < POOLSIZE; i++) {
        for (int j = 0; j < N; j++) {
            printf("%1d", pool[i][j]);
        }
        fitness = evalfit(pool[i]);
        printf("\t%d\n", fitness);
        if (fitness > bestfit) {
            bestfit = fitness;
            elite = i;
        }
        totalfitness += fitness;
    }
    // エリート解の適応度を出力
    printf("%d\t%d \t", elite, bestfit);
    // 平均の適応度を出力
    printf("%lf\n", totalfitness / POOLSIZE);
}

// 初期集団の生成
void initpool(int pool[POOLSIZE][N]) {
    for (int i = 0; i < POOLSIZE; i++) {
        for (int j = 0; j < N; j++) {
            pool[i][j] = rndn(2);
        }
    }
}

// n未満の乱数の生成
int rndn(int l) {
    int rndno;  // 生成した乱数

    while ((rndno = ((double)rand() / RAND_MAX) * l) == l)
        ;

    return rndno;
}

// 突然変異
void mutation(int ngpool[POOLSIZE * 2][N]) {
    for (int i = 0; i < POOLSIZE; i++) {
        for (int j = 0; j < N; j++) {
            if ((double)rndn(100) / 100.0 <=
                MRATE) {  // 特定の確率で突然変異を起こす
                // 反転の突然変異
                ngpool[i][j] = notval(ngpool[i][j]);
            }
        }
    }
}

// 値を反転させる
int notval(int v) {
    if (v == YES) {
        return NO;
    } else {
        return YES;
    }
}
