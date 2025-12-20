import pandas as pd
import matplotlib.pyplot as plt

def main():
    # CSV読み込み
    try:
        df = pd.read_csv("build/simulation_result.csv")
    except FileNotFoundError:
        try:
            df = pd.read_csv("simulation_result.csv")
        except FileNotFoundError:
            print("Error: simulation_result.csv not found.")
            return

    # グラフ設定
    fig, axes = plt.subplots(2, 1, figsize=(10, 8), sharex=True)

    # 1. 位置応答の比較
    ax1 = axes[0]
    ax1.plot(df["time"], df["pos_single"], label="Single PID", color="red",  linestyle="-", alpha=0.8)
    ax1.plot(df["time"], df["pos_cascade"], label="Cascade + FF", color="blue", linestyle="-", alpha=0.8)
    ax1.plot(df["time"], df["target"], label="Target", color="black", linestyle="--")
    
    ax1.set_title("Position Response Comparison")
    ax1.set_ylabel("Position [rad]")
    ax1.grid(True)
    ax1.legend()

    # 2. 電圧入力の比較
    ax2 = axes[1]
    ax2.plot(df["time"], df["vol_single"], label="Single PID", color="red", alpha=0.6)
    ax2.plot(df["time"], df["vol_cascade"], label="Cascade + FF", color="blue", alpha=0.6)
    
    ax2.set_title("Control Input (Voltage)")
    ax2.set_ylabel("Voltage [V]")
    ax2.set_xlabel("Time [s]")
    ax2.grid(True)
    ax2.legend()

    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    main()