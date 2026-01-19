# 変更履歴

## v3.5.2
  - CoordinateMeasurer
    - 四角形配置の角度の計算が間違っていたので修正
## v3.5.1
  - CoordinateMeasurer
    - 三角形配置の角度の計算が間違っていたので修正
## v3.5.0
  - CoordinateMeasurer
    - エンコーダ自身の角度を微調整できる機能を追加

## v3.4.0
- ドキュメント
   - フォーマット コミットメッセージを追加
   - リリース時の手順を追加
- クラスを追加
   - `CoordinateMeasurer`
     - `CoordinateMeasurerSquare`
- CoordinateMeasurer
  - エンコーダの数をテンプレートで変更できるようにした
  - `CoordinateMeasurer.cpp`を`CoordinateMeasurer.hpp`に統合

## v3.3.0
- ドキュメント
  - フォーマット を追加
  - 目次を修正
- PID_Controller
  - `PID_Controller` と `PID_Controller_Velocity` を統一
  - 許容誤差内に一定時間とどまったとき、安定したとみなし、操作量を0にする機能を追加
  - 非推奨のコンストラクタ、関数を削除
  - バグの修正
- CoordinateMeasurer
  - 変数名、関数名を命名規則に従って変更

## v3.2.0
- クラスを追加
  - `CoordinateMeasurer`
    - `CoordinateMeasurerLine`
    - `CoordinateMeasurerTriangle`
  - `WheelController`
    - (`OmniWheelController`)(既存のクラスを`WheelController`の派生クラスにした)
    - `MecanumWheelController`
- ドキュメント
  - 理解するのに必要な知識 を追加
  - 設計思想 を追加
- TargetBasedOnTime
  - 目標位置と現在位置の差が0の時、NaNを出力するバグを修正
  - 初期速度、最終速度が負の方向に最高速度より大きかったとき、訂正しないバグを修正
  - 初期位置が目標位置より大きいとき、初期速度と最終速度の正負が入れ替わるバグを修正

## v3.1.0
- OmniWheelController
  - 各ホイールの情報を個別に指定できるようにした

## v3.0.2
- PID_Controller
  - init関数で、指定していない値が自動的に0になるバグを修正

## v3.0.1
- PID_Controller
  - const変数を普通の変数に修正した

## v3.0.0
- namespace `aca` を追加
- クラスを追加
  - `OmniWheelController`
- PID_Controller
  - init関数を追加
  - `index` を `pid_parameter` に名前変更
  - README をわかりやすくした

## v2.3.0
- クラスを追加 
  - `PID_Controller`

## v2.2.0
- TargetBasedOnTime
  - 初期速度と最終速度を設定できる機能を追加
  - time のデータ型を uint32_t に変更

## v2.1.0
- TargetBasedOnTime
  - デバッグ用のプログラムをdebugフォルダに入れた

## v2.0.0
- Inc ディレクトリを削除

## v1.0.0
- クラスを追加
    - `TargetBasedOnTime`
