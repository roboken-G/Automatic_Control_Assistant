# TargetBasedOnTime

自由度1の自動制御（三角関数）  
目標値、最大加速度、最大速度を入れると、 その時間にいるべき位置と速度を返してくれる。  
モータを急加減速すると回路に負荷がかかるため、三角関数を用いて緩やかな加減速をするようにしている。

## 理解するのに必要な知識
（使う分には知らなくても問題ない）
- 理論
  - 微分積分
  - 三角関数

## 目次
- [TargetBasedOnTime](#targetbasedontime)
  - [目次](#目次)
  - [コンストラクタ](#コンストラクタ)
    - [TargetBasedOnTime::TargetBasedOnTime()](#targetbasedontimetargetbasedontime)
  - [関数](#関数)
    - [TargetBasedOnTime::set(float, float, const MotionProfile&)](#targetbasedontimesetfloat-float-const-motionprofile)
    - [TargetBasedOnTime::set(float, const MotionProfile&)](#targetbasedontimesetfloat-const-motionprofile)
    - [TargetBasedOnTime::update(uint32_t)](#targetbasedontimeupdateuint32_t)
    - [TargetBasedOnTime::getVelocity()](#targetbasedontimegetvelocity)
    - [TargetBasedOnTime::getPosition()](#targetbasedontimegetposition)
    - [TargetBasedOnTime::getAcceleration()](#targetbasedontimegetacceleration)
    - [TargetBasedOnTime::getTimeRequired()](#targetbasedontimegettimerequired)

## コンストラクタ
##### TargetBasedOnTime::TargetBasedOnTime()
> ```c++
> TargetBasedOnTime();
> ```
> 今のところ何も処理していません。  
> ```c++
> // 例
> TargetBasedOnTime arm();
> ```

## 関数

##### TargetBasedOnTime::set(float, float, float, float, float, float)
このオーバーロードは現行実装では廃止されています。代わりに `MotionProfile` 構造体を用いる以下の関数を使用してください。

→ 新API: [TargetBasedOnTime::set(float, float, const MotionProfile&)](#targetbasedontimesetfloat-float-const-motionprofile)

##### TargetBasedOnTime::set(float, float, const MotionProfile&)
```c++
void set(
  float targetPosition,
  float initialPosition,
  const aca::MotionProfile& profile
);
```
目標位置（絶対座標）と初期位置（絶対座標）を `MotionProfile` とともに設定します。
```c++
aca::TargetBasedOnTime arm;
aca::MotionProfile prof{ /*maxAcceleration=*/25.0f, /*maxVelocity=*/50.0f };
prof.firstVelocity = 20.0f;
prof.finalVelocity = 10.0f;
arm.set(150.0f, 50.0f, prof);
```

##### TargetBasedOnTime::set(float, const MotionProfile&)
```c++
void set(
  float targetPositionDistance,
  const aca::MotionProfile& profile
);
```
現在位置を0とする相対距離で設定します。
```c++
aca::MotionProfile prof{25.0f, 50.0f};
arm.set(100.0f, prof); // 0 → +100
```

##### TargetBasedOnTime::setTarget(float)
この関数は現行実装では廃止されています。相対距離での設定は `set(float, const MotionProfile&)` を使用してください。

##### TargetBasedOnTime::update(uint32_t)
```c++
aca::ControlStatus update(uint32_t time_ms);
```
渡された経過時間[ms]時点の状態を計算し，`position`・`velocity`・`acceleration`・`isFinished` を含む `ControlStatus` を返します。
```c++
auto st = arm.update(100);
// st.position, st.velocity, st.acceleration, st.isFinished
```

##### TargetBasedOnTime::getVelocity()
> ```c++
> float getVelocity();
> ```
> `velocity` を返します。
> ```c++
> // 例
> arm.getVelocity();
> ```

##### TargetBasedOnTime::getPosition()
> ```c++
> float getPosition();
> ```
> `position` を返します。  
> ```c++
> // 例
> arm.getPosition();
> ```

##### TargetBasedOnTime::getAcceleration()
> ```c++
> float getAcceleration();
> ```
> `acceleration` を返します。  
> ```c++
> // 例
> arm.getAcceleration();
> ```

##### TargetBasedOnTime::getTimeRequired()
> ```c++
> float getTimeRequired();
> ```
> 目標値に達するまでの時間を返します。  
> ```c++
> // 例
> arm.getTimeRequired();
> ```

[<< 戻る](../README.md)