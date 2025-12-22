# PidController

PIDを簡単に実装できるライブラリ。`PidController` クラスで位置型PID制御を提供し、`PidGain` 構造体による指示付き初期化子、`PidResult` での安定判定付き結果返却をサポートしています。

## 使用するのに必要な知識
- 理論
  - PID制御の基本知識  
  - 微分積分の概念  

## サンプルプログラム
### コンストラクタ
```c++
// 初期化指示子でパラメータを明確に指定
aca::PidGain gain = {
    .kp = 50.0f,           // 比例ゲイン
    .ki = 0.0f,            // 積分ゲイン
    .kd = 0.0f,            // 微分ゲイン
    .min_output = -100.0f, // 出力下限
    .max_output = 100.0f   // 出力上限
    // .tau = 0.02f         // D項フィルタ時定数 (デフォルト: 0.02s)
};
aca::PidController pid_controller(gain);

// 安定判定の条件を設定（オプション）
pid_controller.set_stability_condition(0.01f, 1.0f); // 許容誤差 0.01、持続時間 1.0s
```

### 更新と取得

### 位置型PID(普通のPID)
```c++
// dt = 制御周期（秒）
auto result = pid_controller.update(target_position, current_position, dt);
float output = result.output;     // 計算された操作量
bool is_stable = result.is_stable; // 安定状態かどうか

// output がハードウェアの限界値を超えないよう処理...
```

### 差分出力（速度型対応）
- `get_delta_output()` で前フレームからの変化量を取得可能
  - アンチワインドアップが自動で働く（飽和時は積分が停止）
  - D項は1階微分フィルタで平滑化される
```c++
auto result = pid_controller.update(target, measurement, dt);
float output = result.output;           // 現在の出力
float delta = pid_controller.get_delta_output(); // 前フレームからの変化

output_command += delta; // 差分を累積して速度型的に使用
if (result.is_stable) {
    output_command = 0; // 安定時は出力停止
}
```

### リセット
```c++
pid_controller.reset(); // 積分値、タイマ、内部状態をすべてリセット
```

## ゲイン再設定
```c++
// 実行中にゲインを変更
aca::PidGain new_gain = {.kp = 100.0f, .ki = 0.5f, .kd = 10.0f, 
                         .min_output = -100.0f, .max_output = 100.0f};
pid_controller.set_gains(new_gain);
```

# CascadePidController

位置ループ（外側）と速度ループ（内側）を組み合わせた2重制御。位置追従制度を高めながら、速度制限やアンチワインドアップの恩恵も受けられます。フィードフォワード（目標速度FF、加速度FF、重力補償）にも対応。

## 使用例
```c++
// ゲイン設定
aca::CascadeGain gain = {
    .pos_kp = 10.0f,      // 位置ループ比例
    .vel_kp = 5.0f,       // 速度ループ比例
    .vel_ki = 0.1f,       // 速度ループ積分
    .max_speed = 50.0f,   // 最大速度
    .max_output = 100.0f  // 最大出力
};
aca::CascadePidController cascade_ctrl(gain);

// 安定判定条件を設定（オプション）
cascade_ctrl.set_stability_condition(0.05f, 0.5f); // 許容誤差 0.05、持続時間 0.5s

// 制御演算（シンプル版）
float dt = 0.01f; // 制御周期
auto result = cascade_ctrl.update(target_position, current_pos, current_vel, dt);
float output = result.output;

// フィードフォワード付き（より詳細）
aca::RobotState state = {current_pos, current_vel};
aca::FeedForward ff = {
    .ref_vel = 10.0f,   // 目標速度FF
    .gravity = 5.0f     // 重力補償
};
auto result_ff = cascade_ctrl.update(target_position, state, ff, 0.0f, dt);
```

## ゲイン動的更新
```c++
// 実行中にゲインを変更（自動チューニング用）
aca::CascadeGain new_gain = {
    .pos_kp = 15.0f, .vel_kp = 8.0f, .vel_ki = 0.2f,
    .max_speed = 60.0f, .max_output = 100.0f
};
cascade_ctrl.set_gains(new_gain);
```

[<< 戻る](../README.md)