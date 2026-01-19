# PID_Controller

PIDを簡単に実装できるライブラリ  

## 使用するのに必要な知識
- 理論
  - PID制御の基本知識  
  - 微分積分の概念  

## サンプルプログラム
### コンストラクタ
```c++
// 宣言例1
aca::PID_controller pid_controller({50000,0,0}, frequency, {0.01, 1000});
// ({比例パラメータ,積分パラメータ,微分パラメータ}, 制御周波数, {許容誤差, 安定したとみなす時間})

// 宣言例2
apc::PID_Element pid_parameter = {10, 0.4, 0.02}; // {比例パラメータ,積分パラメータ,微分パラメータ}
aca::PID_controller pid_controller(pid_parameter, 1000); // (パラメータ, 制御周波数)
```

### 更新と取得

### 位置型PID(普通のPID)
```c++
pid_controller.update_operation( target_position - current_position );
compare = pid_controller.get_operation();
// 以下compare がハードウェアの限界値を超えないよう再代入処理などが続く...
```

### 速度型PID
- 位置型と比較したメリット
  - PIDで求められた操作量がハードウェアの限界操作量を超えても、積分値が肥大化することがない（ワインドアップがない）
    - → オーバーシュートが少なく、応答性が良い  
  - 手動から自動へのモード変更やコントローラの切り替え時に急激に値が変化することがない（bumpがない）  
    - → 手動/自動切り替え時のの衝撃が小さい
```c++
pid_controller.update_operation( target_position - current_position );
compare += pid_controller.get_operation_difference(); // 注：ハードウェアの限界値などが反映され、実際にMDに渡された compare の値に足していく
if( pid_controller.get_is_stable() ){
  compare = 0;
}
// 以下compare がハードウェアの限界値を超えないよう再代入処理などが続く...
```

### リセット
```c++
pid_controller.reset_integral();
```
※位置型PIDでのみ有効  


[<< 戻る](../README.md)