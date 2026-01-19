#ifndef WHEEL_WHEEL_CONTROLLER_HPP_
#define WHEEL_WHEEL_CONTROLLER_HPP_

#include <array>
#include <cmath>
#include <cstdint>
#include "coordinate.hpp"

namespace aca {

/**
 * @brief 駆動を制御するためのクラス
 * @tparam NUMBER_OF_WHEELS 駆動輪の数
 * @tparam WHEEL_VELOCITY_TYPE 出力するデータの型
 */
template <std::size_t NUMBER_OF_WHEELS, typename WHEEL_VELOCITY_TYPE>
class WheelController{
protected:
    std::array<WHEEL_VELOCITY_TYPE, NUMBER_OF_WHEELS> wheel_velocity;

private:
    /**
	 * @brief フィールド座標をロボット座標に変換する関数
     * @param field_velocity フィールド座標を格納した構造体
     * @param angle ロボットの角度
     * @return ロボット座標
     */
    Coordinate<float> convert_to_robot_velocity(Coordinate<float> field_velocity, float angle){
        Coordinate<float> robot_velocity;
    	float cos_value = std::cos(angle);
    	float sin_value = std::sin(angle);
        robot_velocity.x =  field_velocity.x * cos_value + field_velocity.y * sin_value;
        robot_velocity.y = -field_velocity.x * sin_value + field_velocity.y * cos_value;
        robot_velocity.angle = field_velocity.angle;
        return robot_velocity;
    };

    /**
	 * @brief ロボット座標速度　→　各ホイールの速度
     * @param robot_velocity ロボット座標での速度
     */
    virtual void convert_each_wheel(Coordinate<float> robot_velocity) = 0;

public:

    /**
	 * @brief 各ホイールの速度を更新する
     * @param field_velocity フィールド座標速度を格納した構造体
     * @param angle ロボットの角度
     */
    void update(Coordinate<float> field_velocity, float angle){
        convert_each_wheel( convert_to_robot_velocity(field_velocity, angle) );
    };

    /**
	 * @brief 各ホイールの速度を更新する
	 * @details 内部で上記の update を実行している
     * @param field_velocity_x フィールド座標のx方向の速度
     * @param field_velocity_y フィールド座標のy方向の速度
     * @param angular_velocity フィールド座標の回転方向の速度
     * @param angle ロボットの角度
     */
    void update(float field_velocity_x, float field_velocity_y, float angular_velocity, float angle){
        update( Coordinate<float>{field_velocity_x, field_velocity_y, angular_velocity}, angle );
    };

    /**
	 * @brief 各駆動輪の速度を取得するための関数
     * @return 各駆動輪の速度を保持したarray配列
     */
    std::array<WHEEL_VELOCITY_TYPE, NUMBER_OF_WHEELS> get_wheel_velocity(){
        return wheel_velocity;
    };

    /**
	 * @brief 指定した駆動輪の速度を取得するための関数
     * @param wheel_number 駆動輪の番号
     * @return 指定した駆動輪の速度
     */
    WHEEL_VELOCITY_TYPE get_wheel_velocity(uint8_t wheel_number){
        return wheel_velocity[wheel_number];
    };
};


/*
 * OmniWheelController
 */

/**
 * @brief 機械的なパラメータを格納する構造体
 */
struct MechanicParameter{
	//! オムニホイールの取付向き
    float wheel_angle;
    //! 駆動中心からオムニホイールまでの距離
    float distance_from_center;
};

/**
 * @brief 機械的パラメータを扱いやすい値の変換した構造体
 */
struct Coefficient{
	//! wheel_angle の sin の値
    float sin_value;
    //! wheel_angle の cos の値
    float cos_value;
    //! 駆動中心からオムニホイールまでの距離
    float distance_from_center;
};

/**
 * @brief オムニホイール駆動のため、WheelController を継承したクラス
 * @tparam NUMBER_OF_WHEELS オムニホイールの数
 * @tparam WHEEL_VELOCITY_TYPE 各オムニホイールの速度の型
 */
template <std::size_t NUMBER_OF_WHEELS, typename WHEEL_VELOCITY_TYPE>
class OmniWheelController : public WheelController<NUMBER_OF_WHEELS, WHEEL_VELOCITY_TYPE>{
private:
    std::array<Coefficient, NUMBER_OF_WHEELS> coefficient;
    void convert_each_wheel(Coordinate<float> robot_velocity) override {
        for (uint8_t i=0; i<NUMBER_OF_WHEELS; i++) {
        	this->wheel_velocity[i] = robot_velocity.x * coefficient[i].cos_value + robot_velocity.y * coefficient[i].sin_value + robot_velocity.angle * coefficient[i].distance_from_center;
        }
    };

public:
    /**
	 * @brief コンストラクタ
     * @param mechanic_parameter オムニホイールの取付構造体配列
     */
    OmniWheelController(std::array<MechanicParameter, NUMBER_OF_WHEELS> mechanic_parameter){
        for (uint8_t i=0; i<NUMBER_OF_WHEELS; i++) {
            coefficient[i].sin_value            = std::sin(mechanic_parameter[i].wheel_angle);
            coefficient[i].cos_value            = std::cos(mechanic_parameter[i].wheel_angle);
            coefficient[i].distance_from_center = mechanic_parameter[i].distance_from_center;
        }
    };
};


/*
 * MecanumWheelController
 */

/**
 * @brief メカナムホイール駆動のため、WheelController を継承したクラス
 * @tparam WHEEL_VELOCITY_TYPE　各メカナムホイールの速度の型
 */
template <typename WHEEL_VELOCITY_TYPE>
class MecanumWheelController : public WheelController<4, WHEEL_VELOCITY_TYPE> {
private:
    float a,b;
	void convert_each_wheel(Coordinate<float> robot_velocity) override {
    	this->wheel_velocity[0] = -robot_velocity.x + robot_velocity.y +  (a+b)*robot_velocity.angle;
    	this->wheel_velocity[1] =  robot_velocity.x + robot_velocity.y   -(a+b)*robot_velocity.angle;
    	this->wheel_velocity[2] = -robot_velocity.x + robot_velocity.y   -(a+b)*robot_velocity.angle;
    	this->wheel_velocity[3] =  robot_velocity.x + robot_velocity.y +  (a+b)*robot_velocity.angle;
    };

public:
    /**
	 * @brief コンストラクタ
     * @param a 左右のホイールの距離の半分
     * @param b 前後のホイールの距離の半分
     */
	MecanumWheelController(
        float a,
        float b
    ):
        a(a),
        b(b)
    {
    };
};

} // namespace aca

#endif // WHEEL_WHEEL_CONTROLLER_HPP_
