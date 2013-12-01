#!/bin/bash

enginedir=/Users/user/Source/mokoi-code/luxengine
resourcesdir=/Users/user/Source/mokoi-code/MokoiGaming
output_dir=/Users/user/Source/
frameworksdir=/Users/user/Library/Frameworks

rm -R $output_dir/MokoiGaming.app

mkdir -p $output_dir/MokoiGaming.app/Resources
mkdir -p $output_dir/MokoiGaming.app/Frameworks
mkdir -p $output_dir/MokoiGaming.app/Contents/share/mokoigaming-1.0/

cp $resourcesdir/bin/luxengine $output_dir/MokoiGaming.app/Contents/MacOS/
cp $enginedir/res/Info.plist $output_dir/MokoiGaming.app/Contents/
cp $enginedir/res/luxengine.icns $output_dir/MokoiGaming.app/Contents/Resources/
cp $resourcesdir/share/mokoigaming-1.0/portal.css $output_dir/MokoiGaming.app/Contents/share/mokoigaming-1.0/





function do_strip {
    tp=$(file -b --mime-type "$1")

    if [ "$tp" != "application/octet-stream" ]; then
        return
    fi

    name=$(mktemp -t bundle)
    st=$(stat -f %p "$1")
    
    strip -o "$name" -S "$1"
    mv -f "$name" "$1"

    chmod "$st" "$1"
    chmod u+w "$1"
}


echo "Strip debug symbols from bundle binaries"

# Strip debug symbols
for i in $(find -E $output_dir"MokoiGaming.app/Contents/Frameworks" -type f -regex '.*\.(so|dylib)$'); do
    do_strip "$i"
done


do_strip $output_dir"MokoiGaming.app/Contents/MacOS/luxengine"




