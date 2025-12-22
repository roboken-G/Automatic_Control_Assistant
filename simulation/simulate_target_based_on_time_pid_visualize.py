#!/usr/bin/env python3
"""
simulate_target_based_on_time_pid_visualize.py
TargetBasedOnTime + CascadePidController シミュレーション結果の可視化
"""

import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

CSV_FILE = "build/simulation_data.csv"

def plot_simulation():
    if not os.path.exists(CSV_FILE):
        print(f"Error: {CSV_FILE} not found.")
        print("Please run the simulation first:")
        print("  ./build/simulate_target_based_on_time_pid")
        return

    try:
        df = pd.read_csv(CSV_FILE)
    except Exception as e:
        print(f"Error reading CSV: {e}")
        return

    # グラフ作成 (4段構成)
    fig, axes = plt.subplots(4, 1, figsize=(12, 14), sharex=True)
    plt.subplots_adjust(hspace=0.3)

    # 1. 位置追従
    axes[0].plot(df['time'], df['target_pos'], 'r--', label='Target Position', linewidth=2)
    axes[0].plot(df['time'], df['actual_pos'], 'b-', label='Actual Position', linewidth=1.5)
    axes[0].set_ylabel('Position [rad]', fontsize=12)
    axes[0].set_title('TargetBasedOnTime + CascadePID Simulation', fontsize=14, fontweight='bold')
    axes[0].grid(True, linestyle='--', alpha=0.5)
    axes[0].legend(loc='best')

    # 2. 速度追従
    axes[1].plot(df['time'], df['target_vel'], 'r--', label='Target Velocity', linewidth=2)
    axes[1].plot(df['time'], df['actual_vel'], 'g-', label='Actual Velocity', linewidth=1.5)
    axes[1].set_ylabel('Velocity [rad/s]', fontsize=12)
    axes[1].grid(True, linestyle='--', alpha=0.5)
    axes[1].legend(loc='best')

    # 3. 追従誤差
    axes[2].plot(df['time'], df['error'], 'orange', label='Position Error', linewidth=1.5)
    axes[2].axhline(0, color='black', linestyle=':', linewidth=0.8)
    axes[2].set_ylabel('Error [rad]', fontsize=12)
    axes[2].grid(True, linestyle='--', alpha=0.5)
    axes[2].legend(loc='best')

    # 4. 制御出力
    axes[3].plot(df['time'], df['control_output'], 'purple', label='Control Output', linewidth=1.5)
    axes[3].set_xlabel('Time [s]', fontsize=12)
    axes[3].set_ylabel('Output [V]', fontsize=12)
    axes[3].grid(True, linestyle='--', alpha=0.5)
    axes[3].legend(loc='best')

    # 統計情報
    max_error = df['error'].abs().max()
    rmse = (df['error']**2).mean()**0.5
    final_error = df['error'].iloc[-1]
    
    info_text = (
        f"Max Error: {max_error:.4f} rad\n"
        f"RMSE: {rmse:.4f} rad\n"
        f"Final Error: {final_error:.4f} rad"
    )
    plt.figtext(0.15, 0.02, info_text, fontsize=10,
                bbox=dict(facecolor='white', alpha=0.8, edgecolor='gray'))

    print(f"=== Simulation Results ===")
    print(f"Max Error:    {max_error:.4f} rad")
    print(f"RMSE:         {rmse:.4f} rad")
    print(f"Final Error:  {final_error:.4f} rad")
    
    plt.show()

if __name__ == "__main__":
    plot_simulation()
