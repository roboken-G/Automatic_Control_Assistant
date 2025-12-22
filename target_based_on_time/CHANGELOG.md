# TargetBasedOnTime 変更履歴

## v4.0.0

- 破壊的変更
	- **`set` 関数の引数変更**
	    - 以前: `set(float target, float current, float maxAcc, float maxVel, ...)` のように引数を羅列．
	    - 現在: 速度・加速度設定を `MotionProfile` 構造体に集約．
	        - `set(float target, float current, const MotionProfile& profile)`
	- **`update` 関数の戻り値変更**
	    - 以前: `void` (状態は内部更新され，getterで取得)．
	    - 現在: `ControlStatus` 構造体を返すように変更．位置・速度・加速度・完了フラグを一括で取得可能．
	- **`setTarget` 関数の削除**
	    - `set` 関数に統合されたため削除．
- 追加
	- **構造体 `MotionProfile` の導入**    
	    - `maxAcceleration`, `maxVelocity`, `firstVelocity`, `finalVelocity` をメンバとして持つ．
	- **構造体 `ControlStatus` の導入**
	    - `position`, `velocity`, `acceleration`, `isFinished` をメンバとして持つ．
	- **`getAcceleration()` の追加**
	    - 現在の加速度を取得する関数を追加．