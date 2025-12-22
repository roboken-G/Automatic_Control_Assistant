#include "target_based_on_time.hpp"

namespace aca {

void TargetBasedOnTime::set(float targetPosition, float initialPosition, const MotionProfile& profileIn) {
    // プロファイルをコピーして保持
    this->profile = profileIn;

    // 最大加速度、最大速度の絶対値をとり、負の入力を補正
    this->profile.maxAcceleration = std::abs(this->profile.maxAcceleration);
    this->profile.maxVelocity = std::abs(this->profile.maxVelocity);

    // バリデーション: 目標位置までの距離、最高加速度、最高速度 のいずれかがほぼ0の場合は処理しない
    if (std::abs(targetPosition - initialPosition) <= FLT_EPSILON ||
        this->profile.maxAcceleration <= FLT_EPSILON ||
        this->profile.maxVelocity <= FLT_EPSILON) {
        return;
    }

    this->targetPosition = targetPosition;
    this->initialPosition = initialPosition;

    // 初期速度、最終速度のクリッピング（最大速度を超えないように制限）
    if (this->profile.maxVelocity < this->profile.firstVelocity) {
        this->profile.firstVelocity = this->profile.maxVelocity;
    } else if (this->profile.firstVelocity < -this->profile.maxVelocity) {
        this->profile.firstVelocity = -this->profile.maxVelocity;
    }

    if (this->profile.maxVelocity < this->profile.finalVelocity) {
        this->profile.finalVelocity = this->profile.maxVelocity;
    } else if (this->profile.finalVelocity < -this->profile.maxVelocity) {
        this->profile.finalVelocity = -this->profile.maxVelocity;
    }

    // 座標系の正規化:
    // ロジックを単純化するため、常に「正の方向」へ進むとして計算し、最後に符号を反転させる方針をとる。
    // そのため、初期位置 > 目標位置（逆走）の場合は、初速度・終端速度の符号を反転させておく。
    if (targetPosition < initialPosition) {
        this->profile.firstVelocity = -this->profile.firstVelocity;
        this->profile.finalVelocity = -this->profile.finalVelocity;
    }

    // 移動距離の計算（絶対値）
    targetPositionDistance = std::abs(targetPosition - initialPosition);

    // 加速しきれる最小距離の計算 (S字制御の理論式より)
    // borderDistance = (PI * (2*Vmax^2 - Vinit^2 - Vend^2)) / (4*Amax)
    borderDistance = static_cast<float>(M_PI) / (4.0f * this->profile.maxAcceleration) *
                     (2.0f * pow2(this->profile.maxVelocity) - pow2(this->profile.firstVelocity) - pow2(this->profile.finalVelocity));

    // 今回の移動における最高速度の決定
    if (targetPositionDistance < borderDistance) {
        // 距離が短く、設定された最高速度まで加速できない場合 -> 達成可能な最高速度を逆算
        maxVelocityThisTime = std::sqrt(
            2.0f * targetPositionDistance * this->profile.maxAcceleration / static_cast<float>(M_PI) +
            pow2(this->profile.firstVelocity) / 2.0f +
            pow2(this->profile.finalVelocity) / 2.0f);
    } else {
        // 十分な距離がある場合 -> 設定された最高速度を使用
        maxVelocityThisTime = this->profile.maxVelocity;
    }

    // 各フェーズの時間計算
    // 加速時間 T_acc = (V_top - V_start) * PI / (2 * A_max)
    periodOfAcceleration = (maxVelocityThisTime - this->profile.firstVelocity) * static_cast<float>(M_PI) / (2.0f * this->profile.maxAcceleration);
    
    // 減速時間 T_dec = (V_top - V_end) * PI / (2 * A_max)
    periodOfDeceleration = (maxVelocityThisTime - this->profile.finalVelocity) * static_cast<float>(M_PI) / (2.0f * this->profile.maxAcceleration);
    
    periodOfConstantVelocity = 0.0f;
    if (borderDistance < targetPositionDistance) {
        // 等速移動が可能なら、その時間を計算
        periodOfConstantVelocity = (targetPositionDistance - borderDistance) / maxVelocityThisTime;
    }
}

void TargetBasedOnTime::set(float targetPositionDistance, const MotionProfile& profileIn) {
    set(targetPositionDistance, 0.0f, profileIn);
}

ControlStatus TargetBasedOnTime::update(uint32_t time_u32) {
    // 設定が無効な場合のガード
    if (profile.maxVelocity <= FLT_EPSILON || profile.maxAcceleration <= FLT_EPSILON) {
        return {initialPosition, 0.0f, 0.0f, false};
    }

    float time = static_cast<float>(time_u32) / 1000.0f; // 秒変換
    bool isFinished = false;

    // --- S字軌道の計算 (すべて正方向として計算) ---
    if (0.0f <= time && time < periodOfAcceleration) {
        // --- 加速区間 ---
        velocity = getVelocityBasic(time, profile.firstVelocity);
        position = getPositionBasic(time, profile.firstVelocity);
        acceleration = getAccelerationBasic(time, profile.firstVelocity);

    } else if (time <= periodOfAcceleration + periodOfConstantVelocity) {
        // --- 等速区間 ---
        velocity = maxVelocityThisTime;
        // 位置 = 加速終了時の位置 + 等速移動距離
        position = getPositionBasic(periodOfAcceleration, profile.firstVelocity) +
                   maxVelocityThisTime * (time - periodOfAcceleration);
        acceleration = 0.0f;

    } else if (time <= periodOfAcceleration + periodOfConstantVelocity + periodOfDeceleration) {
        // --- 減速区間 ---
        float timeInDecelSequence = time - (periodOfConstantVelocity + periodOfAcceleration);
        
        // 減速計算ロジック:
        // getVelocityBasic等は、周期関数の位相を利用して計算しているため、
        // 減速フェーズ用の時間を加算して呼び出すことで減速波形（速度低下・負の加速度）を得る。
        float phaseTime = timeInDecelSequence + periodOfDeceleration;

        velocity = getVelocityBasic(phaseTime, profile.finalVelocity);
        
        position = getPositionBasic(phaseTime, profile.finalVelocity) +
                   maxVelocityThisTime * periodOfConstantVelocity +
                   getPositionBasic(periodOfAcceleration, profile.firstVelocity) -
                   getPositionBasic(periodOfDeceleration, profile.finalVelocity);
        
        // 加速度: 位相が後半に入っているため、sin波形は負になり、自動的に減速（負の加速度）となる
        acceleration = getAccelerationBasic(phaseTime, profile.finalVelocity);

    } else {
        // --- 終了後 ---
        velocity = profile.finalVelocity;
        
        // 最終的な到達位置計算
        float timeOver = time - (periodOfAcceleration + periodOfConstantVelocity + periodOfDeceleration);
        position = targetPositionDistance + timeOver * profile.finalVelocity;
        
        acceleration = 0.0f;
        isFinished = true;
    }

    // --- 座標変換 (逆方向移動の場合の符号反転) ---
    // set関数で「目標位置 < 初期位置」だった場合、ここまでの計算は正方向として行われているため、
    // 最後に符号を反転させて正しい座標系に戻す。
    if (initialPosition > targetPosition) {
        velocity = -velocity;
        position = -position;
        acceleration = -acceleration;
    }

    // 初期位置オフセットを加算して絶対座標にする
    position += initialPosition;

    // 構造体に詰めて返す
    return {position, velocity, acceleration, isFinished};
}

float TargetBasedOnTime::getVelocityBasic(float time, float bottomUp) const {
    if (std::abs(maxVelocityThisTime - bottomUp) <= FLT_EPSILON) {
        return 0.0f;
    }
    float term = maxVelocityThisTime - bottomUp;
    // v(t) = -Amp * cos(ωt) + Offset
    // ω = 2 * A / term
    return -term / 2.0f * std::cos(2.0f * profile.maxAcceleration / term * time) + term / 2.0f + bottomUp;
}

float TargetBasedOnTime::getPositionBasic(float time, float bottomUp) const {
    if (std::abs(maxVelocityThisTime - bottomUp) <= FLT_EPSILON) {
        return 0.0f;
    }
    float term = maxVelocityThisTime - bottomUp;
    // x(t) = 積分結果
    return -pow2(term) / (4.0f * profile.maxAcceleration) * std::sin(2.0f * profile.maxAcceleration / term * time) +
           term / 2.0f * time + bottomUp * time;
}

float TargetBasedOnTime::getAccelerationBasic(float time, float bottomUp) const {
    if (std::abs(maxVelocityThisTime - bottomUp) <= FLT_EPSILON) {
        return 0.0f;
    }
    float term = maxVelocityThisTime - bottomUp;
    
    // a(t) = v'(t)
    // v(t) = -C1 * cos(ωt) + ...
    // a(t) = C1 * ω * sin(ωt)
    // C1 = term / 2
    // ω = 2 * maxAcc / term
    // C1 * ω = maxAcc
    // よって a(t) = maxAcc * sin(ωt)
    
    return profile.maxAcceleration * std::sin(2.0f * profile.maxAcceleration / term * time);
}

} // namespace aca