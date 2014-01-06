#!/bin/sh

export ANDROID_HOME=/opt/android-sdk-linux/
export ANDROID_NDK=/opt/android-ndk-r9c
/opt/android-ndk-r9c/ndk-build NDK_DEBUG=1 $1

