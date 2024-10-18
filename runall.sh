for file in data/phase1/*
do
    echo $file
    build/instance_test "$file"
done
