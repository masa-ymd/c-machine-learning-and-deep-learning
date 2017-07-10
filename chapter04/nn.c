#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define INPUTNO 2       // 入力数
#define HIDDENNO 2      // 中間層のニューロン数
#define MAXINPUTNO 100  // 入力データの最大数

double f(double u);                             // 出力関数
void initwh(double wh[HIDDENNO][INPUTNO + 1]);  // 重みと閾値の初期化
void initwo(double wo[HIDDENNO + 1]);           // 重みと閾値の初期化
double forward(double wh[HIDDENNO][INPUTNO + 1], double wo[HIDDENNO + 1],
               double hi[], double e[INPUTNO]);  // 順方向の計算
int getdata(double e[][INPUTNO]);                // データ読み込み

int main() {
    double wh[HIDDENNO][INPUTNO + 1];  // 中間層の重みと閾値（バイアス）
    double wo[HIDDENNO + 1];           // 出力層の重みと閾値
    double e[MAXINPUTNO][INPUTNO];  // 入力データ
    double hi[HIDDENNO + 1];        // 中間層の出力
    double o;                       // 出力
    int n_of_e;                     // 入力データ数

    // 重み初期化
    initwh(wh);
    initwo(wo);

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
        o = forward(wh, wo, hi, e[i]);
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
double forward(double wh[HIDDENNO][INPUTNO + 1], double wo[HIDDENNO + 1],
               double hi[], double e[INPUTNO]) {
    int i, j;     // カウンタ
    double u, o;  // 入力とバイアス項の和と出力値

    // 中間層のインプットを計算
    for (i = 0; i < HIDDENNO; i++) {
        // 中間層毎にuを計算
        u = 0;
        for (j = 0; j < INPUTNO; j++) {
            u += e[j] * wh[i][j];  // 入力を積算
        }
        u -= wh[i][j];  // 閾値（バイアス）を減算
        hi[i] = f(u);   // 中間層の出力値を格納
    }

    // 出力層の計算
    for (i = 0; i < HIDDENNO; i++) {
        o += hi[i] * wo[i];  // 中間層からの入力を積算
    }
    o -= wo[i];  // 閾値（バイアス）を減算

    return f(o);

    // アウトプットを計算
    o = f(u);

    return o;
}

// 中間層の重みと閾値の初期化
void initwh(double wh[HIDDENNO][INPUTNO + 1]) {
    // 定数で初期化
    wh[0][0] = -2;
    wh[0][1] = 3;
    wh[0][2] = -1;
    wh[1][0] = -2;
    wh[1][1] = 1;
    wh[1][2] = 0.5;
}

// 出力層の重みと閾値の初期化
void initwo(double wo[HIDDENNO + 1]) {
    // 定数で初期化
    wo[0] = -60;
    wo[1] = 94;
    wo[2] = -1;
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
