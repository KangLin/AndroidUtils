TARGET = QtAndroidUtils
TEMPLATE = subdirs

CONFIG += ordered

daemon.file = daemon/daemon.pro

SUBDIRS = \
    android/QtAndroidUtilsModule \
    daemon 

OTHER_FILES += README.md \
    README_zh_CN.md \
    .travis.yml \
    ci/* \
    
