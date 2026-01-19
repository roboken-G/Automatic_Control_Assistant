#include "target_based_on_time.hpp"

namespace aca {

TargetBasedOnTime::TargetBasedOnTime(){

}

void TargetBasedOnTime::set(float targetPosition, float initialPosition, float maxAcceleration, float maxVelocity, float firstVelocity, float finalVelocity){
    // 最大加速度、最大速度を設定（負の数は正の数に直す）
    this->maxAcceleration = maxAcceleration = std::abs(maxAcceleration);
    this->maxVelocity = maxVelocity = std::abs(maxVelocity);
    if (std::abs(targetPosition - initialPosition) <= FLT_EPSILON || maxAcceleration <= FLT_EPSILON || maxVelocity <= FLT_EPSILON) // 目標位置までの距離、最高加速度、最高速度 のいずれかが0(floatの誤差幅以下)の時
        return;

    this->targetPosition = targetPosition;
    this->initialPosition = initialPosition;

    // 初期速度、最終速度の絶対値が最高速度より大きかったら、最高速度の範囲内に収まるように変更する
    if (maxVelocity < firstVelocity){
        firstVelocity = maxVelocity;
    }else if (firstVelocity < -maxVelocity){
        firstVelocity = -maxVelocity;
    }
    if (maxVelocity < finalVelocity){
        finalVelocity = maxVelocity;
    }else if (finalVelocity < -maxVelocity){
        finalVelocity = -maxVelocity;
    }

    // 処理方法の都合上、初期位置が目標位置より大きかったら、初期速度と最終速度の符号を入れ替える
    if (targetPosition < initialPosition){
        firstVelocity = -firstVelocity;
        finalVelocity = -finalVelocity;
    }

    this->finalVelocity = finalVelocity;
    this->firstVelocity = firstVelocity;

    targetPositionDistance = std::abs(targetPosition-initialPosition);

    //加速しきれる最小距離の計算
    borderDistance = M_PI/(4*maxAcceleration) * ( 2*pow2(maxVelocity) - pow2(firstVelocity) - pow2(finalVelocity) );

    //最高速度の計算
    if (targetPositionDistance < borderDistance){
        maxVelocityThisTime = sqrtf(2*targetPositionDistance*maxAcceleration/M_PI + pow2(firstVelocity)/2 + pow2(finalVelocity)/2 );
    }else {
        maxVelocityThisTime = maxVelocity;
    }

    //加速時間、等速時間、減速時間の計算
    periodOfAcceleration = (maxVelocityThisTime-firstVelocity)*M_PI/(2*maxAcceleration);
    periodOfDeceleration = (maxVelocityThisTime-finalVelocity)*M_PI/(2*maxAcceleration);
    periodOfConstantVelocity = 0;
    if (borderDistance < targetPositionDistance) {
        periodOfConstantVelocity = (targetPositionDistance-borderDistance)/maxVelocityThisTime;
    }
}

void TargetBasedOnTime::set(float targetPosition, float initialPosition, float maxAcceleration, float maxVelocity){
    set(targetPosition, initialPosition, maxAcceleration, maxVelocity, 0, 0);
}

void TargetBasedOnTime::set(float targetPositionDistance, float maxAcceleration, float maxVelocity){
	set(targetPositionDistance, 0, maxAcceleration, maxVelocity);
}

void TargetBasedOnTime::setTarget(float targetPosition){
	set(targetPosition, maxAcceleration, maxVelocity);
}



void TargetBasedOnTime::update(uint32_t time){
    // 最大加速度、最大速度が0の場合、そのまま処理終了
    if (maxVelocity == 0.0 || maxAcceleration == 0.0)
        return;

	if (0 <= time  &&  time < periodOfAcceleration) { //period of acceleration
		velocity = getVelocityBasic(time, firstVelocity);
		position = getPositionBasic(time, firstVelocity);
	}else if (time <= periodOfAcceleration + periodOfConstantVelocity) { //period of constant velocity
		velocity = maxVelocityThisTime;
		position = getPositionBasic(periodOfAcceleration, firstVelocity) + maxVelocityThisTime*(time-periodOfAcceleration);
	}else if (time <= periodOfAcceleration + periodOfConstantVelocity + periodOfDeceleration) { //period of deceleration
		velocity = getVelocityBasic(time-(periodOfConstantVelocity+periodOfAcceleration)+periodOfDeceleration, finalVelocity);
		position = getPositionBasic(time-(periodOfConstantVelocity+periodOfAcceleration)+periodOfDeceleration, finalVelocity) + maxVelocityThisTime*periodOfConstantVelocity + getPositionBasic(periodOfAcceleration, firstVelocity) - getPositionBasic(periodOfDeceleration, finalVelocity);
	}else { //目標値に到達後
		velocity = finalVelocity;
		position = targetPosition + (time - (periodOfAcceleration + periodOfConstantVelocity + periodOfDeceleration)) * finalVelocity;
	}

	//初期値と目標値の大小関係によって、速度と位置の正負を変える
    if (initialPosition > targetPosition){
        velocity = -velocity;
        position = -position;
    }
    //初期値を足す
    position += initialPosition;
}


float TargetBasedOnTime::getVelocityBasic(float time, float bottomUp){
    if (maxVelocityThisTime == bottomUp)
        return 0;
	return -(maxVelocityThisTime-bottomUp) /2 *cosf(2*maxAcceleration/(maxVelocityThisTime-bottomUp)*time) + (maxVelocityThisTime-bottomUp)/2 + bottomUp;;
}

float TargetBasedOnTime::getPositionBasic(float time, float bottomUp){
    if (maxVelocityThisTime == bottomUp)
        return 0;
	return -pow2(maxVelocityThisTime-bottomUp) /(4*maxAcceleration) *sinf(2*maxAcceleration /(maxVelocityThisTime-bottomUp) *time) + (maxVelocityThisTime-bottomUp)/2*time + bottomUp*time;
}

float TargetBasedOnTime::getVelocity(){
	return velocity;
}

float TargetBasedOnTime::getPosition(){
	return position;
}

float TargetBasedOnTime::getTimeRequired(){
	return periodOfAcceleration + periodOfConstantVelocity + periodOfDeceleration;
}

} // namespace aca