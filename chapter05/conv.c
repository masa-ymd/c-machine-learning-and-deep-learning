#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define INPUTSIZE 11   // 入力数
#define FILTERSIZE 3   // フィルタサイズ
#define POOLSIZE 3     // プーリングの範囲
#define POOLOUTSIZE 3  // プーリング層の出力サイズ

// 畳み込みの計算
void conv(double filter[][FILTERSIZE], double e[][INPUTSIZE],
          double convout[][INPUTSIZE]);
// 畳み込み計算(本体)
double calcconv(double filter[][FILTERSIZE], double e[][INPUTSIZE], int i,
                int j);
// 畳み込みの結果出力
void convres(double convout[][INPUTSIZE]);
// プーリング
void pool(double convout[][INPUTSIZE], double poolout[][POOLOUTSIZE]);
// マックスプーリングの実行
double maxpooling(double convout[][INPUTSIZE], int i, int j);
// 結果出力
void poolres(double poolout[][POOLOUTSIZE]);
// データ読み込み
void getdata(double e[][INPUTSIZE]);

int main() {
    double filter[FILTERSIZE][FILTERSIZE] = {
        {0, 0, 0}, {1, 1, 1}, {0, 0, 0}};  // 横フィルタ 横方向に強く反応
    //{{0, 1, 0}, {0, 1, 0}, {0, 1, 0}};  // 縦フィルタ 縦方向により強く反応
    double e[INPUTSIZE][INPUTSIZE];              // 入力データ
    double convout[INPUTSIZE][INPUTSIZE] = {0};  // 畳み込み層からの出力
    double poolout[POOLOUTSIZE][POOLOUTSIZE];    // プール層からの出力

    // 入力データ読み込み
    getdata(e);

    // 畳み込み計算
    // フィルタをかけることでエッジや特定の形を検出する
    conv(filter, e, convout);
    // 畳み込み結果を表示
    convres(convout);

    // プーリング計算
    // データを圧縮することで、位置の違いに対して頑健になる
    pool(convout, poolout);
    // プーリング計算の結果を表示
    poolres(poolout);

    return 0;
}

// データ読み込み
void getdata(double e[][INPUTSIZE]) {
    int i = 0, j = 0;  // カウンタ

    // 標準入力を読み込む
    while (scanf("%lf", &e[i][j]) != EOF) {
        j++;
        if (j >= INPUTSIZE) {  // 列の終了
            j = 0;
            i++;
            if (i >= INPUTSIZE) {  // 行の終了
                break;
            }
        }
    }
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

// 畳み込みの結果出力
void convres(double convout[][INPUTSIZE]) {
    for (int i = 0; i < INPUTSIZE; i++) {
        for (int j = 0; j < INPUTSIZE; j++) {
            printf("%.3lf ", convout[i][j]);
        }
        printf("\n");
    }
    printf("\n");
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

// 結果出力
void poolres(double poolout[][POOLOUTSIZE]) {
    for (int i = 0; i < POOLOUTSIZE; i++) {
        for (int j = 0; j < POOLOUTSIZE; j++) {
            printf("%.3lf ", poolout[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
