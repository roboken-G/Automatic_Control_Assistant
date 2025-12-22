import pandas as pd
import matplotlib
matplotlib.use('Agg') # GUIなしで動作させる設定
import matplotlib.pyplot as plt
import os
import platform
import subprocess

def main():
    # CSV読み込み
    csv_path = "simulation_result.csv"
    if not os.path.exists(csv_path):
        # buildフォルダにある場合も考慮
        csv_path = "build/simulation_result.csv"
        
    if not os.path.exists(csv_path):
        print(f"[Error] CSV file not found: {csv_path}")
        return

    df = pd.read_csv(csv_path)

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
    
    # 画像として保存
    output_file = "simulation_result.png"
    plt.savefig(output_file)
    print(f"[Success] Plot saved to {output_file}")
    
    # 画像を自動で開く試み (デバッグ機能付き)
    print(f"[Debug] Attempting to open {output_file} automatically...")
    try:
        system = platform.system()
        print(f"[Debug] Detected OS: {system}")
        
        if system == "Windows":
            os.startfile(output_file)
            print("[Debug] Executed os.startfile")
        elif system == "Darwin": # macOS
            subprocess.run(["open", output_file], check=False)
            print("[Debug] Executed 'open' command")
        else: # Linux (Docker含む)
            # VS CodeのCLI 'code' で開くのを優先トライ
            print("[Debug] Trying 'code' command...")
            try:
                subprocess.run(["code", output_file], check=True)
                print("[Debug] Successfully executed 'code' command")
            except (FileNotFoundError, subprocess.CalledProcessError) as e:
                print(f"[Debug] 'code' command failed: {e}")
                # codeコマンドがない場合は xdg-open (Linux GUI) を試す
                print("[Debug] Trying 'xdg-open' command...")
                try:
                    subprocess.run(["xdg-open", output_file], check=False)
                    print("[Debug] Executed 'xdg-open' command")
                except FileNotFoundError:
                    print("[Debug] 'xdg-open' command not found")
                    pass
    except Exception as e:
        print(f"[Debug] Failed to open image: {e}")
        pass # 開けなくてもエラーにはせず終了

if __name__ == "__main__":
    main()