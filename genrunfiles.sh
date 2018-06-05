

# Output file name
output=$1

# Verify if the output file already exists and asks for deletion
if [ -f runfiles/$output ]; then
   rm -i runfiles/$output
fi

MAXEVALS=10000000

for m in 100
do
    for d in 5 10 30
    do
        for n in 100 250 500
        do
            # GroupMutation: 0 - RGmutation, 1 - RGshuffle, 3 - NOmutation
            for gm in 0 1 3
            do
                # decimal cases to create the efficiencies groups
                for dc in 1 2
                do
                    for i in `seq -w 0 29`
                    do
                        file="${d}-${n}-$i-$gm-$dc-$MAXEVALS"
                        # Number of experiment repetitions
                        for k in `seq 1 30`
                        do
                            echo "nohup ./cbga -i instances/chu-beasley/${d}.${n}-$i -n $m -m $gm -d $dc -e $MAXEVALS >> output/$file.out " >> runfiles/$output
                        done
                    done
                done
            done
        done
    done
done
