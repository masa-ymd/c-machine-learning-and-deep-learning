/**
 * 畳み込みフィルタとプーリング層のセットを2つずつ用意
 * 簡単のため、ここでのフィルタは学習対象としない
 * 各プーリング層からの出力を全結合層（中間層＋出力層）
 * に入力し、中間層と出力層の重みと閾値を学習させて出力
 * 結果を得る
  **/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define INPUTSIZE 11    // 入力数
#define FILTERSIZE 3    // フィルタサイズ
#define FILTERNO 2      // フィルタの個数
#define POOLSIZE 3      // プーリングの範囲
#define POOLOUTSIZE 3   // プーリング層の出力サイズ
#define MAXINPUTNO 100  // 入力データの最大数
#define SEED 65503      // 乱数のシード
#define LIMIT 0.00001   // 誤差の上限値
#define BIGNUM 100      // 誤差の初期値
#define HIDDENNO 3      // 中間層のニューロン数
#define ALPHA 10        // 学習係数

// 畳み込みの計算
void conv(double filter[FILTERSIZE][FILTERSIZE], double e[][INPUTSIZE],
          double convout[][INPUTSIZE]);
// 畳み込み計算(本体)
double calcconv(double filter[][FILTERSIZE], double e[][INPUTSIZE], int i,
                int j);
// プーリング
void pool(double convout[][INPUTSIZE], double poolout[][POOLOUTSIZE]);
// マックスプーリングの実行
double maxpooling(double convout[][INPUTSIZE], int i, int j);
// データ読み込み
int getdata(double e[][INPUTSIZE][INPUTSIZE], int t[]);
// データ表示
void showdata(double e[][INPUTSIZE][INPUTSIZE], int t[], int n_of_e);
// フィルタの初期化
void initfilter(double filter[FILTERNO][FILTERSIZE][FILTERSIZE]);
// 乱数の生成
double drnd(void);
// 出力関数
double f(double u);
// 中間層の重みと閾値の初期化
void initwh(double wh[HIDDENNO][POOLOUTSIZE * POOLOUTSIZE * FILTERNO + 1]);
// 出力層の重みと閾値の初期化
void initwo(double wo[HIDDENNO + 1]);
// 順方向の計算
double forward(double wh[HIDDENNO][POOLOUTSIZE * POOLOUTSIZE * FILTERNO + 1],
               double wo[HIDDENNO + 1], double hi[],
               double e[POOLOUTSIZE * POOLOUTSIZE * FILTERNO + 1]);
// 出力層の重みの調整
void olearn(double wo[HIDDENNO + 1], double hi[],
            double e[POOLOUTSIZE * POOLOUTSIZE * FILTERNO + 1], double o);
// 中間層の重み調整
void hlearn(double wh[HIDDENNO][POOLOUTSIZE * POOLOUTSIZE * FILTERNO + 1],
            double wo[HIDDENNO + 1], double hi[],
            double e[POOLOUTSIZE * POOLOUTSIZE * FILTERNO + 1], double o);
// 学習による重み変化表示
void print(double wh[HIDDENNO][POOLOUTSIZE * POOLOUTSIZE * FILTERNO + 1],
           double wo[HIDDENNO + 1]);

int main() {
    double filter[FILTERNO][FILTERSIZE][FILTERSIZE];  // フィルタ
    double e[MAXINPUTNO][INPUTSIZE][INPUTSIZE];       // 入力データ
    int t[MAXINPUTNO];                                // 教師データ
    double convout[INPUTSIZE][INPUTSIZE] = {0};  // 畳み込み層からの出力
    double poolout[POOLOUTSIZE][POOLOUTSIZE];    // プール層からの出力
    int n_of_e;                                  // 入力データ数
    double err = BIGNUM;                         // 誤差
    int count = 0;                               // カウンタ
    double
        ef[POOLOUTSIZE * POOLOUTSIZE * FILTERNO + 1];  // 全結合層への入力データ
    double o;                                          // 最終出力
    double hi[HIDDENNO + 1];                           // 中間層の出力
    double wh[HIDDENNO][POOLOUTSIZE * POOLOUTSIZE * FILTERNO +
                        1];   // 中間層の重みと閾値（バイアス）
    double wo[HIDDENNO + 1];  // 出力層の重みと閾値

    // 乱数を初期化
    srand(SEED);

    // フィルターを初期化
    initfilter(filter);

    // 重み初期化
    initwh(wh);
    initwo(wo);

    // 入力データの読み込み
    n_of_e = getdata(e, t);
    showdata(e, t, n_of_e);

    // 誤差が閾値を下回るまで、重みの調整を繰り返す
    while (err > LIMIT) {
        err = 0.0;
        // 入力データ1つ毎に調整を繰り返す
        for (int i = 0; i < n_of_e; i++) {
            // フィルタ毎に繰り返す
            for (int j = 0; j < FILTERNO; j++) {
                // 畳み込み
                conv(filter[j], e[i], convout);
                // プーリング
                pool(convout, poolout);
                // プーリングの出力を全結合層に入力に格納
                for (int m = 0; m < POOLOUTSIZE; m++) {
                    for (int n = 0; n < POOLOUTSIZE; n++) {
                        // j * POOLOUTSIZE * POOLOUTSIZE:
                        // 0 〜 POOLOUTSIZE^2 フィルタ
                        // m * POOLOUTSIZE: 行
                        // n: 列
                        ef[j * POOLOUTSIZE * POOLOUTSIZE + POOLOUTSIZE * m +
                           n] = poolout[m][n];
                    }
                }
                // 教師データも格納 (最後の+1)
                ef[POOLOUTSIZE * POOLOUTSIZE * FILTERNO] = t[i];
            }
            // フィードフォワード計算
            o = forward(wh, wo, hi, ef);
            // 出力層の重みを調整
            olearn(wo, hi, ef, o);
            // 中間層の重みを調整
            hlearn(wh, wo, hi, ef, o);
            // ２乗誤差
            err += (o - t[i]) * (o - t[i]);
        }
        count++;
        // 誤差を出力
        fprintf(stderr, "学習回数:%d\t誤差:%lf\n", count, err);
    }
    // 学習（重みの調整）終了

    printf("\n***Results***\n");

    // 学習後の重み表示
    printf("Weights\n");
    print(wh, wo);

    // 学習後の重みを使って出力
    printf("Network output\n");
    printf("#\tteacher\toutput\n");
    for (int i = 0; i < n_of_e; i++) {  // データごとの繰り返し
        printf("%d\t%d\t", i, t[i]);
        for (int j = 0; j < FILTERNO; j++) {  // フィルタ毎の繰り返し
            // 畳み込みの計算
            conv(filter[j], e[i], convout);
            // プーリングの計算
            pool(convout, poolout);
            // プーリング層からの出力を全結合層へコピー
            for (int m = 0; m < POOLOUTSIZE; m++) {
                for (int n = 0; n < POOLOUTSIZE; n++) {
                    ef[j * POOLOUTSIZE * POOLOUTSIZE + POOLOUTSIZE * m + n] =
                        poolout[m][n];
                }
            }
            ef[POOLOUTSIZE * POOLOUTSIZE * FILTERNO] = t[i];  // 教師データ
        }
        // フィードフォワード
        o = forward(wh, wo, hi, ef);
        printf("%lf\n", o);
    }

    return 0;
}

// 学習による重み変化表示
void print(double wh[HIDDENNO][POOLOUTSIZE * POOLOUTSIZE * FILTERNO + 1],
           double wo[HIDDENNO + 1]) {
    // 入力層から中間層への重みを表示
    printf("中間層の重み: ");
    for (int i = 0; i < HIDDENNO; i++) {
        for (int j = 0; j < POOLOUTSIZE * POOLOUTSIZE * FILTERNO + 1; j++) {
            printf("%lf ", wh[i][j]);
        }
    }
    printf("\n");

    // 中間層から出力層への重みを表示
    printf("出力層の重み: ");
    for (int i = 0; i < HIDDENNO + 1; i++) {
        printf("%lf ", wo[i]);
    }
    printf("\n");
}

// 中間層の重み調整
void hlearn(double wh[HIDDENNO][POOLOUTSIZE * POOLOUTSIZE * FILTERNO + 1],
            double wo[HIDDENNO + 1], double hi[],
            double e[POOLOUTSIZE * POOLOUTSIZE * FILTERNO + 1], double o) {
    double dj;  // 重み更新のための一時変数
    int i, j;   // カウンタ

    // 重みの更新式は誤差の２乗誤差を重みで（偏）微分して導かれる
    // 詳細は http://hokuts.com/2016/10/09/bp2/
    // の多層パーセプトロンの重み更新アルゴリズムを参照
    // 中間層は(l<L)の場合
    for (i = 0; i < HIDDENNO; i++) {
        dj = hi[i] * (1 - hi[i]) * wo[i] *
             (e[POOLOUTSIZE * POOLOUTSIZE * FILTERNO] - o) * o * (1 - o);
        for (j = 0; j < POOLOUTSIZE * POOLOUTSIZE * FILTERNO; j++) {
            // 中間層への入力毎の重みを更新
            wh[i][j] += ALPHA * dj * e[j];
        }
        // バイアス項は中間層から常に-1の入力がきているものに対する重みとして扱う
        wh[i][j] += ALPHA * dj * (-1.0);  // 閾値の学習
    }
}

// 出力層の重みの調整
void olearn(double wo[HIDDENNO + 1], double hi[],
            double e[POOLOUTSIZE * POOLOUTSIZE * FILTERNO + 1], double o) {
    double d;  // 重み更新のための一時変数
    int i;     // カウンタ

    // 重みの更新式は誤差の２乗誤差を重みで（偏）微分して導かれる
    // 詳細は http://hokuts.com/2016/10/09/bp2/
    // の多層パーセプトロンの重み更新アルゴリズムを参照
    // 出力層は(l=L)の場合
    // e[INPUTNO]は教師データ
    // シグモイド関数の導関数はfx x (1-fx)
    d = (e[POOLOUTSIZE * POOLOUTSIZE * FILTERNO] - o) * o * (1 - o);
    for (i = 0; i < HIDDENNO; i++) {
        // 出力層への入力（HIDDENNO個）の重みを更新
        wo[i] += ALPHA * d * hi[i];
    }
    // バイアス項は中間層から常に-1の入力がきているものに対する重みとして扱う
    wo[i] += ALPHA * d * (-1.0);  // 閾値（バイアス項）の更新
}

// 順方向の計算
double forward(double wh[HIDDENNO][POOLOUTSIZE * POOLOUTSIZE * FILTERNO + 1],
               double wo[HIDDENNO + 1], double hi[],
               double e[POOLOUTSIZE * POOLOUTSIZE * FILTERNO + 1]) {
    int i, j;     // カウンタ
    double u, o;  // 入力とバイアス項の和と出力値

    // 中間層のインプットを計算
    for (i = 0; i < HIDDENNO; i++) {
        // 中間層毎にuを計算
        u = 0;
        for (j = 0; j < POOLOUTSIZE * POOLOUTSIZE * FILTERNO; j++) {
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

// 中間層の重みと閾値の初期化
void initwh(double wh[HIDDENNO][POOLOUTSIZE * POOLOUTSIZE * FILTERNO + 1]) {
    for (int i = 0; i < HIDDENNO; i++) {
        for (int j = 0; j < POOLOUTSIZE * POOLOUTSIZE * FILTERNO + 1; j++) {
            wh[i][j] = drnd();
        }
    }
}

// 出力層の重みと閾値の初期化
void initwo(double wo[]) {
    for (int i = 0; i < HIDDENNO + 1; i++) {
        wo[i] = drnd();
    }
}

// フィルタの初期化
void initfilter(double filter[FILTERNO][FILTERSIZE][FILTERSIZE]) {
    for (int i = 0; i < FILTERNO; i++) {
        for (int j = 0; j < FILTERSIZE; j++) {
            for (int k = 0; k < FILTERSIZE; k++) {
                filter[i][j][k] = drnd();
            }
        }
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

// プーリングの計算
void pool(double convout[][INPUTSIZE], double poolout[][POOLOUTSIZE]) {
    // POOLOUTSIZE x POOLOUTSIZE 個最大値を抽出
    for (int i = 0; i < POOLOUTSIZE; i++) {
        for (int j = 0; j < POOLOUTSIZE; j++) {
            poolout[i][j] = maxpooling(convout, i, j);
        }
    }
}

// 最大値を返す
double maxpooling(double convout[][INPUTSIZE], int i, int j) {
    double max;                   // 最大値
    int halfpool = POOLSIZE / 2;  // プーリングのサイズの1/2

    // 渡されたi, jを起点として、POOLSIZEに収まる範囲の最大値を求める
    // halfpoolを引いているのは、最大値を取得する範囲が、
    // POOLSIZE内に収まるようにするため サイズを変えると成り立たない？
    // convoutの周辺部分はフィルタが当たらないので0になっている そのため+1
    max =
        convout[i * POOLOUTSIZE + 1 + halfpool][j * POOLOUTSIZE + 1 + halfpool];
    for (int m = POOLOUTSIZE * i + 1;
         m <= POOLOUTSIZE * i + 1 + (POOLSIZE - halfpool); m++) {
        for (int n = POOLOUTSIZE * j + 1;
             n <= POOLOUTSIZE * j + 1 + (POOLSIZE - halfpool); n++) {
            if (max < convout[m][n]) {
                max = convout[m][n];
            }
        }
    }
    return max;
}

// データ表示
void showdata(double e[][INPUTSIZE][INPUTSIZE], int t[], int n_of_e) {
    for (int i = 0; i < n_of_e; i++) {
        printf("N=%d category=%d\n", i, t[i]);
        for (int j = 0; j < INPUTSIZE; j++) {
            for (int k = 0; k < INPUTSIZE; k++) {
                printf("%.3lf ", e[i][j][k]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

// データ読み込み
int getdata(double e[][INPUTSIZE][INPUTSIZE], int t[]) {
    int i = 0, j = 0, k = 0;  // カウンタ

    while (scanf("%d", &t[i]) != EOF) {             // 教師データ読込
        while (scanf("%lf", &e[i][j][k]) != EOF) {  // 画像データ読込
            k++;
            printf("i=%d, j=%d, k=%d\n", i, j, k);
            if (k >= INPUTSIZE) {  // 次の行
                k = 0;
                j++;
                if (j >= INPUTSIZE) {  // 1サンプル終了
                    break;
                }
            }
        }
        j = 0;
        k = 0;
        i++;
    }
    return i;
}

// 畳み込みの計算
void conv(double filter[][FILTERSIZE], double e[][INPUTSIZE],
          double convout[][INPUTSIZE]) {
    int startpoint =
        FILTERSIZE / 2;  // 開始位置 フィルタが入力データからはみ出さないように

    // 縦横ともフィルタが入力データから飛びでない範囲でループ
    for (int i = startpoint; i < INPUTSIZE - startpoint; i++) {
        for (int j = startpoint; j < INPUTSIZE - startpoint; j++) {
            // 畳み込み計算結果を取得
            // 端の方のi,jは初期値により0になる(zero padding)
            convout[i][j] = calcconv(filter, e, i, j);
        }
    }
}

// 畳み込み計算(本体)
double calcconv(double filter[][FILTERSIZE], double e[][INPUTSIZE], int i,
                int j) {
    double sum = 0;  // 指定されたi, jでの畳込み結果

    for (int m = 0; m < FILTERSIZE; m++) {
        for (int n = 0; n < FILTERSIZE; n++) {
            // i or j - FILTERSIZE / 2 で、i, jを中心とした時のフィルタの
            // 左端 or 上端
            // そこから右端、下端まで、filterの対応する値をかけながら積算していく
            // 事を「畳み込み」と言っている
            sum += e[i - FILTERSIZE / 2 + m][j - FILTERSIZE / 2 + n] *
                   filter[m][n];
        }
    }

    return sum;
}

// 出力関数
double f(double u) {
    // シグモイド関数の計算
    return 1.0 / (1.0 + exp(-u));
}
