#ifdef ENABLE_TEST_CODE

// GNUplot を用いたデバッグ用のプログラム

#include <cstdio>
#include <stdint.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "target_based_on_time.hpp"
#define GNUPLOT_PATH "C:/PROGRA~1/gnuplot/bin/gnuplot.exe -persist" // gnuplot.exeのある場所
// \ではなく、/を使いましょう。
// Program FilesはPROGRA~1、Program Files(x86)はPROGRA~2に置き換えましょう
#define NX 1000
struct dot
{
  float time;
  float position;
  float velocity;
};

int main(void){
    //準備処理//
    FILE *gp; //ファイルポインタを格納する変数
    gp = popen("gnuplot -persist","w"); // パイプを開く
    //範囲設定//

    //データ作成//
    float initialPosition = 1000.0f;
    aca::TargetBasedOnTime test;
    test.set(500.0f, initialPosition,0.2f,10.0f, -15.0f, 1.0f);

//    dot a[NX];
//    float dx = test.getTimeRequired()/(float)NX;
//    for(int i=0; i<NX; i++){
//        a[i].time = dx*i;
//        test.update(dx*(float)i);
//        a[i].position = test.getPosition();
//        a[i].velocity = test.getVelocity();
//        printf("%f\n",dx*i);
//    }

    const int numberOfPoints = (int) test.getTimeRequired() + 5;
    dot a[numberOfPoints];
    for(int i=0; i<numberOfPoints; i++){
        a[i].time = i;
        test.update(i);
        a[i].position = test.getPosition();
        a[i].velocity = test.getVelocity();
    }

    //plot//

//    fprintf(gp, "set xrange [-1:70]\n");
//    fprintf(gp, "set yrange [0:60]\n");

    // fprintf(gp, "plot '-' with lines  linetype 1  title \"a(time)\"\n"); //ラインタイプ、タイトル指定

    fprintf(gp, "plot '-' with points linetype 1  title \"position(time)\", \
                      '-' with points linetype 2  title \"velocityから積分したposition(time)\", \
                      '-' with points linetype 3  title \"velocity(time)\"\n"); //データ送信準備
    for(int i=0; i<numberOfPoints; i++){
        fprintf(gp, "%f\t%f\n", a[i].time, a[i].position); // データの書き込み 送信
    }
    fprintf(gp,"e\n"); //データ終了

    for(int i=0; i<numberOfPoints; i++){
        static float buffer = initialPosition;
        fprintf(gp, "%f\t%f\n", a[i].time, buffer);
        buffer += (a[i].velocity+a[i+1].velocity)/2;
    }
    fprintf(gp,"e\n"); //データ終了

    for(int i=0; i<numberOfPoints; i++){
        fprintf(gp, "%f\t%f\n", a[i].time, a[i].velocity);
    }
    fprintf(gp,"e\n"); //データ終了


    //終了処理//
    // fflush(gp); // バッファに格納されているデータを吐き出す（不要）
    // fprintf(gp, "exit\n"); // gnuplotの終了(不要)
    pclose(gp); //パイプを閉じる

    return 0;
}

#endif