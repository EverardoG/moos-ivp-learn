#!/usr/bin/env python3

import argparse
import subprocess
from pathlib import Path
from tqdm import tqdm

class Sweep():
    def __init__(self, args):
        self.args = args
        self.base_logdir = Path(self.args.log_directory).expanduser()

    def run_combinations(self):
        total = self.args.total_weight
        step = self.args.weight_step

        primary_weight_list = list(range(total, -1, -step))
        colregs_weight_list = [total-p for p in primary_weight_list]

        combined_weight_lists = list(zip(primary_weight_list, colregs_weight_list))

        # Outer loop: sweep through weight combinations
        with tqdm(total=len(combined_weight_lists), desc="Weight Combinations", position=0) as pbar_combinations:
            for (primary_weight, colregs_weight) in combined_weight_lists:
                pbar_combinations.set_description(f"Weights: PB={primary_weight}, CR={colregs_weight}")

                # Inner loop: statistical runs
                with tqdm(total=self.args.num_stat_runs, desc="  Stat Runs", position=1, leave=False) as pbar_runs:
                    for num in range(self.args.num_stat_runs):
                        self.run_stat_run(
                            num=num,
                            primary_weight=primary_weight,
                            colregs_weight=colregs_weight
                        )
                        pbar_runs.update(1)

                pbar_combinations.update(1)

    def run_stat_run(self, num, primary_weight, colregs_weight):
        # Create log directory for this specific trial
        logdir = self.base_logdir / f"pb_pwt_{primary_weight}_cr_pwt_{colregs_weight}" / f"trial_{num}"
        logdir.mkdir(parents=True, exist_ok=True)

        alpha_learn_logdir = Path('~/moos-ivp-learn/missions/alpha_learn').expanduser()

        # Build command
        cmd = [
            './launch.sh', '10',
            '--r8',
            f'--swimmers={self.args.swimmers}',
            '--uMayFinish',
            f'--max_db_uptime={self.args.max_db_uptime}',
            '--autodeploy',
            '--rescuebehavior=NeuralNetwork',
            f'--neural_network_dir={self.args.neural_network_dir}',
            '--r_sense_vehicles',
            f'--rescue_observation_radius={self.args.rescue_observation_radius}',
            f'--primary_behavior_weight={primary_weight}',
            f'--colregs_weight={colregs_weight}',
            f'--logdir={logdir}',
            '--nogui'
        ]

        # Execute command with learnKill first, then launch.sh in the correct directory
        with open(logdir / 'output.log', 'w') as logfile:
            subprocess.run(['learnKill'], cwd=alpha_learn_logdir, stdout=logfile, stderr=logfile)
            subprocess.run(cmd, cwd=alpha_learn_logdir, stdout=logfile, stderr=logfile)

def main():
    parser = argparse.ArgumentParser(description='Run parameter sweep for MOOS-IvP rescue behavior')
    parser.add_argument('log_directory', type=str,
                        help='Base directory to save log files')
    parser.add_argument('--num_stat_runs', '-n', type=int, default=50,
                        help='Number of statistical runs per parameter combination (default: 50)')
    parser.add_argument('--total_weight', type=int, default=450,
                        help='Total weight to distribute between behaviors (default: 450)')
    parser.add_argument('--weight_step', type=int, default=25,
                        help='Step size for weight increments (default: 25)')
    parser.add_argument('--swimmers', type=int, default=40,
                        help='Number of swimmers (default: 40)')
    parser.add_argument('--neural_network_dir', type=str,
                        default='/Users/ever/moos-ivp-learn/missions/alpha_learn/net-bv/2025_09_09/',
                        help='Directory containing neural network')
    parser.add_argument('--rescue_observation_radius', type=int, default=100,
                        help='Rescue observation radius (default: 100)')
    parser.add_argument('--max_db_uptime', type=int, default=600,
                        help='Moos timeout for mission evaluation in seconds (default: 600)')

    args = parser.parse_args()

    sweeper=Sweep(args)
    sweeper.run_combinations()

if __name__ == '__main__':
    main()
