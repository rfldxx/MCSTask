#!/bin/bash

PATH_TO_TESTS='tests/'


rm -rf "$PATH_TO_TESTS"tests_obtain
mkdir  "$PATH_TO_TESTS"tests_obtain

counter=0

# Используем цикл for для итерации по каждому файлу в директории
for file in "$PATH_TO_TESTS"tests_in/*
do
    if [ -f "$file" ]; then
        filename=$(basename "$file")  # Получаем только имя файла

        
        ./main < "$PATH_TO_TESTS"tests_in/$filename > "$PATH_TO_TESTS"tests_obtain/$filename

        diff_result=$(diff -r "$PATH_TO_TESTS"tests_expect/"$filename" "$PATH_TO_TESTS"tests_obtain/"$filename")
        if [ -n "$diff_result" ]; then
            echo "Diff  in  $filename"
            counter=$(( $counter + 1 ))
        fi

    fi
done


if [[ counter -eq 0 ]]; then 
    echo -e "\033[92mALL GOOD\033[0m"
else
    echo -e "\033[91mTotal ERRORs:\033[0m $counter"
fi

