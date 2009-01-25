#!/bin/sh

if [ "$3" = "" ]; then
    cat <<EOF
Usage: packagefont.sh fontfile.bdf platform uid [certificate] [key] [passphrase]
        fontfile.bdf    Font in BDF format. Target file will be named 
                        putty_font_fontfile_platform.sis[x]
        platform        Target platform: s60v1 or s60v3
                        Note that s60v1 also supports S60 2nd ed.
        uid             Package UID
        certificate     Certificate file, needed for s60v3
        key             Key file, needed for s60v3
        passphrase      Optional key passphrase for s60v3
EOF
    exit 1
fi

# Check platform
case $2 in
    s60v1) platform=s60v1
        ;;
    s60v3) platform=s60v3
        certificate=$4
        key=$5
        passphrase=$6
        if [ "$key" = "" ]; then
            echo No key and certificate specified
            exit 3
        fi
        ;;
    *) echo "Unsupported platform"
        exit 2
        ;;
esac

# Set up file names
fontfile=$1
fontname=`echo $fontfile | cut -d . -f 1`
if [ -z "$TEMP" ]; then
    TEMP=c:\temp
fi
s2ffile=$TEMP/$fontname.s2f
pkgfile=$TEMP/putty_font_${fontname}_${platform}.pkg
sisfile=putty_font_${fontname}_${platform}.sis

# Convert font
gawk -f bdftos2font.awk < $fontfile > $s2ffile

# Build package description file
rm -rf $pkgfile
printf "#{\"PuTTY Font %s\"},(0x%08x),1,0,0\r\n" $fontname $3 > $pkgfile
case $platform in
    s60v1)
        printf "(0x101F6F88), 0, 0, 0, {\"Series60ProductID\"}\r\n" >> $pkgfile
        printf "\"%s\"-\"!:\\\\system\\\\apps\\\\putty\\\\fonts\\\\%s.s2f\"\r\n" `cygpath -w $s2ffile` $fontname >> $pkgfile
        ;;
    s60v3)
        printf "[0x101F7961], 0, 0, 0, {\"Series60ProductID\"}\r\n" >> $pkgfile
        printf "%%{\"Petteri Kangaslampi\"}\r\n" >> $pkgfile
        printf ":\"Petteri Kangaslampi\"\r\n" >> $pkgfile
        printf "\"%s\"-\"c:\\\\system\\\\apps\\\\putty\\\\fonts\\\\%s.s2f\"\r\n" `cygpath -w $s2ffile` $fontname >> $pkgfile
        ;;
esac

# Build SIS package
makesis `cygpath -w $pkgfile` $sisfile
if [ "$platform" = "s60v3" ]; then
    signsis $sisfile ${sisfile}x $certificate $key $passphrase
    rm $sisfile
fi

# Clean up
rm $s2ffile $pkgfile
