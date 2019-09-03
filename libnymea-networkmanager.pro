TEMPLATE = subdirs
SUBDIRS += libnymea-networkmanager

VERSION_STRING=$$system('dpkg-parsechangelog | sed -n -e "s/^Version: //p"')
