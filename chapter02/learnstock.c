/*
 * 株価上昇(1になる)を最も良く予測するパターンを算出する
 * （パターンが一致しない場合は株価加工(0)が正解）
 * パターンはランダムに複数生成し、
 * より予測精度が高いものを最終結果とする
 */

#include <stdio.h>
#include <stdlib.h>

/* 定数の定義 */
#define SETSIZE 100  /* 学習データセットのサイズ */
#define CNO 10       /* 学習データの桁数（特徴量）*/
#define GENMAX 10000 /* 生成する解の個数 */
#define SEED 3267    /* 乱数のシード */

/* 関数のプロトタイプ宣言 */
// 学習用データの読み込み
void readdata(int data[SETSIZE][CNO], int teacher[SETSIZE]);
// 乱数生成
int rand012();
// 解に対するスコアを計算
int calcscore(int data[SETSIZE][CNO], int teacher[SETSIZE], int answer[CNO]);

int main() {
    int score = 0;          /* スコア */
    int answer[CNO];        /* 解 */
    int data[SETSIZE][CNO]; /* 学習データ */
    int teacher[SETSIZE];   /* 教師データ */
    int bestscore = 0;      /* 最良のスコア */
    int bestanswer[CNO];    /* 最良解 */

    // 乱数を初期化
    srand(SEED);

    // 学習データを読み込み
    readdata(data, teacher);

    // 解の生成とスコア計算
    for (int i = 0; i < GENMAX; i++) {
        // 解を生成
        for (int j = 0; j < CNO; j++) {
            answer[j] = rand012();
        }
        // スコア計算
        score = calcscore(data, teacher, answer);

        // 最良スコアを更新
        if (score > bestscore) {
            for (int j = 0; j < CNO; j++) {
                bestanswer[j] = answer[j];
            }
            bestscore = score;
            for (int j = 0; j < CNO; j++) {
                printf("%1d", bestanswer[j]);
            }
            printf(":score=%d\n", bestscore);
        }
    }

    // 最良解の出力
    printf("\n Best Score\n");
    for (int j = 0; j < CNO; j++) {
        printf("%1d", bestanswer[j]);
    }
    printf(":score=%d\n", bestscore);

    return 0;
}

// 学習用データの読み込み
void readdata(int data[SETSIZE][CNO], int teacher[SETSIZE]) {
    for (int i = 0; i < SETSIZE; i++) {
        for (int j = 0; j < CNO; j++) {
            scanf("%d", &data[i][j]);
        }
        scanf("%d", &teacher[i]);
    }
}

// 0か1か2を返す
int rand012() {
    int rnd;

    // 乱数の最大値を除く
    while ((rnd = rand()) == RAND_MAX)
        ;

    return (double)rnd / RAND_MAX * 3;
}

// 解のスコアを計算する
int calcscore(int data[SETSIZE][CNO], int teacher[SETSIZE], int answer[CNO]) {
    int score = 0;
    int point; /* 一致した桁数 */

    for (int i = 0; i < SETSIZE; i++) {
        point = 0;
        for (int j = 0; j < CNO; j++) {
            if (answer[j] == 2) {
                ++point; /* ワイルドカード */
            } else if (answer[j] == data[i][j]) {
                ++point; /* 一致 */
            }
        }

        // 特徴量が全て一致して、株価上昇ならスコア加算
        if ((point == CNO) && (teacher[i] == 1)) {
            ++score;
            // 特徴量が一致せず、株価加工ならスコア加算
        } else if ((point != CNO) && (teacher[i] == 0)) {
            ++score;
        }
    }

    return score;
}
