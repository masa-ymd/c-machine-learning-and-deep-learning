#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 256 /* 入力バッファサイズ */

int main() {
    char   linebuf[BUFSIZE]; /* 入力バッファ */
    double data;             /* 入力データ */
    double sum = 0.0;        /* 和 */
    double sum2 = 0.0;       /* 2乗和 */

    while (fgets(linebuf, BUFSIZE, stdin) != NULL) {
        /* 行の読み取りが可能な間、繰り返し */
        /* 第一引数の文字列を第二引数のフォーマットで、
           第三引数以降の変数に格納する */
        if (sscanf(linebuf, "%lf", &data) != 0) /* 変換が成功したら */ {
            sum += data;
            sum2 += data * data;
            printf("%lf\t%lf\n", sum, sum2);
        }
    }
    return 0;
}
