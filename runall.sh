app="build/base_data_test"

for file in data/*.json
do
    echo $file
    $app "$file"
done
