#!/usr/bin/awk -f
#
# chart.awk
# High Luminosity LHC Squeeze analysis
#
# This script will combine the specified signal from all the charts
# from each squeeze into one Flot chart.
# --------------------------------------------------------------------------

BEGIN {

# Process arguments as filenames

    if(ARGC < 3)
    {
        print "usage: gawk -f squeeze.awk SIGNAL CSV_Filename..."
        exit -1
    }

    signal = ARGV[1]

    print "--------------------------------------"
    print "Creating charts for signal " signal

    for(file_idx = 2; file_idx < ARGC ; file_idx++)
    {
        n = split(ARGV[file_idx], path, "/")
        split(path[n], filename, ".")

        Chart(filename[1], ARGV[file_idx], "results/results-" filename[1] ".csv")
    }

    exit
}

# --------------------------------------------------------------------------

function GetHeading(inputfile, csvfile, expected)
{
    getline < inputfile

    if($1 != expected)
    {
        printf "Error in %s : expected heading '%s' and read '%s'\n", inputfile, expected, $1
        exit -1
    }
}

# --------------------------------------------------------------------------

function ChartHeader(inputfile, outputfile, signal, first_f)
{
    do
    {
        if((getline < inputfile) <= 0)
        {
            print "Error: unexpected end of " inputfile
            exit -1
        }

        if(first_f)
        {
            print $0 > outputfile
        }

    } while($2 != "analog_signals")
}

# --------------------------------------------------------------------------

function ChartData(inputfile, outputfile, signal, circuitname)
{
    do
    {
        if((getline < inputfile) <= 0)
        {
            print "Error: unexpected end of " inputfile
            exit -1
        }

        if(index($1, signal) > 0)
        {
            sub(signal, circuitname)
            print $0 > outputfile
            getline < inputfile
            print $0 > outputfile
            print " }," > outputfile

            return
        }
    } while($2 != "digital_signals")

    print "Error: unknown signal " signal
}

# --------------------------------------------------------------------------

function ChartFooter(inputfile, outputfile, signal, circuitname)
{
    do
    {
        if((getline < inputfile) <= 0)
        {
            print "Error: unexpected end of " inputfile
            exit -1
        }
    } while($2 != "OnLoad()")

    print "};\n    var digital_signals = {};\n\n" $0 > outputfile

    while((getline < inputfile) > 0)
    {
        sub("analog_chart_div'  style='width:95%;height:65", "analog_chart_div'  style='width:95%;height:100")
        print > outputfile
    }
}

# --------------------------------------------------------------------------

function Chart(testname, inputfile, csvfile)
{
    print "Processing: " testname

    # Read in header lines

    FS = ","

    GetHeading(inputfile, csvfile, "HENRYS")
    split($0, henrys)
    GetHeading(inputfile, csvfile, "OHMS_SER")
    split($0, ohms_ser)
    GetHeading(inputfile, csvfile, "V_POS")
    split($0, v_pos)
    GetHeading(inputfile, csvfile, "V_NEG")
    split($0, v_neg)
    GetHeading(inputfile, csvfile, "I_POS")
    split($0, i_pos)
    GetHeading(inputfile, csvfile, "I_NEG")
    split($0, i_neg)
    GetHeading(inputfile, csvfile, "kmax/nominal")
    split($0, kmax)

    getline < inputfile
    split($0, circuit)

    max_cols = NF

    close(intputfile)

    # Read each Flot chart

    FS = " "

    outputfile = "../../../results/html/tests/HL_LHC/" testname "-" signal ".html"

    print "Generating: " outputfile

    for(c = 2 ; c <= max_cols ; c++)
	{
        inputfile = "../../../results/html/tests/HL_LHC/" testname "-" circuit[c] ".html"

        first_f = (c == 2)

        ChartHeader(inputfile, outputfile, signal, first_f)

        ChartData(inputfile, outputfile, signal, circuit[c])

        if(c == max_cols)
        {
            ChartFooter(inputfile, outputfile)
        }

        close(inputfile)
	}

    close(outputfile)
}
# EOF
