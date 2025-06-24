for file in data/phase2/*
do
    echo $file
    build/instance_test "$file"
done
