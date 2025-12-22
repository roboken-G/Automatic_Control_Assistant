import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

# CSVファイルのパス
CSV_FILE = "build/simulation_data.csv"

def plot_simulation():
    if not os.path.exists(CSV_FILE):
        print(f"Error: {CSV_FILE} not found. Please run the C++ test program first.")
        return

    # データの読み込み
    try:
        df = pd.read_csv(CSV_FILE)
    except Exception as e:
        print(f"Error reading CSV: {e}")
        return

    # プロット設定
    fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 12), sharex=True)
    plt.subplots_adjust(hspace=0.3)
    
    # 1. 位置 (Position)
    ax1.plot(df['time'], df['position'], label='Position', color='blue', linewidth=2)
    ax1.set_ylabel('Position [unit]', fontsize=12)
    ax1.set_title('S-Curve Profile Simulation', fontsize=14)
    ax1.grid(True, linestyle='--', alpha=0.7)
    ax1.legend()

    # 2. 速度 (Velocity)
    ax2.plot(df['time'], df['velocity'], label='Velocity', color='orange', linewidth=2)
    ax2.set_ylabel('Velocity [unit/s]', fontsize=12)
    ax2.grid(True, linestyle='--', alpha=0.7)
    ax2.legend()

    # 3. 加速度 (Acceleration)
    ax3.plot(df['time'], df['acceleration'], label='Acceleration', color='green', linewidth=2)
    ax3.set_xlabel('Time [s]', fontsize=12)
    ax3.set_ylabel('Acceleration [unit/s^2]', fontsize=12)
    ax3.grid(True, linestyle='--', alpha=0.7)
    ax3.legend()

    # 最大値などの情報をテキスト表示
    max_vel = df['velocity'].abs().max()
    max_acc = df['acceleration'].abs().max()
    target_pos = df['position'].iloc[-1]
    
    info_text = (
        f"Max Velocity: {max_vel:.2f}\n"
        f"Max Acceleration: {max_acc:.2f}\n"
        f"Final Position: {target_pos:.2f}"
    )
    plt.figtext(0.02, 0.02, info_text, fontsize=10, 
                bbox=dict(facecolor='white', alpha=0.8))

    print(f"Plotting data... (MaxVel: {max_vel:.2f}, MaxAcc: {max_acc:.2f})")
    plt.show()

if __name__ == "__main__":
    plot_simulation()