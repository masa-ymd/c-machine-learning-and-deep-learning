#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define INPUTNO 3       // 入力数
#define HIDDENNO 3      // 中間層のニューロン数
#define ALPHA 10        // 学習係数
#define SEED 65535      // 乱数のシード
#define MAXINPUTNO 100  // 入力データの最大数
#define BIGNUM 100      // 誤差の初期値
#define LIMIT 0.001     // 誤差の上限値

double f(double u);                             // 出力関数
void initwh(double wh[HIDDENNO][INPUTNO + 1]);  // 中間層の重みと閾値の初期化
void initwo(double wo[HIDDENNO + 1]);  // 出力層の重みと閾値の初期化
double drnd(void);                     // 乱数の生成
void print(double wh[HIDDENNO][INPUTNO + 1],
           double wo[HIDDENNO + 1]);  // 学習による重み変化表示
double forward(double wh[HIDDENNO][INPUTNO + 1], double wo[HIDDENNO + 1],
               double hi[], double e[INPUTNO]);  // 順方向の計算
void olearn(double wo[HIDDENNO + 1], double hi[], double e[INPUTNO + 1],
            double o);                 // 出力層の重みの調整
int getdata(double e[][INPUTNO + 1]);  // データ読み込み
void hlearn(double wh[HIDDENNO][INPUTNO + 1], double wo[HIDDENNO + 1],
            double hi[], double e[INPUTNO + 1], double o);  // 中間層の重み調整

int main() {
    double wh[HIDDENNO][INPUTNO + 1];  // 中間層の重みと閾値（バイアス）
    double wo[HIDDENNO + 1];           // 出力層の重みと閾値
    double e[MAXINPUTNO][INPUTNO + 1];  // 入力データ
    double hi[HIDDENNO + 1];            // 中間層の出力
    double o;                           // 出力
    double err = BIGNUM;                // 誤差
    int n_of_e;                         // 入力データ数
    int count = 0;                      // カウンタ

    // 乱数を初期化
    srand(SEED);

    // 重み初期化
    initwh(wh);
    initwo(wo);
    // 重みの初期状態を表示
    print(wh, wo);

    // 入力データの読み込み
    n_of_e = getdata(e);
    printf("データの個数:%d\n", n_of_e);

    // 誤差が閾値を下回るまで、重みの調整を繰り返す
    while (err > LIMIT) {
        err = 0.0;
        // 入力データ1つ毎に調整を繰り返す
        for (int i = 0; i < n_of_e; i++) {
            // フィードフォワード計算
            o = forward(wh, wo, hi, e[i]);
            // 出力層の重みを調整
            olearn(wo, hi, e[i], o);
            // 中間層の重みを調整
            hlearn(wh, wo, hi, e[i], 0);
            // ２乗誤差
            err += (o - e[i][INPUTNO]) * (o - e[i][INPUTNO]);
        }
        count++;
        // 誤差を出力
        fprintf(stderr, "学習回数:%d\t誤差:%lf\n", count, err);
    }
    // 学習（重みの調整）終了

    // 学習後の重み表示
    print(wh, wo);

    // 学習後の重みで出力
    for (int i = 0; i < n_of_e; i++) {
        printf("%d ", i);
        for (int j = 0; j < INPUTNO + 1; j++) {  // 最後の+1は教師データ
            printf("%lf ", e[i][j]);
        }
        // フィードフォワード
        o = forward(wh, wo, hi, e[i]);
        printf("%lf\n", o);
    }

    return 0;
}

// 出力層の重みの調整
void olearn(double wo[HIDDENNO + 1], double hi[], double e[INPUTNO + 1],
            double o) {
    double d;  // 重み更新のための一時変数
    int i;     // カウンタ

    // 重みの更新式は誤差の２乗誤差を重みで（偏）微分して導かれる
    // 詳細は http://hokuts.com/2016/10/09/bp2/
    // の多層パーセプトロンの重み更新アルゴリズムを参照
    // 出力層は(l=L)の場合
    // e[INPUTNO]は教師データ
    // シグモイド関数の導関数はfx x (1-fx)
    d = (e[INPUTNO] - o) * o * (1 - o);
    for (i = 0; i < HIDDENNO; i++) {
        // 出力層への入力（HIDDENNO個）の重みを更新
        wo[i] += ALPHA * d * hi[i];
    }
    wo[i] += ALPHA * d * (-1.0);  // 閾値（バイアス項）の更新
    // バイアス項は中間層から常に-1の入力がきているものに対する重みとして扱う
}

// 中間層の重み調整
void hlearn(double wh[HIDDENNO][INPUTNO + 1], double wo[HIDDENNO + 1],
            double hi[], double e[INPUTNO + 1], double o) {
    double dj;  // 重み更新のための一時変数
    int i, j;   // カウンタ

    // 重みの更新式は誤差の２乗誤差を重みで（偏）微分して導かれる
    // 詳細は http://hokuts.com/2016/10/09/bp2/
    // の多層パーセプトロンの重み更新アルゴリズムを参照
    // 中間層は(l<L)の場合
    for (i = 0; i < HIDDENNO; i++) {
        dj = hi[i] * (1 - hi[i]) * wo[i] * (e[INPUTNO] - o) * o * (1 - o);
        for (j = 0; j < INPUTNO; j++) {
            // 中間層への入力毎の重みを更新
            wh[j][i] += ALPHA * dj * e[j];
        }
        wh[i][j] += ALPHA * dj * (-1.0);  // 閾値の学習
    }
}

// データ読み込み
int getdata(double e[][INPUTNO + 1]) {
    int j = 0;       // カウンタ
    int n_of_e = 0;  // データセットの個数

    // データ入力
    while (scanf("%lf", &e[n_of_e][j]) != EOF) {
        j++;
        if (j > INPUTNO) {  // jがINPUTNOを超えたら次のデータセット
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
    o = 0;
    for (i = 0; i < HIDDENNO; i++) {
        o += hi[i] * wo[i];  // 中間層からの入力を積算
    }
    o -= wo[i];  // 閾値（バイアス）を減算

    return f(o);
}

// 学習による重み変化表示
void print(double wh[HIDDENNO][INPUTNO + 1], double wo[HIDDENNO + 1]) {
    // 入力層から中間層への重みを表示
    printf("中間層の重み: ");
    for (int i = 0; i < HIDDENNO; i++) {
        for (int j = 0; j < INPUTNO; j++) {
            printf("%lf ", wh[i][j]);
        }
        printf("\n");
    }

    // 中間層から出力層への重みを表示
    printf("出力層の重み: ");
    for (int i = 0; i < HIDDENNO; i++) {
        printf("%lf ", wo[i]);
    }
    printf("\n");
}

// 中間層の重みと閾値の初期化
void initwh(double wh[HIDDENNO][INPUTNO + 1]) {
    for (int i = 0; i < HIDDENNO; i++) {
        for (int j = 0; j < INPUTNO; j++) {
            wh[i][j] = drnd();
        }
    }
}

// 出力層の重みと閾値の初期化
void initwo(double wo[HIDDENNO + 1]) {
    for (int i = 0; i < HIDDENNO; i++) {
        wo[i] = drnd();
    }
}

// 乱数の生成
double drnd(void) {
    double rndno;  // 乱数格納用

    // 乱数が1になるのを防ぐ
    while ((rndno = (double)rand() / RAND_MAX) == 1.0)
        ;

    rndno = rndno * 2 - 1;  // -1から1の乱数を生成
    return rndno;
}

// 出力関数
double f(double u) {
    // シグモイド関数の計算
    return 1.0 / (1.0 + exp(-u));
}
