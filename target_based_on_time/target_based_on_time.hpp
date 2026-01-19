#ifndef AUTOMATIC_CONTROL_ASSISTANT_TARGET_BASED_ON_TIME
#define AUTOMATIC_CONTROL_ASSISTANT_TARGET_BASED_ON_TIME

#include <cmath>
#include <cfloat>
#include <cstdint>

namespace aca {

class TargetBasedOnTime{
public:
	TargetBasedOnTime();
    void set(float targetPosition, float initialPosition, float maxAcceleration, float maxVelocity, float firstVelocity, float finalVelocity);
	void set(float targetPosition, float initialPosition, float maxAcceleration, float maxVelocity);
	void set(float targetPositionDistance, float maxAcceleration, float maxVelocity);
	void setTarget(float targetPosition);
	void update(uint32_t time);
	float getPosition();
	float getVelocity();
	float getTimeRequired(); //目標値に達するまでの時間を返す
private:
	float targetPositionDistance;
	float targetPosition;
	float initialPosition;
	float maxAcceleration;
	float maxVelocity;
	float maxVelocityThisTime;
	float borderDistance; //加速しきれる最小距離

	float position;
	float velocity;
	float getPositionBasic(float time, float bottomUp);
	float getVelocityBasic(float time, float bottomUp);

	float periodOfAcceleration; //加速時間
	float periodOfConstantVelocity; //等速時間
	float periodOfDeceleration; //減速時間

	float firstVelocity;
	float finalVelocity;

    float pow2(float x){return x*x;}
};

} // namespace aca

#endif //AUTOMATIC_CONTROL_ASSISTANT_TARGET_BASED_ON_TIME
