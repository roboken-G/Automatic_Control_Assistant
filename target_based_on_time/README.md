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
    - [TargetBasedOnTime::set(float, float, float, float, float, float)](#targetbasedontimesetfloat-float-float-float-float-float)
    - [TargetBasedOnTime::set(float, float, float, float)](#targetbasedontimesetfloat-float-float-float)
    - [TargetBasedOnTime::set(float, float, float)](#targetbasedontimesetfloat-float-float)
    - [TargetBasedOnTime::setTarget(float)](#targetbasedontimesettargetfloat)
    - [TargetBasedOnTime::update(uint32_t)](#targetbasedontimeupdateuint32_t)
    - [TargetBasedOnTime::getVelocity()](#targetbasedontimegetvelocity)
    - [TargetBasedOnTime::getPosition()](#targetbasedontimegetposition)
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
> ```c++
> void set(
>     float targetPosition,
>     float initialPosition,
>     float maxAcceleration,
>     float maxVelocity,
>     float firstVelocity,
>     float finalVelocity
> );
> ```
> 目標位置、現在位置、最大加速度、最大速度、初期速度、最終速度を設定します。
> ```c++
> // 例
> arm.set(150, 50, 25, 50, 20, 10);
> ```

##### TargetBasedOnTime::set(float, float, float, float)
> ```c++
> void set(
>     float targetPosition,
>     float initialPosition,
>     float maxAcceleration,
>     float maxVelocity
> );
> ```
> 初期速度、最終速度を0とし、目標位置、現在位置、最大加速度、最大速度を設定します。  
> ```c++
> // 例
> arm.set(150, 50, 25, 50);
> ```

##### TargetBasedOnTime::set(float, float, float)
> ```c++
> void set(
>     float targetPositionDistance,
>     float maxAcceleration,
>     float maxVelocity
> );
> ```
> 現在位置を0とし、 `TargetBasedOnTime::set(float, float, float, float)` を実行します。  
> ```c++
> // 例
> arm.set(100, 25, 50);
> ```

##### TargetBasedOnTime::setTarget(float)
> ```c++
> void setTarget(
>     float targetPosition
> );
> ```
> 現在位置を0とし、目標位置を設定します。  
> 最大加速度、最大速度はそのままです。
> ```c++
> // 例
> arm.setTarget(100);
> ```

##### TargetBasedOnTime::update(uint32_t)
> ```c++
> void update(uint32_t time);
> ```
> 渡された時間を元に、 `velocity` と `position` を更新します。  
> ```c++
> // 例
> arm.update(100);
> ```

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