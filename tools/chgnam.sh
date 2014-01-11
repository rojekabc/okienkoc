#!/bin/sh

. /usr/local/share/projbuild/color.shfun;
. /usr/local/share/projbuild/vendor.shfun;

FILTR=
ISTEST="true";

colortext YELLOW "Starting [Test mode]";

while ! test -z $1; do
	case $1 in
		-help | -h | --help )
			echo -n "Usage: ";
			colortext -n GREEN $PROGNAME;
			echo " <command>";
			echo "	-nt";
			echo "		Switch to no test mode - Change real files";
			echo "	-t";
			echo "		Switch to test mode - Simulate operation";
			echo "	-f 'filte'";
			echo "		User parameter for file filtering";
			echo "	-r 'one' 'two'";
			echo "		Replace firt occurence of substring from one to two";
			echo "	-ra 'one' 'two'";
			echo "		Replace all occurence of substring one to two";
			echo "";
			echo -n "				Copyright @ ";
			colortext CYAN "$FUN_VENDOR_AUTHOR";
			exit 0;
		;;
		-nt | -notest )
			ISTEST="false";
			colortext YELLOW "Switching to NO TEST mode !";
		;;
		-t | -test)
			ISTEST="true";
			colortext YELLOW "Switching to Test mode !";
		;;
		-f | -filtr )
			shift; FILTR="$1";
		;;
		-r | -replace | -ra | -replaceall )
			COMMAND=$1;
			colortext GREEN "Operation: Replacing";
			shift; REPLACEFROM="$1";
			shift; REPLACETO="$1";

			SAVEIFS=$IFS;
			IFS=$'\n';
			for i in `ls $FILTR`; do
				OLDNAME="$i";
				case $COMMAND in
					-r | -replace )
					NEWNAME="${i/$REPLACEFROM/$REPLACETO}";
					;;
					-ra | -replaceall )
					NEWNAME="${i//$REPLACEFROM/$REPLACETO}";
					;;
				esac;
				if test "$NEWNAME" != "$OLDNAME"; then
					echo -n "$OLDNAME";
				       	colortext -n GREEN " -> ";
					echo "$NEWNAME";
					if test "$ISTEST" = "false"; then
						mv "$OLDNAME" "$NEWNAME";
					fi
				fi
			done;
			IFS=$SAVEIFS;
		;;
		*)
			echo "Unknown operation [$1]";
		;;
	esac;
	shift;
done
