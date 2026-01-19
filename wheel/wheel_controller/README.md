# WheelController

駆動のための、x,y,回転方向の速度を各ホイールの速度に変換するためのライブラリ

## 駆動輪の配置
サンプルプログラムの配置
![駆動輪配置の図](drive_wheel_position.png "駆動輪配置")
ピンクの数字：駆動輪の番号  
矢印：正の方向  

## サンプルプログラム

### コンストラクタ（オムニホイール）
```c++
constexpr robot_radius = 850;
aca::OmniWheelController<4, int16_t> omni_wheel_controller( // <ホイール数, 各ホイール速度の型>
  std::array<MechanicParameter, 4>{
  	aca::MechanicParameter{ 3.0/4.0*M_PI, robot_radius},
  	aca::MechanicParameter{-3.0/4.0*M_PI, robot_radius},
  	aca::MechanicParameter{-1.0/4.0*M_PI, robot_radius},
  	aca::MechanicParameter{ 1.0/4.0*M_PI, robot_radius} // { ホイール取り付け角度, ロボット半径 }
  }
);
```

### コンストラクタ（メカナムホイール）
```C++
aca::MecanumWheelController<int16_t> omni_wheel_controller(270, 350); // (a, b)
// a: 左右のホイールの距離の半分
// b: 前後のホイールの距離の半分
```

### 更新と取得
オムニホイールの例を示すが、メカナムホイールの場合も同様
```c++
// 更新方法1
aca::field_velocity = {field_velocity_x, field_velocity_y, angular_velocity};
omniWheelController.update(field_velocity, angle);
// 更新方法2
omniWheelController.update(field_velocity_x, field_velocity_y, angular_velocity, angle);

// 取得方法1
std::array<int16_t,4> wheel_velocity;
wheel_velocity = omniWheelController.get_wheel_velocity();
// 取得方法2（非推奨）
std::array<int16_t,4> wheel_velocity;
for(uint8_t i=0; i++; i<4;){
  wheel_velocity[i] = omniWheelController.get_wheel_velocity(i);
}
```

[<< 戻る](../README.md)