#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define INPUTNO 2       // 入力数
#define MAXINPUTNO 100  // 入力データの最大数

double f(double u);                 // 出力関数
void initw(double w[INPUTNO + 1]);  // 重みと閾値の初期化 w[3]はバイアス項
double forward(double w[INPUTNO + 1], double e[INPUTNO]);  // 順方向の計算
int getdata(double e[][INPUTNO]);  // データ読み込み

int main() {
    double w[INPUTNO + 1];          // 重みと閾値（バイアス）
    double e[MAXINPUTNO][INPUTNO];  // 入力データ
    double o;                       // 出力
    int n_of_e;                     // 入力データ数

    // 重み初期化
    initw(w);

    // 入力データの読み込み
    n_of_e = getdata(e);
    printf("データの個数:%d\n", n_of_e);

    // フィードフォワード計算
    for (int i = 0; i < n_of_e; i++) {
        printf("input:%d ", i);
        for (int j = 0; j < INPUTNO; j++) {
            printf("%lf ", e[i][j]);
        }
        // フィードフォワードプロパゲーション実行
        o = forward(w, e[i]);
        printf("output:%lf\n", o);
    }

    return 0;
}

// データ読み込み
int getdata(double e[][INPUTNO]) {
    int j = 0;       // カウンタ
    int n_of_e = 0;  // データセットの個数

    // データ入力
    while (scanf("%lf", &e[n_of_e][j]) != EOF) {
        j++;
        if (j >= INPUTNO) {  // jがINPUTNOを超えたら次のデータセット
            j = 0;
            n_of_e++;
        }
    }
    return n_of_e;
}

// 順方向の計算
double forward(double w[INPUTNO + 1], double e[INPUTNO]) {
    int i;        // カウンタ
    double u, o;  // 入力とバイアス項の和と出力値

    // インプットの和を計算
    u = 0;
    for (i = 0; i < INPUTNO; i++) {
        u += e[i] * w[i];  // 重みと入力値を掛けあわせて積算
    }
    u -= w[i];  // 閾値（バイアス）を減算

    // アウトプットを計算
    o = f(u);

    return o;
}

// 重みと閾値の初期化
void initw(double w[INPUTNO + 1]) {
    // 定数で初期化
    w[0] = 1;
    w[1] = 1;
    w[2] = 1.5;
    // w[2] = 0.5;
}

// 出力関数
double f(double u) {
    // ステップ関数の計算
    if (u >= 0) {
        return 1.0;
    } else {
        return 0.0;
    }

    // シグモイド関数の計算
    // return 1.0 / (1.0 + exp(-u));
}
