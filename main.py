import os
import subprocess
import glob
import time
import random
import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns

NUM_OPERATIONS_SCALES = [5000,10000, 50000 ]
MAX_VAL = 1000000

CHANCES = {
    "InsertOnly": (1.0, 0.0, 0.0, 0.0, 0.0, 0.0),
    "SearchHeavy": (0.1, 0.1, 0.6, 0.05, 0.05, 0.1),
    "DeleteHeavy": (0.1, 0.6, 0.1, 0.05, 0.05, 0.1),
    "BalancedBasic": (0.33, 0.33, 0.34, 0.0, 0.0, 0.0),  # Insert, Delete, Search
    "BalancedAll": (0.2, 0.15, 0.15, 0.15, 0.15, 0.2),  # All operations
    "QueryHeavy": (0.05, 0.05, 0.2, 0.2, 0.2, 0.3),  # Focus on search, LE, GE, Range
}


def compile_all():
    cpps = glob.glob("*.cpp")
    execs = []
    for file in cpps:
        name = os.path.splitext(file)[0]
        cmd = ["g++", "-O2", "-std=c++11", file, "-o", name]
        print(f"Compiling {file} -> {name}")
        subprocess.run(cmd, check=True)
        execs.append(name)
    return execs


def gen(n_ops, max_val, op_distribution_probs, file="abce.in"):
    with open(file, "w") as f:
        f.write(str(n_ops) + "\n")
        op_types = [1, 2, 3, 4, 5, 6]

        inserted = []
        sample_size_limit = max(100, n_ops // 100)

        for _ in range(n_ops):
            chosen_op = random.choices(op_types, weights=op_distribution_probs, k=1)[0]

            if chosen_op == 1:  # Insert
                val = random.randint(0, max_val)
                f.write(f"1 {val}\n")
                if len(inserted) < sample_size_limit:
                    inserted.append(val)
                elif random.random() < 0.1:
                    if inserted:
                        inserted[
                            random.randint(0, len(inserted) - 1)
                        ] = val

            elif chosen_op == 2:  # Delete
                if (
                    inserted and random.random() < 0.7
                ):  # 70% chance to delete an inserted value
                    val = random.choice(inserted)
                else:
                    val = random.randint(0, max_val)
                f.write(f"2 {val}\n")

            elif chosen_op == 3:  # Search
                if (
                    inserted and random.random() < 0.7
                ):  # 70% chance to pick from an inserted value
                    val = random.choice(inserted)
                else:
                    val = random.randint(0, max_val)
                f.write(f"3 {val}\n")

            elif chosen_op == 4:  # LE
                val = random.randint(0, max_val)
                f.write(f"4 {val}\n")

            elif chosen_op == 5:  # GE
                val = random.randint(0, max_val)
                f.write(f"5 {val}\n")

            elif chosen_op == 6:  # Range Query
                val1 = random.randint(0, max_val)
                val2 = random.randint(0, max_val)
                f.write(f"6 {min(val1, val2)} {max(val1, val2)}\n")


def run(x):
    start_time = time.perf_counter()
    subprocess.run(["./" + x], check=True)
    end_time = time.perf_counter()
    return end_time - start_time


def main():
    execs = compile_all()
    all_results_data = []

    dummy_num_ops = 1000
    dummy_max_val = 10000
    dummy_op_dist_name = "BalancedBasic"
    if dummy_op_dist_name not in CHANCES:
        dummy_op_dist_name = list(CHANCES.keys())[0]
    dummy_tp = CHANCES[dummy_op_dist_name]

    gen(dummy_num_ops, dummy_max_val, dummy_tp, file="abce.in")
    for algo in execs:
        run(algo)
    for dist_name, tp in CHANCES.items():
        print(f"\n--- Testing Distribution: {dist_name} ---")
        for num_ops in NUM_OPERATIONS_SCALES:
            print(f"Generating tests: Num_Ops={num_ops}, Max_Val={MAX_VAL}")
            gen(num_ops, MAX_VAL, tp, file="abce.in")

            for algo in execs:
                print(f"Running {algo} (Num_Ops={num_ops}, Dist='{dist_name}')...")
                time_taken = run(algo)
                all_results_data.append(
                    {
                        "Algorithm": algo,
                        "Num Operations": num_ops,
                        "Distribution": dist_name,
                        "Time": time_taken,
                    }
                )
                print(f"Time : {time_taken:.4f} seconds for {algo}")

    df = pd.DataFrame(all_results_data)

    for name, group_df in df.groupby("Distribution"):
        if group_df.empty:
            print(f"No data to plot for distribution: {name}")
            continue

        plt.figure(figsize=(10, 7))
        try:
            heatmap_data = group_df.pivot_table(
                index="Algorithm", columns="Num Operations", values="Time"
            )
            if not heatmap_data.empty:
                alg_order = heatmap_data.mean(axis=1).sort_values().index
                heatmap_data = heatmap_data.reindex(alg_order)

                sns.heatmap(
                    heatmap_data,
                    annot=True,
                    fmt=".4f",
                    cmap="RdYlGn_r",
                    cbar_kws={"label": "Time (seconds)"},
                    robust=True,
                )
                plt.title(f"Performance Heatmap ({name})")
                plt.ylabel("Algorithm")
                plt.xlabel("Number of Operations")
                plt.tight_layout()
                plt.savefig(
                    f"performance_heatmap_{name.replace(' ', '_').replace('/', '_')}.png"
                )
                plt.show()
            else:
                print(f"Heatmap data is empty for {name}.")
        except Exception as e:
            print(f"Could not generate heatmap for {name}: {e}")

        plt.figure(figsize=(12, 8))
        try:
            line_data = group_df.pivot_table(
                index="Num Operations", columns="Algorithm", values="Time"
            )
            if not line_data.empty:
                line_data = line_data.reindex(sorted(line_data.columns), axis=1)

                colors = [
                    "#1f77b4",
                    "#ff7f0e",
                    "#2ca02c",
                    "#d62728",
                    "#9467bd",
                    "#8c564b",
                    "#e377c2",
                    "#7f7f7f",
                    "#bcbd22",
                    "#17becf",
                    "#1a55FF",
                    "#FF5733",
                    "#33FF57",
                    "#FF33A8",
                    "#33A8FF",
                ]

                for i, alg_name in enumerate(line_data.columns):
                    color_idx = i % len(colors)
                    plt.plot(
                        line_data.index,
                        line_data[alg_name],
                        marker="o",
                        linewidth=2,
                        label=alg_name,
                        color=colors[color_idx],
                        markersize=7,
                    )

                plt.xscale("log")
                plt.title(f"Performance Line Graph ({name})")
                plt.xlabel("Number of Operations (Log Scale)")
                plt.ylabel("Execution Time (seconds)")
                plt.grid(True, which="both", linestyle="--", linewidth=0.5, alpha=0.7)
                plt.legend(
                    loc="upper left", bbox_to_anchor=(1.02, 1), borderaxespad=0.0
                )
                plt.tight_layout(rect=[0, 0, 0.85, 1])
                plt.savefig(
                    f"performance_line_graph_{name.replace(' ', '_').replace('/', '_')}.png"
                )
                plt.show()
            else:
                print(f"Line graph data is empty for {name}.")
        except Exception as e:
            print(f"Could not generate line graph for {name}: {e}")


if __name__ == "__main__":
    main()

