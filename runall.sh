for file in data/testing/*
do
    echo $file
    build/instance_test "$file"
done

for file in data/phase1/*
do
    echo $file
    build/instance_test "$file"
done

for file in data/phase2/*
do
    echo $file
    build/instance_test "$file"
done
