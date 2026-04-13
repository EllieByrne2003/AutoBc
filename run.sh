#!/bin/bash

# Check command line arguments
if [ $# -ne 2 ]; then
    echo "Usage: $0 <final_format> <priority>"
    echo "  custom_commands : semicolon-separated list of ABC commands, e.g., 'resyn2; dc2; print_stats'"
    exit 1
fi

final_format="$1"
priority="$2"

# List of circuit names to process (hardcoded)
circuits=(
    "i10"
    "adder"
    "bar"
    "max"
    "multiplier"
    "log2"
    "div"
    "sin"
    "sqrt"
)

# Loop through each circuit
for circuit_name in "${circuits[@]}"; do
    echo "========================================="
    echo "Processing circuit: $circuit_name"
    echo "========================================="

    log_file="resources/logs/${circuit_name}_log.txt"
    circuit_file="resources/circuits/${circuit_name}.aig"

    # Step 1: Run autobc, filter output, and save log
    echo "Running autobc for circuit: $circuit_name"
    ./bin/autobc ${circuit_file} -s 1024 -l 1 -t 16 -g 100 -p ${priority} -f ${final_format} | grep "Generation" -A 6 --line-buffered > "$log_file"

    # Check if log file was created and has content
    if [ ! -s "$log_file" ]; then
        echo "Warning: Failed to generate log or log is empty for $circuit_name. Skipping."
        continue
    fi

    # Step 2: Find the line number of the last occurrence of "Generation"
    last_gen_line=$(grep -n "^Generation" "$log_file" | tail -1 | cut -d: -f1)

    if [ -z "$last_gen_line" ]; then
        echo "Warning: No 'Generation' line found in log for $circuit_name. Skipping."
        continue
    fi

    # Step 3: Extract the chromosome from the block starting at that line (next 6 lines)
    chromosome=$(sed -n "${last_gen_line},+6p" "$log_file" | grep -oP '^\s*Chromosone:\s*\K.*' | head -1)

    if [ -z "$chromosome" ]; then
        echo "Warning: Could not extract chromosome from the last generation block for $circuit_name. Skipping."
        continue
    fi

    echo "Extracted chromosome: $chromosome"

    # Step 4: Run the two test scripts with the circuit name and chromosome
    echo "Running testAig.sh for $circuit_name..."
    bash ./testAig.sh ${circuit_name} "$chromosome"

    echo "Running testLut6.sh for $circuit_name..."
    bash ./testLut6.sh ${circuit_name} "$chromosome"

    echo "Finished processing $circuit_name"
    echo ""
done

echo "All circuits processed."