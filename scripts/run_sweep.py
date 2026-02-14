#!/usr/bin/env python3

import argparse
import subprocess
from pathlib import Path
from tqdm import tqdm
import shutil

class Sweep():
    def __init__(self, args):
        self.args = args
        self.base_logdir = Path(self.args.log_directory).expanduser()
        self.timeout = self.args.max_db_uptime

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

    def run_stat_run(self, num, primary_weight, colregs_weight, retry_count=0):
        if retry_count >= self.args.max_retries:
            tqdm.write(f"ERROR: Maximum retries ({self.args.max_retries}) reached for trial {num}. Aborting.")
            raise Exception(f"Too many timeouts for trial {num}")

        # Figure out log directory
        logdir = self.base_logdir / f"pb_pwt_{primary_weight}_cr_pwt_{colregs_weight}" / f"trial_{num}"

        # Check if directory exists and has COMPLETE marker
        if logdir.exists():
            complete_file = logdir / 'COMPLETE'
            if complete_file.exists():
                # Trial already completed successfully, skip
                return

            # Directory exists but no COMPLETE file - need to handle timeout/retry
            timeout_file = logdir / 'TIMEOUT'
            if timeout_file.exists():
                # Move to timeouts folder
                timeout_base = self.base_logdir / 'timeouts' / f"pb_pwt_{primary_weight}_cr_pwt_{colregs_weight}"
                timeout_base.mkdir(parents=True, exist_ok=True)

                # Count existing timeout folders to determine next timeout number
                existing_timeouts = list(timeout_base.glob(f"trial_{num}_timeout_*"))
                next_timeout_num = len(existing_timeouts)

                timeout_dir = timeout_base / f"trial_{num}_timeout_{next_timeout_num}"
                logdir.rename(timeout_dir)
            else:
                # No TIMEOUT file, just remove the incomplete directory
                shutil.rmtree(logdir)

            # Retry
            return self.run_stat_run(num, primary_weight, colregs_weight, retry_count + 1)

        # Create log directory for this specific trial
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
        try:
            with open(logdir / 'output.log', 'w') as logfile:
                subprocess.run(['learnKill'], cwd=alpha_learn_logdir, stdout=logfile, stderr=logfile)
                result = subprocess.run(cmd, cwd=alpha_learn_logdir, stdout=logfile, stderr=logfile,
                                      timeout=self.timeout)

            # If we got here without timeout, mark as complete
            complete_file = logdir / 'COMPLETE'
            complete_file.touch()

        except subprocess.TimeoutExpired:
            # Timeout occurred, create TIMEOUT file
            timeout_file = logdir / 'TIMEOUT'
            timeout_file.touch()

            # Retry by calling recursively
            return self.run_stat_run(num, primary_weight, colregs_weight, retry_count + 1)

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
    parser.add_argument('--max_retries', type=int, default=10,
                        help='Maximum number of retries for any particular trial (default: 10)')

    args = parser.parse_args()

    sweeper=Sweep(args)
    sweeper.run_combinations()

if __name__ == '__main__':
    main()
