#!/bin/bash

formats=(
    "logic-6"
    "aig"
)

priorities=(
    "gates"
    "levels"
)

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

for format in "${formats[@]}"; do
    for priority in "${priorities[@]}"; do
        echo "=============================================="
        echo "Optimizing for $priority in $format format"
        echo "=============================================="

        for circuit_name in "${circuits[@]}"; do
            echo "    =========================================="
            echo "    Processing circuit: $circuit_name"
            echo "    =========================================="

            dir_name="resources/logs/${format}/${priority}"
            mkdir -p $dir_name
            log_file="${dir_name}/${circuit_name}.log"
            circuit_file="resources/circuits/${circuit_name}.aig"

            echo ""
            echo "        Running autobc for circuit: $circuit_name"
            ./bin/autobc ${circuit_file} -s 1024 -l 3 -t 16 -g 100 -p ${priority} -f ${format} | grep "Generation" -A 6 --line-buffered > "$log_file"

            if [ ! -s "$log_file" ]; then
                echo "        Warning: Failed to generate log or log is empty for $circuit_name. Skipping."
                continue
            fi

            last_gen_line=$(grep -n "^Generation" "$log_file" | tail -1 | cut -d: -f1)

            if [ -z "$last_gen_line" ]; then
                echo "        Warning: No 'Generation' line found in log for $circuit_name. Skipping."
                continue
            fi

            chromosome=$(sed -n "${last_gen_line},+6p" "$log_file" | grep -oP '^\s*Chromosone:\s*\K.*' | head -1)
            echo "        Extracted chromosome: $chromosome"

            echo "        Running testAig.sh for $circuit_name..."
            bash ./testAig.sh ${circuit_name} "$chromosome"

            echo "        Running testLut6.sh for $circuit_name..."
            bash ./testLut6.sh ${circuit_name} "$chromosome"

            echo "        Finished processing $circuit_name"
            echo ""
        done
    done
done
        
echo "=============================================="
echo "All circuits processed in all formats and for all priorities."
echo "=============================================="