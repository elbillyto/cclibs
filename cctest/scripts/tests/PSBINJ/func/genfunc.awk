#! /usr/bin/awk -f 
#
# genfunc max 
#
BEGIN {

    if(ARGC != 2)
    {
        print "usage: genfunc max"
        exit -1
    }

    max = ARGV[1]
    pi = 3.14159265359

    # Calcualte function 5ms + 2ms + 5ms

    for(i=0 ; i <= 50 ; i++)
    {
        time[i] = i * 1.e-4
        ref [i] = max * 0.5 * (1 - cos(pi * time[i] / 5e-3))   
    }

    for(j=0 ; j <= 50 ; j++)
    {
        time[i] = (70 + j) * 1.e-4
        ref [i] = max * 0.5 * (1 + cos(pi * j / 50))   
        i++
    }

    time[i] = time[i-1] + 2e-3
    ref[i]  = 0


    print "# Function to " max " A\n"

    printf "TABLE TIME "

    for(j = 0 ; j <= i ; j++) printf "%.4f ", time[j]

    printf "\nTABLE REF  "

    for(j = 0 ; j <= i ; j++) printf "%.4f ", ref[j]

    print "\n\n# EOF"

}
# EOF
