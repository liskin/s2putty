#** @file rawtos2font.awk
#*
#* A converter that creates s2font bitmap font files from BDF font files.
#*
#* Copyright 2005 Petteri Kangaslampi
#*
#* See license.txt for full copyright and license information.

BEGIN {
    inChar = 0;
    inBitmap = 0;
    encoding = -1;
    numChars = 0;
}


END {
    WriteU32(0x53324f4e);
    WriteU32(0x00010000);
    WriteU32(numChars);
    WriteU32(width);
    WriteU32(height);
    printf("UCS-2\0");
    for ( n = 0; n < numChars; n++ ) {
        c = chars[n];
        WriteU16(c);
        for ( i = 0; i < height; i++ ) {
            if ( width < 9 ) {
                WriteU8(bitmap[c,i]);
            } else {
                WriteU16(bitmap[c,i]);
            }    
        }
    }
}


$1 == "PIXEL_SIZE" {
    height = $2;
    next;
}

$1 == "FONTBOUNDINGBOX" {
    width = $2;
    if ( width > 16 ) {
        printf("Maximum supported width is 16 pixels\n") > "/dev/stderr";
        exit 1;
    }
    next;
}

$1 == "STARTCHAR" {
    inChar = 1;
    next;
}

$1 == "ENCODING" {
    encoding = $2;
    chars[numChars] = encoding;
    numChars++;
    next;
}

$1 == "DWIDTH" {
    if ( $2 != width ) {
        printf("Not monospaced! Char %d, DWIDTH %d, width %d\n",
               encoding, $2, width) > "/dev/stderr";
        exit 1;
    }
}

$1 == "BITMAP" {
    if ( !inChar || (encoding == -1) ) {
        printf("Bad BITMAP\n") > "/dev/stderr";
        exit 1;
    }
    inBitmap = 1;
    bitmapRow = 0;
    next;
}

$1 == "ENDCHAR" {
    if ( !inBitmap ) {
        printf("Bad ENDCHAR\n") > "/dev/stderr";
        exit 1;
    }
    if ( bitmapRow != height ) {
        printf("Char %d has only %d rows\n", bitmapRow) > "/dev/stderr";
        exit 1;
    }
    inBitmap = 0;
    inChar = 0;
    encoding = -1;
    next;
}

inBitmap {
    bitmap[encoding,bitmapRow] = strtonum("0x"$1);
    bitmapRow++;
}


# Write an unsigned 32-bit value to stdout in network byte order
function WriteU32(val) {
    printf("%c%c%c%c",
           and(rshift(val, 24), 255),
           and(rshift(val, 16), 255),
           and(rshift(val, 8), 255),
           and(val, 255));
}

# Write an unsigned 16-bit value to stdout in network byte order
function WriteU16(val) {
    printf("%c%c",
           and(rshift(val, 8), 255),
           and(val, 255));
}

# Write an unsigned 16-bit value to stdout in network byte order
function WriteU8(val) {
    printf("%c", and(val, 255));
}
