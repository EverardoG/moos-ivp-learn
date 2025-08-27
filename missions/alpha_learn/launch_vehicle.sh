#!/bin/bash
#--------------------------------------------------------------
#   Script: launch_vehicle.sh
#  Mission: rescue_baseline
#   Author: Michael Benjamin
#   LastEd: March 2025
#------------------------------------------------------------
#  Part 1: Set convenience functions for producing terminal
#          debugging output, and catching SIGINT (ctrl-c).
#------------------------------------------------------------
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }
on_exit() { echo; echo "$ME: Halting all apps"; kill -- -$$; }
trap on_exit SIGINT

#------------------------------------------------------------
#  Part 2: Declare global var defaults
#------------------------------------------------------------
ME=`basename "$0"`
CMD_ARGS=""
TIME_WARP=1
VERBOSE=""
JUST_MAKE=""
LOG_CLEAN="no"
AUTO_LAUNCHED="no"

IP_ADDR="localhost"
MOOS_PORT="9001"
PSHARE_PORT="9201"
SHORE_IP="localhost"
SHORE_PSHARE="9200"
MMOD=""

VNAME="abe"
COLOR="yellow"
XMODE="M300"
START_POS="x=0,y=0,heading=0"
STOCK_SPD="1.4"
MAX_SPD="2"

# Custom
START_POS="0,0"
SPEED="1.0"
RETURN_POS="5,0"
MAXSPD="2"
PGR="pGenRescue"
VUSER=""

TMATE=""
VROLE="rescue"
PRIMARY_BEHAVIOR="FollowCOM"
OBSERVATION_RADIUS=50
TRIM="no"
LOGDIR="./"
NOSTAMP="no"
R_SWIMMER_SECTORS=8
R_VEHICLE_SECTORS=8
R_SENSE_VEHICLES="no"
DIAMOND_PATTERN="14.82,-11.42:-17.51,-59.95:40.52,-65.63:72.85,-17.11"

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    CMD_ARGS+=" ${ARGI}"
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "$ME [OPTIONS] [time_warp]                        "
	echo "                                                 "
	echo "Options:                                         "
	echo "  --help, -h             Show this help message  "
	echo "  --just_make, -j        Only create targ files  "
	echo "  --verbose, -v          Verbose, confirm launch "
	echo "  --log_clean, -lc       Run clean.sh bef launch "
    echo "  --auto, -a             Script launch, no uMAC  "
	echo "                                                 "
	echo "  --ip=<localhost>       Veh Default IP addr     "
	echo "  --mport=<9001>         Veh MOOSDB port         "
	echo "  --pshare=<9201>        Veh pShare listen port  "
	echo "  --shore=<localhost>    Shoreside IP to try     "
	echo "  --shore_pshare=<9200>  Shoreside pShare port   "
    echo "  --mmod=<mod>           Mission variation/mod   "
	echo "                                                 "
	echo "  --vname=<abe>          Veh name given          "
	echo "  --color=<yellow>       Veh color given         "
	echo "  --sim, -s              Sim, not fld vehicle    "
	echo "  --start_pos=<X,Y,Hdg>  Sim start pos/hdg       "
	echo "  --stock_spd=<m/s>      Default vehicle speed   "
	echo "  --max_spd=<m/s>        Max Sim and Helm speed  "
	echo "                                                 "
	echo "Options (custom):                                "
	echo "  --speed=meters/sec                             "
	echo "    The speed use for transiting/loitering       "
	echo "  --maxspd=meters/sec                            "
	echo "    Max speed of vehicle (for sim and in-field)  "
	echo "  --vrole=<rescue>, scout, fixed                 "
	echo "    Vehicle role, either rescue, scout, or fixed "
    echo "  --observation_radius=<meters>                  "
    echo "    Observation radius for vehicle. How far the  "
    echo "    vehicle can \"see\"                          "
    echo "  --primarybehavior=<behavior_choice>            "
    echo "    Which behavior should be the primary behavior"
    echo "    that the vehicle is running                  "
    echo "    Choices for rescue vehicle:                  "
    echo "      FollowCOM, MaxReading, NeuralNetwork       "
    echo "        FollowCOM (Default)                      "
    echo "          Go to center of sensed swimmers        "
    echo "        MaxReading                               "
    echo "          Go to sector with highest reading of   "
    echo "          sensed swimmers                        "
    echo "        NeuralNetwork                            "
    echo "          Use neural network to map sectors to a "
    echo "          desired heading and velocity           "
    echo "    Choices for scout vehicle not implemented    "
    echo "  --neural_network_config=<csv_directory>        "
    echo "    Directory to the csv file containing neural  "
    echo "    network configuration for NeuralNetwork      "
    echo "    behavior                                     "
    echo "  --r_swimmer_sectors=<N>                        "
    echo "    Number of swimmer sectors for rescue vehicles"
    echo "    (default: 8)                                 "
    echo "  --r_sense_vehicles                             "
    echo "    Enable vehicle sensing for rescue vehicles.  "
    echo "    When set, implies --r_vehicle_sectors with   "
    echo "    default number                               "
    echo "  --r_vehicle_sectors=<N>                        "
    echo "    Number of vehicle sectors for rescue vehicles"
    echo "    (default: 8). When set, implies              "
    echo "    --r_sense_vehicles                           "
    echo "  --trim, -t           Trim logging for learning "
    echo "  --logdir, -ld        Directory to save log info"
    echo "  --nostamp       Do not include timestamp       "
    echo "                  in directory names for logs    "
	echo "  --tmate=<vname>                                "
	echo "    Name of the teammate vehicle if applicable   "
	echo "  --pgr=<app>                                    "
	echo "    Full path of version of pGenRescue           "
	echo "  --vuser=<vuser>                                "
	echo "    Name of the user if competing                "
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
        VERBOSE="yes"
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--log_clean" -o "${ARGI}" = "-lc" ]; then
	LOG_CLEAN="yes"
    elif [ "${ARGI}" = "--auto" -o "${ARGI}" = "-a" ]; then
        AUTO_LAUNCHED="yes"

    elif [ "${ARGI:0:5}" = "--ip=" ]; then
        IP_ADDR="${ARGI#--ip=*}"
    elif [ "${ARGI:0:7}" = "--mport" ]; then
	MOOS_PORT="${ARGI#--mport=*}"
    elif [ "${ARGI:0:9}" = "--pshare=" ]; then
        PSHARE_PORT="${ARGI#--pshare=*}"
    elif [ "${ARGI:0:8}" = "--shore=" ]; then
        SHORE_IP="${ARGI#--shore=*}"
    elif [ "${ARGI:0:15}" = "--shore_pshare=" ]; then
        SHORE_PSHARE="${ARGI#--shore_pshare=*}"
    elif [ "${ARGI:0:7}" = "--mmod=" ]; then
        MMOD="${ARGI#--mmod=*}"

    elif [ "${ARGI:0:8}" = "--vname=" ]; then
        VNAME="${ARGI#--vname=*}"
    elif [ "${ARGI:0:8}" = "--color=" ]; then
        COLOR="${ARGI#--color=*}"
    elif [ "${ARGI}" = "--sim" -o "${ARGI}" = "-s" ]; then
        XMODE="SIM"
    elif [ "${ARGI:0:12}" = "--start_pos=" ]; then
        START_POS="${ARGI#--start_pos=*}"
    elif [ "${ARGI:0:12}" = "--stock_spd=" ]; then
        STOCK_SPD="${ARGI#--stock_spd=*}"
    elif [ "${ARGI:0:10}" = "--max_spd=" ]; then
        MAX_SPD="${ARGI#--max_spd=*}"

    elif [ "${ARGI:0:21}" = "--observation_radius=" ]; then
        OBSERVATION_RADIUS="${ARGI#--observation_radius=*}"
    elif [ "${ARGI:0:8}" = "--vrole=" ]; then
        VROLE="${ARGI#--vrole=*}"
    elif [ "${ARGI:0:18}" = "--primarybehavior=" ]; then
        PRIMARY_BEHAVIOR="${ARGI#--primarybehavior=*}"
    elif [ "${ARGI:0:23}" = "--neural_network_config" ]; then
        NEURAL_NETWORK_CONFIG="${ARGI#--neural_network_config=*}"
    elif [ "${ARGI:0:20}" = "--r_swimmer_sectors=" ]; then
        R_SWIMMER_SECTORS="${ARGI#--r_swimmer_sectors=*}"
    elif [ "${ARGI}" = "--r_sense_vehicles" ]; then
        R_SENSE_VEHICLES="yes"
    elif [ "${ARGI:0:20}" = "--r_vehicle_sectors=" ]; then
        R_VEHICLE_SECTORS="${ARGI#--r_vehicle_sectors=*}"
        R_SENSE_VEHICLES="yes"

    elif [ "${ARGI}" = "--trim" -o "${ARGI}" = "-t" ]; then
	    TRIM="yes"
    elif [[ "${ARGI}" = --logdir=* ]]; then
        LOGDIR="${ARGI#--logdir=}"
    elif [ "${ARGI}" = "--nostamp" ]; then
	    NOSTAMP="yes"

    elif [ "${ARGI:0:8}" = "--tmate=" ]; then
        TMATE="${ARGI#--tmate=*}"
    elif [ "${ARGI:0:6}" = "--pgr=" ]; then
        PGR="${ARGI#--pgr=*}"
    elif [ "${ARGI:0:8}" = "--vuser=" ]; then
        VUSER="${ARGI#--vuser=*}"

    else
	echo "$ME: Bad Arg:[$ARGI]. Exit Code 1."
	exit 1
    fi
done

#------------------------------------------------------------
#  Part 4: If Heron hardware, set key info based on IP address
#------------------------------------------------------------
if [ "${XMODE}" = "M300" ]; then
    COLOR=`get_heron_info.sh --color --hint=$COLOR`
    IP_ADDR=`get_heron_info.sh --ip`
    FSEAT_IP=`get_heron_info.sh --fseat`
    VNAME=`get_heron_info.sh --name`
    if [ $? != 0 ]; then
	echo "$ME: Problem getting Heron Info. Exit Code 2"
	exit 2
    fi
fi

#--------------------------------------------------------------
#  Part 4B: If VROLE is scout, ensure a teammate is specified
#--------------------------------------------------------------
if [ "${VROLE}" = "scout" ]; then
    if [ "${TMATE}" = "" -o "${TMATE}" = "${VNAME}" ]; then
	echo "Scouts must declare a distinct teammate. Exit Code 3."
	exit 3
    fi
fi

#---------------------------------------------------------------
#  Part 4: If verbose, show vars and confirm before launching
#---------------------------------------------------------------
if [ "${VERBOSE}" = "yes" ]; then
    echo "============================================"
    echo "     launch_vehicle.sh SUMMARY        $VNAME"
    echo "============================================"
    echo "$ME                               "
    echo "CMD_ARGS =      [${CMD_ARGS}]     "
    echo "TIME_WARP =     [${TIME_WARP}]    "
    echo "JUST_MAKE =     [${JUST_MAKE}]    "
    echo "AUTO_LAUNCHED = [${AUTO_LAUNCHED}]"
    echo "LOG_CLEAN =     [${LOG_CLEAN}]    "
    echo "----------------------------------"
    echo "IP_ADDR =       [${IP_ADDR}]      "
    echo "MOOS_PORT =     [${MOOS_PORT}]    "
    echo "PSHARE_PORT =   [${PSHARE_PORT}]  "
    echo "SHORE_IP =      [${SHORE_IP}]     "
    echo "SHORE_PSHARE =  [${SHORE_PSHARE}] "
    echo "MMOD =          [${MMOD}]         "
    echo "----------------------------------"
    echo "VNAME =         [${VNAME}]        "
    echo "COLOR =         [${COLOR}]        "
    echo "XMODE =         [${XMODE}]        "
    echo "------------Sim-------------------"
    echo "START_POS =     [${START_POS}]    "
    echo "STOCK_SPD =     [${STOCK_SPD}]    "
    echo "MAX_SPD =       [${MAX_SPD}]      "
    echo "------------Fld-------------------"
    echo "FSEAT_IP =      [${FSEAT_IP}]     "
    echo "------------Custom----------------"
    echo "OBSERVATION_RADIUS = [${OBSERVATION_RADIUS}]"
    echo "VROLE =         [${VROLE}]        "
    echo "PRIMARY_BEHAVIOR = [${PRIMARY_BEHAVIOR}] "
    echo "TMATE =         [${TMATE}]        "
    echo "PGR =           [${PGR}]          "
    echo "VUSER =         [${VUSER}]        "
    echo "R_SWIMMER_SECTORS = [${R_SWIMMER_SECTORS}]"
    echo "R_SENSE_VEHICLES =  [${R_SENSE_VEHICLES}] "
    echo "R_VEHICLE_SECTORS = [${R_VEHICLE_SECTORS}]"
    echo "----------------------------------"
    echo "TRIM =          [${TRIM}]         "
    echo "LOGDIR =        [${LOGDIR}]       "
    echo "NOSTAMP =       [${NOSTAMP}]      "
    echo "----------------------------------"
    echo -n "Hit any key to continue launching $VNAME "
    read ANSWER
fi

#------------------------------------------------------------
#  Part 6: If Log clean before launch, do it now.
#------------------------------------------------------------
if [ "$LOG_CLEAN" = "yes" -a -f "clean.sh" ]; then
    vecho "Cleaning local Log Files"
    ./clean.sh
fi

# Check if LOGDIR exists, create it if it doesn't
if [ ! -d "$LOGDIR" ]; then
    vecho "Creating log directory: $LOGDIR"
    mkdir -p "$LOGDIR"
    if [ $? -ne 0 ]; then
        echo "$ME: Error creating log directory $LOGDIR. Exit Code 4."
        exit 4
    fi
fi

#------------------------------------------------------------
#  Part 7: Create the .moos and .bhv files.
#------------------------------------------------------------
NSFLAGS="--strict --force"
if [ "${AUTO_LAUNCHED}" = "no" ]; then
    NSFLAGS="--interactive --force"
fi

echo $TRIM

nsplug meta_vehicle.moos targ_$VNAME.moos $NSFLAGS WARP=$TIME_WARP \
       IP_ADDR=$IP_ADDR             MOOS_PORT=$MOOS_PORT \
       PSHARE_PORT=$PSHARE_PORT     SHORE_IP=$SHORE_IP   \
       SHORE_PSHARE=$SHORE_PSHARE   VNAME=$VNAME         \
       COLOR=$COLOR                 XMODE=$XMODE         \
       START_POS=$START_POS         MAX_SPD=$MAX_SPD     \
       MMOD=$MMOD                                        \
       VROLE=$VROLE                 TMATE=$TMATE         \
       PGR=$PGR                     VUSER=$VUSER         \
       FSEAT_IP=$FSEAT_IP           TRIM=$TRIM           \
       LOGDIR=$LOGDIR               NOSTAMP=$NOSTAMP     \
       OBSERVATION_RADIUS=$OBSERVATION_RADIUS \
       SWIMMER_SECTORS=$R_SWIMMER_SECTORS \
       VEHICLE_SECTORS=$R_VEHICLE_SECTORS \
       R_SENSE_VEHICLES=$R_SENSE_VEHICLES

nsplug meta_vehicle.bhv targ_$VNAME.bhv $NSFLAGS         \
       START_POS=$START_POS         VNAME=$VNAME         \
       STOCK_SPD=$STOCK_SPD         MMOD=$MMOD           \
       COLOR=$COLOR                 VROLE=$VROLE         \
       TMATE=$TMATE                 PRIMARY_BEHAVIOR=$PRIMARY_BEHAVIOR \
       NEURAL_NETWORK_CONFIG=$NEURAL_NETWORK_CONFIG \
       SWIMMER_SECTORS=$R_SWIMMER_SECTORS \
       VEHICLE_SECTORS=$R_VEHICLE_SECTORS \
       R_SENSE_VEHICLES=$R_SENSE_VEHICLES \
       DIAMOND_PATTERN=$DIAMOND_PATTERN

if [ "${JUST_MAKE}" = "yes" ]; then
    echo "$ME: Targ files made; exiting without launch."
    exit 0
fi

#------------------------------------------------------------
#  Part 8: Launch the vehicle mission
#------------------------------------------------------------
echo "Launching $VNAME MOOS Community. WARP="$TIME_WARP
pAntler targ_$VNAME.moos >& /dev/null &
echo "Done Launching $VNAME MOOS Community"

#------------------------------------------------------------
#  Part 9: If launched from script, we're done, exit now
#------------------------------------------------------------
if [ "${AUTO_LAUNCHED}" = "yes" ]; then
    exit 0
fi

#------------------------------------------------------------
# Part 10: Launch uMAC until the mission is quit
#------------------------------------------------------------
uMAC targ_$VNAME.moos
trap "" SIGINT
kill -- -$$
