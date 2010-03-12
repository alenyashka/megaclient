#!/bin/sh
#
#

APPNAME="megaclient"

BUILDDATE=$(date +%F)

MAJORVERSION=`sed '/^\#/d' version.properties | grep 'major'  | tail -n 1 | sed 's/^.*=//;s/^[[:space:]]*//;s/[[:space:]]*$//'`
MINORVERSION=`sed '/^\#/d' version.properties | grep 'minor'  | tail -n 1 | sed 's/^.*=//;s/^[[:space:]]*//;s/[[:space:]]*$//'`

echo "Version: $MAJORVERSION.$MINORVERSION"
echo "Build date: $BUILDDATE"

BASEDIR="./"
DEPLOYDIR="deploy/$APPNAME-$MAJORVERSION.$MINORVERSION"
PLATFORMDIR="linux"
BUILDNAME="$APPNAME-$MAJORVERSION.$MINORVERSION"
RELEASEDIR="$DEPLOYDIR/$PLATFORMDIR/$BUILDNAME"

echo "Build dir: $RELEASEDIR"

rm -rf $DEPLOYDIR/$PLATFORMDIR/

mkdir -p "$DEPLOYDIR/$PLATFORMDIR"
git clone ./ $RELEASEDIR
cd $RELEASEDIR

#sed -i "s/%test-major-version%/$MAJORVERSION/g" version.cpp
#sed -i "s/%test-minor-version%/$MINORVERSION/g" version.cpp
#sed -i "s/%test-date%/$BUILDDATE/g" version.cpp
#sed -i "s/QApplication::applicationDirPath()/\"\/usr\/share\/yourownnewsmaker\"/g" apppaths.cpp

rm -rf *.user
rm -rf *.sh
rm -rf Makefile
rm -rf *.properties

mkdir src1
mv src src1
mv src1 src
mv $APPNAME.pro src/src.pro
mv *.qrc src
mv images src
mv l10n src
mv $APPNAME.desktop src
mv changes src
mv license src

touch $APPNAME.pro
echo "QMAKEVERSION = $$[QMAKE_VERSION]" >> $APPNAME.pro
echo "ISQT4 = $$find(QMAKEVERSION, ^[2-9])" >> $APPNAME.pro
echo "isEmpty( ISQT4 ) {" >> $APPNAME.pro
echo "error(\"Use the qmake include with Qt4.4 or greater, on Debian that is qmake-qt4\");" >> $APPNAME.pro
echo "}" >> $APPNAME.pro
echo "" >> $APPNAME.pro
echo "TEMPLATE = subdirs" >> $APPNAME.pro
echo "SUBDIRS  = src" >> $APPNAME.pro

#PREFIX=/usr

#qmake linux.pro -spec linux-g++ -r 
#checkinstall
#touch package.sh
#echo "#!/bin/bash" >> package.sh
#echo "set -e" >> package.sh
#echo "" >> package.sh
#echo "debuild -us -uc" >> package.sh
#echo "debuild -S -us -uc" >> package.sh
#echo "su -c pbuilder build ../helloworld_1.0.dsc" >> package.sh
#chmod +x package.sh

export DEBFULLNAME="Alexander O. Anisimov"
#dh_make -e  eeexception@gmail.com -n -s -c gpl
dh_make -e anisimov2004@gmail.com --createorig --single -c gpl

mkdir debian
mv rules debian/rules
mv control debian/control
mv changelog debian/changelog

sed -i "s/<url:\/\/example.com>/http:\/\/google.com/g" debian/copyright
sed -i "s/<put author's name and email here>/Alexander O. Anisimov <anisimov2004@gmail.com>/g" debian/copyright
sed -i "s/<likewise for another author>//g" debian/copyright
sed -i "s/Copyright (C) YYYY Firstname Lastname/Copyright (C) 2010 Alexander O. Anisimov/g" debian/copyright

dpkg-buildpackage 
#create src
rm -rf debian
rm -rf builddir
cd ../
tar -pczf $BUILDNAME.tar.gz $BUILDNAME
rm -rf $BUILDNAME

#create rpm
#cd ../
#mkdir temp
#cp *.deb temp
#cd temp
#fakeroot alien -r *.deb
#mv *.rpm ../
#cd ../
#rm -rf temp
#-rfakeroot

