#ifndef PID_CONTROLLER_HPP
#define PID_CONTROLLER_HPP

#include <stdint.h>
#include <cmath>

namespace aca {

/**
 * PIDのパラメータを保持する構造体
 */
struct PID_Element{
    float proportional;
    float integral;
    float differential;
};

/**
 * 安定条件を表す構造体
 */
struct StableConditions{
	/**
	 * 許容誤差
	 * @details 大きくし過ぎると応答性が悪くなるので、小さい値から調整していくことを推奨する
	 */
	float allowable_error;

	/**
	 * 安定時間 [ms]
	 * @details この時間分、許容誤差内に収まっていたら、安定したみなす
	 * @details 初期値(0)の場合は判定しない(常にfalse)
	 */
	uint16_t stable_time;
};

class PID_controller
{
private:
	/* 基本設定値 */
	PID_Element pid_parameter;	 		//!< PIDの各パラメータ
	float frequency;					//!< 制御周波数 [Hz]

	/* 操作量関係の変数 */
	PID_Element operation;				//!< PIDの各生値（パラメータをかける前）
	float operation_value;				//!< 操作量
	float last_operation_value = 0;		//!< 前回の操作量
	float last_difference = 0;			//!< 前回の差

	/* 安定判定関係の設定値及び変数 */
	StableConditions stable_conditions;	//!< 安定条件
	uint16_t stable_time_counter;		//!< 安定時間をカウントするカウンタ変数 [ms]
	bool is_stable = false;				//!< 安定しているかどうかを表わす変数

public:
	/* コンストラクタ */
	PID_controller(const PID_Element &pid_parameter, const float frequency, const StableConditions &stable_conditions);
	PID_controller(const PID_Element &pid_parameter, const float frequency);
	PID_controller(const PID_Element &pid_parameter);
	PID_controller(const float frequency);

	/* 初期化関数(init, reset) */
	void init(const PID_Element &pid_parameter, const float frequency);
	void init(const PID_Element &pid_parameter);
	void init(const float frequency);
	void reset_integral();

	/* 設定関数(setter) */
	void set_stable_conditions(const StableConditions &stable_conditions);

	/* 更新関数(update) */
	void update_operation(const float difference);

	/* 取得関数(getter) */
	float get_operation();
	float get_operation_difference();
	bool get_is_stable();
};

} // namespace aca

#endif //PID_CONTROLLER_HPP
